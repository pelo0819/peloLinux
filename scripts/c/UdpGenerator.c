#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef __FAVOR_BSD
# define __FAVOR_BSD
#endif
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <err.h>

// チェックサム計算用UDP擬似ヘッダ
struct pseudo_hdr{
    struct in_addr src;
    struct in_addr dst;
    unsigned char zero;
    unsigned char proto;
    unsigned short len;
};

/*
入力エラーを通知する
*/
static void m_usage(char *prog)
{
    fprintf(stderr, "Usage: %s <src ip> <dst ip> <port> <string>\n", prog);
    exit(EXIT_FAILURE);
}

/*
チェックサム計算
*/
static unsigned short in_chksum(unsigned short *addr, int len)
{
    int nleft;
    int sum;
    unsigned short *w;
    union{
        unsigned short us;
        unsigned char uc[2];
    } last;
    unsigned short answer;

    nleft = len;
    sum = 0;
    w = addr;

    while(nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if(nleft == 1)
    {
        last.uc[0]= *(unsigned char *)w;
        last.uc[1]= 0;
        sum += last.us;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);

}

/*
IPヘッダを生成
*/
static void m_build_ip(char *p, struct in_addr *src, struct in_addr *dst, size_t len)
{
    struct ip *ip;

    ip=(struct ip *)p;
    ip->ip_v = 4;//version, IPv4 or IPv6
    ip->ip_hl = 5;//length of header , basically 5
    ip->ip_tos = 1;//Type Of Service
    ip->ip_len = len;//length of packet
    ip->ip_id = htons(getpid());//identification
    ip->ip_off = 0;//Not fragmentaion
    ip->ip_ttl=0x40;//Time To Live
    ip->ip_p=IPPROTO_UDP;//protocol index
    ip->ip_src = *src;
    ip->ip_dst = *dst;

    ip->ip_sum = 0;
    ip->ip_sum = in_chksum((unsigned short *)ip, ip->ip_hl << 2);
}

/*
Udpヘッダを生成
*/
static void m_build_udp(char *p, struct in_addr *src, struct in_addr *dst, unsigned short dport, char *data)
{
    char *ubuf;
    struct ip *ip;
    struct udphdr *udp;
    struct pseudo_hdr *pse;
    int needlen;
    
    needlen=sizeof(struct pseudo_hdr) + sizeof(struct udphdr) + strlen(data);
    if((ubuf = malloc(needlen)) == NULL) errx(1, "malloc");
    memset(ubuf, 0 , needlen);

    pse = (struct pseudo_hdr *)ubuf;
    pse->src.s_addr = src->s_addr;
    pse->dst.s_addr = dst->s_addr;
    pse->proto = IPPROTO_UDP;
    pse->len = htons(sizeof(struct udphdr) + strlen(data));

    udp = (struct udphdr *)(ubuf + sizeof(struct pseudo_hdr));
    
    udp->uh_sport = htons(65001);
    udp->uh_dport = htons(dport);
    udp->uh_ulen = pse->len;
    udp->uh_sum = 0;
    
    // udp->source = htons(65001);
    // udp->dest = htons(dport);
    // udp->len = pse->len;
    // udp->check = 0;
    
    memcpy((char *)udp + sizeof(struct udphdr), data, strlen(data));
    udp->uh_sum = in_chksum((unsigned short *)ubuf, needlen);

    ip = (struct ip *)p;
    memcpy(p + (ip->ip_hl << 2), udp, needlen - sizeof(struct pseudo_hdr));

    free(ubuf);
}

/* 
エントリーポイント 
argv[0] : 
argv[1] : 送信元IPアドレス
argv[2] : 送信先IPアドレス
argv[3] : 送信先UDPポート番号
argv[4] : データ文字列
*/
int main(int argc, char *argv[])
{
    int sd;
    int on=1;
    char *data;
    char *buf;
    struct in_addr src;
    struct in_addr dst;
    struct sockaddr_in to;
    socklen_t tolen=sizeof(struct sockaddr_in);
    size_t packetsize;
    unsigned short dport;
    
    // 引数の数が5でなかったら入力エラー
    if(argc!=5) m_usage(argv[0]);

    dport=atoi(argv[3]);
    data=argv[4];

    packetsize=sizeof(struct ip) + sizeof(struct udphdr) + strlen(data);
    if((buf = malloc(packetsize)) == NULL) errx(1, "malloc");

    /*RAWソケットの生成、戻り値はfile descriptor*/
    if((sd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) errx(1, "socket");
    /*送信パケットにIPヘッダを含めるというオプションの設定*/
    if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) errx(1, "setsockopt");

    /*送信元アドレスの設定*/
    src.s_addr=inet_addr(argv[1]);
    /*送信先アドレスの設定*/
    dst.s_addr=inet_addr(argv[2]);

    m_build_ip(buf, &src, &dst, packetsize);
    m_build_udp(buf, &src, &dst, dport, data);

    memset(&to, 0, sizeof(struct sockaddr_in));
    to.sin_addr = dst;
    to.sin_port = htons(dport);
    to.sin_family = AF_INET;
    connect(sd, (struct sockaddr *)&to, sizeof(struct sockaddr_in));

    printf("Senfing to %s from %s\n" , argv[2], argv[1]);
    if(sendto(sd, buf, packetsize, 0, (struct sockaddr *)&to, tolen) < 0) perror("sendto");

    close(sd);
    free(buf);

    return 0;
}