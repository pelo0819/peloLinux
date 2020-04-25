#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
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
    ip->ip_p=IPPROTO_TCP;//protocol index
    ip->ip_src = *src;
    ip->ip_dst = *dst;

    ip->ip_sum = 0;
    ip->ip_sum = in_chksum((unsigned short *)ip, ip->ip_hl << 2);
}

/*
Tcpヘッダを生成
*/
static void m_build_tcp(char *p, struct in_addr *src, struct in_addr *dst, unsigned short dport, char *data)
{
    char *ubuf;
    struct ip *ip;
    struct tcphdr *tcp;
    struct pseudo_hdr *pse;
    int needlen;
    
    needlen=sizeof(struct pseudo_hdr) + sizeof(struct tcphdr) + strlen(data);
    if((ubuf = malloc(needlen)) == NULL) errx(1, "malloc");
    memset(ubuf, 0 , needlen);

    pse = (struct pseudo_hdr *)ubuf;
    pse->src.s_addr = src->s_addr;
    pse->dst.s_addr = dst->s_addr;
    pse->proto = IPPROTO_UDP;
    pse->len = htons(sizeof(struct tcphdr) + strlen(data));

    tcp = (struct tcphdr *)(ubuf + sizeof(struct pseudo_hdr));
    
    tcp->source = htons(65001);
    tcp->dest = htons(dport);
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->urg = 0;
    tcp->ack = 0;
    tcp->psh = 0;
    tcp->rst = 0;
    tcp->syn = 0;
    tcp->fin = 0;
    tcp->window = 500;
    tcp->urg_ptr = 0;  
    
    memcpy((char *)tcp + sizeof(struct tcphdr), data, strlen(data));
    tcp->check = in_chksum((unsigned short *)ubuf, needlen);

    ip = (struct ip *)p;
    memcpy(p + (ip->ip_hl << 2), tcp, needlen - sizeof(struct pseudo_hdr));

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
    if(argc != 5) m_usage(argv[0]);

    dport=atoi(argv[3]);
    data=argv[4];

    packetsize=sizeof(struct ip) + sizeof(struct tcphdr) + strlen(data);
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
    m_build_tcp(buf, &src, &dst, dport, data);

    memset(&to, 0, sizeof(struct sockaddr_in));
    to.sin_addr = dst;
    to.sin_port = htons(dport);
    to.sin_family = AF_INET;
    connect(sd, (struct sockaddr *)&to, sizeof(struct sockaddr_in));

    printf("Sending to %s from %s\n" , argv[2], argv[1]);
    if(sendto(sd, buf, packetsize, 0, (struct sockaddr *)&to, tolen) < 0) perror("sendto");

    close(sd);
    free(buf);

    return 0;
}