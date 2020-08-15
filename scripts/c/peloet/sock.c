#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <netpacket/packet.h>// sockaddr_ll
#include <net/if.h>// ifreq
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <stdint.h>

#include "sock.h"
#include "param.h"

u_int16_t checksum(u_int8_t *data, int len)
{
    u_int32_t sum;
    u_int16_t *ptr;
    int c;

    sum = 0;
    ptr = (u_int16_t *)data;

    for(c = len; c > 1; c-=2)
    {
        sum += (*ptr);
        if(sum & 0x80000000)
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ptr++;
    }

    if(c == 1)
    {
        u_int16_t val;
        val = 0;
        memcpy(&val, ptr, sizeof(u_int8_t));
        sum += val;
    }

    while(sum >> 16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

u_int16_t checksum2(u_int8_t *data1, int len1, u_int8_t *data2, int len2)
{
    u_int32_t sum;
    u_int16_t *ptr;
    int c;

    sum = 0;
    ptr = (u_int16_t *)data1;

    for(c = len1; c > 1; c-=2)
    {
        sum += (*ptr);
        if(sum & 0x80000000)
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ptr++;
    }
    if(c == 1)
    {
        u_int16_t val;
        val = ((*ptr) << 8) + (*data2);
        sum += val;
        if(sum & 0x80000000)
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ptr = (u_int16_t *)(data2 + 1);
        len2--;
    }
    else
    {
        ptr = (u_int16_t *)data2;
    }

    for(c = len2; c > 1; c -=2)
    {
        sum += (*ptr);
        if(sum & 0x80000000)
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ptr++;
    }

    if(c == 1)
    {
        u_int16_t val;
        val = 0;
        memcpy(&val, ptr, sizeof(u_int16_t));
        sum += val;
    }

    while(sum >> 16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

/*
なんか待ちたい時に使う
*/
int DummyWait(int ms)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = ms * 1000 * 1000;
    nanosleep(&ts, NULL);
    return 0;
}



/// device:インターフェイス名、enp0s3とか
int init_sock(char *device)
{
    // Network Interface Card(NIC)のパラメタを格納する構造体
    struct ifreq if_req;
    // sockaddr_llはデバイスに依存しない物理層のアドレス、は？
    struct sockaddr_ll sa;
    // ディスクリプタ
    int soc;

    // ソケットを作成するシステムコール、戻り値はファイルディスクリプタ
    // ディスクリプタ：プログラムがアクセスする標準入出力をOSが識別するために用いる識別子
    // arg1：AF_PACKET(PF_PACKET)を設定することで、生パケットをアプリケーションで扱える
    // arg2：SOCK_RAWならデータリンク層から扱える、SOCK_DGRAMならネットワーク層の生データを扱える
    // arg3：受信するイーサネットタイプを指定、
    //         ETH_P_IPならIPパケットのみを受信、ETH_P_ARPならARPパケットのみを受信、今回は全部(ETH_P_ALL)
    if((soc =socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL) )) < 0 )
    {
        perror("socket");
        return -1;
    }

    // printf("soc index in init_sock : %d\n", soc);

    strcpy(if_req.ifr_name, device);
    
    // IOコントロール
    // ネットワークインターフェイス名からインターフェイス番号を取得
    if(ioctl(soc, SIOCGIFINDEX, &if_req) < 0)
    {
        perror("ioctl");
        close(soc);
        return -1;
    }
    
    // socketを生成しただけなので、パラメタをif_reqにバインド
    // 生パケットを扱えるよう「PF_PACKET」を指定、AF_PACKETとの違いは不明
    sa.sll_family = PF_PACKET; 
    sa.sll_protocol = htons(ETH_P_ALL);
    sa.sll_ifindex = if_req.ifr_ifindex;
    if(bind(soc, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        perror("bind");
        close(soc);
        return -1;
    }

    // NICで機能フラグワードをif_reqに教える
    // 言い換えるとネットインターフェイスの状態フラグ(状態)をif_reqに保存
    if(ioctl(soc, SIOCGIFFLAGS, &if_req) < 0)
    {
        perror("ioctl");
        close(soc);
        return -1;
    }

    // if_reqのインターフェイスのフラグをビット演算子で設定
    // プレミスキャストモードにインターフェイスを動作中に設定
    if_req.ifr_flags = if_req.ifr_flags | IFF_PROMISC | IFF_UP;
    if(ioctl(soc, SIOCSIFFLAGS, &if_req) < 0)
    {
        perror("ioctl");
        close(soc);
        return -1;
    }

    return soc;
}

int GetMacAddress(char *device, u_int8_t *hwaddr)
{
    struct ifreq ifreq;
    int soc;
    u_int8_t *p;

    if((soc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("GetMacAddress():sock");
        return -1;
    }

    strncpy(ifreq.ifr_name, device, sizeof(ifreq.ifr_name)-1);
    
    // ifreqにMACアドレスを設定
    if(ioctl(soc, SIOCGIFHWADDR, &ifreq) == -1)
    {
        perror("GetMacAddress():ioctl:hwaddr");
        close(soc);
        return -1;
    }
    else
    {
        p = (u_int8_t *)&ifreq.ifr_hwaddr.sa_data;

        int i;
        printf("ifreq.ifr_hwaddr.sa_data : ");
        for(i=0; i< sizeof ifreq.ifr_hwaddr.sa_data; i++)
        {
            printf("%d:", p[i]);    
        }
        printf("\n");
        
        memcpy(hwaddr, p, 6);
        close(soc);
        return 1;
    }

}