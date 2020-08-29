#include <sys/time.h> // timeval , gettimeofday
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> // strtol()
#include <string.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include "sock.h"
#include "ether.h"
#include "ip.h"
#include "icmp.h"
#include "param.h"

extern PARAM Param;

#define ECHO_HDR_SIZE (8)
#define PING_SEND_NO (4)

typedef struct{
    struct timeval sendTime;
}PING_DATA;

PING_DATA PingData[PING_SEND_NO];

int print_icmp(struct icmp *icmp)
{
    static char *icmp_type[] =
    {
        "Echo Reply",
        "undefined",
        "undefined",
        "Destination Unreachable",
        "Source Quench",
        "Redirect",
        "undefined",
        "undefined",
        "Echo Request",
        "Router Adverisement",
        "Router Selection",
        "Time Exceeded for Datagram",
        "Parameter Problem on Datagram",
        "Timestamp Request",
        "Timestamp Reply",
        "Address Mask Request",
        "Address Mask Reply",
    };

    // printf("icmp --------------------------------------------\n");
    printf("--- icmp ---\n[\n");
    printf("icmp_type = %u", icmp->icmp_type);
    if(icmp->icmp_type <= 18)
    {
        printf("(%s),", icmp_type[icmp->icmp_type]);
    }
    else
    {
        printf("(undefined),");
    }

    printf("icmp_code = %u,", icmp->icmp_code);
    printf("icmp_cksum = %u\n", ntohs(icmp->icmp_cksum));

    if(icmp->icmp_type == 0 || icmp->icmp_type == 8)
    {
        printf("icmp_id = %u,", ntohs(icmp->icmp_id));
        printf("icmp_seq = %u\n", ntohs(icmp->icmp_seq));
    }
    printf("]\n");
    printf("--- icmp ---\n");
    // printf("icmp --------------------------------------------\n");

}

int IcmpSendEchoReply(
    int soc,
    struct ip *r_ip,
    struct icmp *r_icmp,
    u_int8_t *data,
    int len,
    int ip_ttl)
{
    u_int8_t *ptr;
    u_int8_t sbuf[64 * 1024];
    struct icmp *icmp;

    ptr = sbuf;
    icmp = (struct icmp *)ptr;
    memset(icmp, 0, sizeof(struct icmp));
    icmp->icmp_type = ICMP_ECHOREPLY;
    icmp->icmp_code = 0;
    icmp->icmp_hun.ih_idseq.icd_id = r_icmp->icmp_hun.ih_idseq.icd_id;
    icmp->icmp_hun.ih_idseq.icd_seq = r_icmp->icmp_hun.ih_idseq.icd_seq;
    icmp->icmp_cksum = 0;

    ptr += ECHO_HDR_SIZE;

    memcpy(ptr, data, len);
    ptr += len;

    icmp->icmp_cksum = checksum(sbuf, ptr - sbuf);

    //printf("--- ICMP Reply ---\n[\n");
    IpSend(
        soc,
        &r_ip->ip_dst,
        &r_ip->ip_src,
        IPPROTO_ICMP,
        0,
        ip_ttl,
        sbuf,
        ptr-sbuf);
    //print_icmp(icmp);
    //printf("]\n");

    return 0;
}

/*
ICMPエコーを送信
*/
int IcmpSendEcho(
    int soc,
    struct in_addr *daddr,
    int seqNo,
    int size)
{
    int psize;
    u_int8_t *ptr;
    u_int8_t sbuf[64 * 1024];
    struct icmp *icmp;

    // 送信するパケット分を確保したメモリの先頭アドレスをptrに代入
    ptr = sbuf;

    // 先頭アドレスからicmpパケット分の記憶方法をipに変換
    icmp = (struct icmp *)ptr;

    // icmpパケット分を0にセット
    memset(icmp, 0, sizeof(struct icmp));

    // icmpの初期化が完了したので、設定していく
    // タイプをエコー要求に設定
    icmp->icmp_type = ICMP_ECHO;

    // エコー要求のコードは0だけなので0
    icmp->icmp_code = 0;

    // ICMPヘッダの識別子はプロセス番号なのでそのように設定
    icmp->icmp_hun.ih_idseq.icd_id = htons((u_int16_t)getpid());

    // シーケンス番号を設定
    icmp->icmp_hun.ih_idseq.icd_seq = htons((u_int16_t)seqNo);

    // チェックサムを初期化　いる？
    icmp->icmp_cksum = 0;

    ptr += ECHO_HDR_SIZE;

    // ICMPパケットのペイロードをセット、ただのカウントアップ
    psize = size - ECHO_HDR_SIZE;

    int i;
    for(i = 0; i < psize; i++)
    {
        // 上限値は255
        *ptr = (i & 0xFF);
        ptr++;
    }

    // ICMPチェックサムを設定
    icmp->icmp_cksum = checksum((u_int8_t *)sbuf, ptr - sbuf);

    //printf("--- ICMP Echo ---\n[\n");
    IpSend(
        soc,
        &Param.vip,
        daddr,
        IPPROTO_ICMP,
        0,
        Param.IpTTL,
        sbuf,
        ptr - sbuf
    );
    //print_icmp(icmp);
    //printf("]\n");

    gettimeofday(&PingData[seqNo - 1].sendTime, NULL);

    return 0;
}

int IcmpSendDestinationUnreachable(
    int soc,
    struct in_addr *daddr,
    struct ip *ip,
    u_int8_t *data,
    int len
)
{
    u_int8_t *ptr;
    u_int8_t sbuf[64 * 1024];
    struct icmp *icmp;

    ptr = sbuf;
    icmp = (struct icmp *)ptr;
    memset(icmp, 0, sizeof(struct icmp));
    icmp->icmp_type = ICMP_DEST_UNREACH;
    icmp->icmp_code = ICMP_DEST_UNREACH;
    icmp->icmp_cksum = 0;

    ptr += ECHO_HDR_SIZE;

    memcpy(ptr, ip, sizeof(struct ip));
    ptr += sizeof(struct ip);

    if(len >= 64)
    {
        memcpy(ptr, data, 64);
        ptr += 64;
    }
    else
    {
        memcpy(ptr, data, len);
        ptr += len;
    }

    icmp->icmp_cksum = checksum((u_int8_t *)sbuf, ptr - sbuf);

    //printf("=== ICMP Destination Unreachable ===[\n");
    IpSend(
        soc,
        &Param.vip,
        daddr,
        IPPROTO_ICMP,
        0,
        Param.IpTTL,
        sbuf,
        ptr - sbuf
    );

    //print_icmp(icmp);
    //printf(")\n");

    return 0;
}

/*
Pingを送信
*/
int PingSend(
    int soc,
    struct in_addr *daddr,
    int size)
{
    int i;
    for(i = 0; i < PING_SEND_NO; i++)
    {
        IcmpSendEcho(soc, daddr, i+1, size);
        sleep(1);
    }

    return 0;
}

/*
ICMPパケットを受信した
*/
int IcmpRecv(
    int soc,
    u_int8_t *raw,
    int raw_len,
    struct ether_header *eh,
    struct ip *ip,
    u_int8_t *data,
    int len)
{
    struct icmp *icmp;
    u_int16_t sum;
    int icmpSize;
    u_int8_t *ptr = data;

    icmpSize = len;

    icmp = (struct icmp *)ptr;
    ptr += ECHO_HDR_SIZE;
    len -= ECHO_HDR_SIZE;

    sum = checksum((u_int8_t *)icmp, icmpSize);
    if(sum != 0 && sum != 0xFFFF)
    {
        printf("bad icmp checksum(%x, %x)\n", sum, icmp->icmp_cksum);
        return -1;
    }

    // 自分宛に来ているか
    if(isTargetIPAddr(&ip->ip_dst))
    {
        //printf("--- recv ---[\n");
        //print_ether_header(eh);
        //print_ip(ip);
        //print_icmp(icmp);
        //printf("]\n");
        if(icmp->icmp_type == ICMP_ECHO)
        {
            //printf("エコー要求に応答します\n");
            IcmpSendEchoReply(soc, ip, icmp, ptr, len, Param.IpTTL);
        }
        else if(icmp->icmp_type == ICMP_ECHOREPLY)
        {
            //printf("こちらからのエコー要求に対する応答がきました\n");
            PingCheckReply(ip, icmp);
        }
    }
    else
    {
        printf("私宛ではないです\n");
    }
    return 0;
}

/*
ICMPエコー応答を受信した場合のチェック関数
args
 *ip:IPパケット
 *icmp:ICMPパケット
*/
int PingCheckReply(
    struct ip *ip,
    struct icmp *icmp)
{
    char buf[80];

    // ICMPパケットのIDがプロセスIDと一致しているか
    // Linuxならね、Windowsなら1
    if(ntohs(icmp->icmp_id) == getpid())
    {
        // シーケンス版棒
        int seqNo = ntohs(icmp->icmp_seq);
        if(seqNo > 0 && seqNo <= PING_SEND_NO)
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            int sec = tv.tv_sec - PingData[seqNo - 1].sendTime.tv_sec;
            int usec = tv.tv_usec - PingData[seqNo - 1].sendTime.tv_usec;
            if(usec < 0)
            {
                sec--;
                usec = 10000 - usec;
            }

            // 受信したICMPエコー応答結果を出力
            //printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d.%03d ms\n",
            //    ntohs(ip->ip_len),
            //    inet_ntop(AF_INET, &ip->ip_src, buf, sizeof(buf)),
            //    ntohs(icmp->icmp_seq),
            //    ip->ip_ttl,
            //    sec,
            //    usec);
        }
    }
    return 0;
}