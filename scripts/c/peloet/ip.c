#include	<stdio.h>
#include	<ctype.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<limits.h>
#include	<time.h>
#include	<sys/ioctl.h>
#include	<netinet/ip.h>
#include	<netinet/ip_icmp.h>
#include	<netinet/if_ether.h>
#include	<netinet/udp.h>
#include    <netinet/tcp.h>
#include	<arpa/inet.h>
#include	"sock.h"
#include	"ether.h"
#include	"arp.h"
#include	"ip.h"
#include	"udp.h"
#include    "tcp.h"
#include	"icmp.h"
#include	"param.h"

extern PARAM Param;

#define IP_RECV_BUF_NO (16)

typedef struct
{
    time_t timestamp;
    int id;
    u_int8_t data[64 * 1024];
    int len;
} IP_RECV_BUF;

IP_RECV_BUF IpRecvBuf[IP_RECV_BUF_NO];

void print_ip(struct ip *ip)
{
    static char *proto[] =
    {
        "undefined",
        "ICMP",
        "IGMP",
        "undefined",
        "IPIP",
        "undefined",
        "TCP",
        "undefined",
        "EGP",
        "undefined",
        "undefined",
        "undefined",
        "PUP",
        "undefined",
        "undefined",
        "undefined",
        "undefined",
        "UDP",
    };

    char buf1[80];
    // printf("ip header----------------------------------------\n"); 
    printf("--- ip header ---\n[\n");   
    printf("ip_v = %u , ", ip->ip_v);
    printf("ip_hl = %u , ", ip->ip_hl);
    printf("ip_tos = %x , ", ip->ip_tos);
    printf("ip_len = %d , ", ntohs(ip->ip_len));
    printf("ip_id = %u , ", ntohs(ip->ip_id));
    printf("ip_off=%x,%d\n", (ntohs(ip->ip_off)) >> 13 & 0x07, ntohs(ip->ip_off) & IP_OFFMASK);
    printf("ip_ttl = %u , ", ip->ip_ttl);
    printf("ip_p = %u , ", ip->ip_p);
    if(ip->ip_p <= 17)
    {
        printf("(%s),", proto[ip->ip_p]);
    }
    else
    {
        printf("(undefined),");
    }

    printf("ip_sum = %4x\n", ntohs(ip->ip_sum));
    printf("ip_src = %s\n", inet_ntop(AF_INET, &ip->ip_src, buf1, sizeof(buf1)));
    printf("ip_dst = %s\n", inet_ntop(AF_INET, &ip->ip_dst, buf1, sizeof(buf1)));
    printf("]\n");
    // printf("ip header----------------------------------------\n");    

    return;
}

int IpRecvBufInit(){
    int i;
    for(i=0;i<IP_RECV_BUF_NO;i++)
    {
        IpRecvBuf[i].id = -1;
    }
    return 0;
}

/*
断片化されたipデータに新たに受信した断片化ipデータを合体させるため、
断片化されたipデータがIpRecvBuf配列のインデックスを戻す関数
新規の断片化ipデータなら空きのインデックスを戻す
引数は識別子、ここで識別子：元は一つのデータであったことがわかるようにするため、
断片化されたデータ毎に同一の値で与えられるインデックス
断片化=フラグメント化、ワタシ横文字キライ
*/
int IpRecvBufAdd(u_int16_t id)
{
    int ret = 0;
    int freeNo = -1;
    int oldestNo = getOldestBufIndex();
    time_t oldestTime = ULONG_MAX;

    int i;

    for(i = 0; i < IP_RECV_BUF_NO; i++)
    {
        // バッファに空きがあるなら、freeNoに代入
        if(IpRecvBuf[i].id == -1)
        {
            freeNo = i;
        }
        else
        {
            // 識別子が同じならそのデータが格納されているインデックスを返す
            if(IpRecvBuf[i].id == id)
            {
                return i;
            }
        }
    }

    // 空きバッファが一つもない場合は一番古いバッファインデックスを返す
    // 空きバッファがあるならそのインデックス
    if(freeNo == -1) ret = oldestNo;
    else ret = freeNo;

    // バッファの情報を設定しておく、dataは別箇所で設定
    IpRecvBuf[ret].timestamp = time(NULL);
    IpRecvBuf[ret].id = id;
    IpRecvBuf[ret].len = 0;

    return ret;
}

int getOldestBufIndex()
{
    int ret = 0;
    int oldestTime = IpRecvBuf[0].timestamp;
    int i;
    for(i = 1; i < IP_RECV_BUF_NO; i++)
    {
        if(oldestTime >= IpRecvBuf[i].timestamp)
        {
            oldestTime = IpRecvBuf[i].timestamp;
            ret = i;
        }
    }
    return ret;
}

/*
空きバッファを削除する関数
厳密にはインデックスを-1にしていれおけば空きバッファと認識されるため、
ちゃんときれいにしている訳ではない
*/
int IpRecvBufDel(u_int16_t id)
{
    int i;
    for(i=0; i <IP_RECV_BUF_NO; i++)
    {
        if(IpRecvBuf[i].id == id)
        {
            IpRecvBuf[i].id = -1;
            return 1;
        }
    }
    return 0;
}


int IpRecv(int soc, u_int8_t *raw, int raw_len, struct ether_header *eh, u_int8_t *data, int len)
{
    struct ip *ip;
    u_int8_t option[1500];// MTUサイズ
    u_int16_t sum;
    int optionLen;
    int no;
    int off;
    int plen;
    u_int8_t *ptr = data;

    char *d = (char *)data;

    if(len < (int)sizeof(struct ip))
    {
        printf("len(%d) < sizeof(struct ip)\n", len);
        return -1;
    }

    ip = (struct ip *)ptr;
    ptr += sizeof(struct ip);
    len -= sizeof(struct ip);

    char buf1[80];
    char buf2[80];

    if(Param.vip.s_addr != 0)
    {
        inet_ntop(AF_INET, &Param.vip.s_addr, buf1, sizeof(buf1));
        inet_ntop(AF_INET, &ip->ip_dst.s_addr, buf2, sizeof(buf2));
        if(strcmp(buf1, buf2) != 0)
        {
            // printf("recv Ip packet but not for me.\n");
            return 0;
        }
    }
    

    optionLen = ip->ip_hl * 4 -sizeof(struct ip);

    if(optionLen > 0)
    {
        if(optionLen >= 1500)
        {
            printf("IP optionLen(%d) too big\n", optionLen);
            return -1;
        }
        memcpy(option, ptr, optionLen);
        ptr += optionLen;
        len -= optionLen;
    }

    if(optionLen == 0)
    {
        sum = checksum((u_int8_t *)ip, sizeof(struct ip));
    }
    else
    {
        sum = checksum2((u_int8_t *)ip, sizeof(struct ip), option, optionLen);
    }
    if(sum != 0 && sum != 0xFFFF)
    {
        printf("bad ip checksum\n");
        return -1;
    }

    ArpAddTable(eh->ether_shost, &ip->ip_src);

    // IPペイロードのバイト長を格納
    plen = ntohs(ip->ip_len) - ip->ip_hl * 4;

    // 空きバッファのインデックスを取得
    no = IpRecvBufAdd(ip->ip_id);
    // bit演算子よりオフセット長が最大値を超えないようにして設定
    off = (ntohs(ip->ip_off) & IP_OFFMASK) * 8;
    memcpy(IpRecvBuf[no].data + off, ptr, plen);

    if(!(ntohs(ip->ip_off) & IP_MF))
    {
        IpRecvBuf[no].len = off + plen;
        if(ip->ip_p == IPPROTO_ICMP)
        {
            IcmpRecv(
                soc,
                raw,
                raw_len,
                eh,
                ip,
                IpRecvBuf[no].data,
                IpRecvBuf[no].len
            );
        }
        else if(ip->ip_p == IPPROTO_UDP)
        {
            UdpRecv(soc, eh, ip, IpRecvBuf[no].data, IpRecvBuf[no].len);
        }
        else if(ip->ip_p == IPPROTO_TCP)
        {
            TcpRecv(soc, eh, ip, IpRecvBuf[no].data, IpRecvBuf[no].len);
        }

        IpRecvBufDel((ntohs(ip->ip_id)));
    }

    return 0;
}

int IpSendLink(
    int soc,
    u_int8_t smac[6],
    u_int8_t dmac[6],
    struct in_addr *saddr,
    struct in_addr *daddr,
    u_int8_t proto,
    int dontFlagment,
    int ttl,
    u_int8_t *data,
    int len)
{
    struct ip *ip;
    u_int8_t *dptr;
    u_int8_t *ptr;
    u_int8_t sbuf[ETHERMTU]; // IPパケット分のメモリを確保
    u_int16_t id; //識別子
    int lest; // 残りパケット量(byte)
    int sndLen; // １回で送信したパケット量(byte)
    int off; // オフセット(先頭から何バイト目か)
    int flagment; // フラグメント化するかしないか、する=1

    // フラグメント化しないでサイズが大きすぎたら送信しない
    if(dontFlagment && len>Param.MTU - sizeof(struct ip))
    {
        printf("IpSend:data too long:%d\n", len);
        return -1;
    }

    // 識別子をランダムで設定
    id = random();

    // フラグメント化を想定してデータをキャッシュしておく
    dptr = data;

    // 残りパケット量の初期化
    lest = len;
    
    // すべてのデータを送信し終わるまでループ
    while(lest > 0)
    {
        // フラグメント化する場合
        if(lest > Param.MTU - sizeof(struct ip))
        {
            sndLen = (Param.MTU - sizeof(struct ip)) / 8 * 8;
            flagment = 1;
        }
        else// フラグメント化しない場合
        {
            sndLen = lest;
            flagment = 0;
        }

        // 送信するパケット分を確保したメモリの先頭アドレスをptrに代入
        ptr = sbuf;
        // 先頭アドレスからipパケット分の記憶方法をipに変換
        ip = (struct ip *)ptr;
        
        // ipパケット分を0にセット
        memset(ip, 0, sizeof(struct ip));
        
        // Ipv4に設定
        ip->ip_v = 4;

        // ヘッダー長を設定(基本的に5,20byteだから)
        ip->ip_hl = 5;

        // パケット長を設定(ipヘッダー＋ipペイロード)
        ip->ip_len = htons(sizeof(struct ip) + sndLen);

        // 識別子を設定
        ip->ip_id = htons(id);

        // フラグとフラグメントオフセットを設定
        off = (dptr - data) / 8;

        // if(dontFlagment == 0)
        // {
        //     ip->ip_off = htons(IP_DF);
        // }
        // else if(flagment == 0)
        // {
        //     ip->ip_off = htons((IP_MF) | (off & IP_OFFMASK));
        // }
        // else
        // {
        //     ip->ip_off = htons((0) | (off & IP_OFFMASK));
        // }

        if(dontFlagment)
        {
			ip->ip_off=htons(IP_DF);
		}
		else if(flagment)
        {
			ip->ip_off=htons((IP_MF)|(off&IP_OFFMASK));
		}
		else
        {
			ip->ip_off=htons((0)|(off&IP_OFFMASK));
		}

        // time to live を設定
        ip->ip_ttl = ttl;

        // ペイロードのプロトコルが何かを設定
        ip->ip_p = proto;

        // 送信元、先のIPアドレスを設定
        ip->ip_src.s_addr = saddr->s_addr;
        ip->ip_dst.s_addr = daddr->s_addr;

        // チェックサムを設定
        ip->ip_sum = 0;// いんの？
        ip->ip_sum = checksum((u_int8_t *)ip, sizeof(struct ip));
        
        ptr += sizeof(struct ip);
        
        memcpy(ptr, dptr, sndLen);
        
        ptr += sndLen;

        EtherSend(soc, smac, dmac, ETHERTYPE_IP, sbuf, ptr - sbuf);
        print_ip(ip);

        dptr += sndLen;
        lest -= sndLen;
    }
    return 0;
}

int IpSend(
    int soc, 
    struct  in_addr *saddr, 
    struct in_addr *daddr, 
    u_int8_t proto, 
    int dontFlagment, 
    int ttl, 
    u_int8_t *data, 
    int len)
{
    u_int8_t dmac[6];
    char buf[80];
    int ret;

    if(GetTargetMac(soc, daddr, dmac, 0))
    {
        ret = IpSendLink(
            soc,
            Param.vmac,
            dmac,
            saddr,
            daddr,
            proto,
            dontFlagment,
            ttl,
            data,
            len
        );
    }
    else
    {
        printf(
            "IpSend:%s Destination Host Unreachable\n", 
            inet_ntop(AF_INET, daddr, buf, sizeof(buf)));
        ret = -1;
    }
    return ret;
}

