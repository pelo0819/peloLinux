#include <netinet/ip_icmp.h>

int print_icmp(struct icmp *icmp);

int IcmpSendEchoReply(
    int soc, 
    struct ip *r_ip, 
    struct icmp *r_icmp, 
    u_int8_t *data, 
    int len, 
    int ip_ttl);

int IcmpRecv(
    int soc, 
    u_int8_t *raw, 
    int raw_len, 
    struct ether_header *eh, 
    struct ip *ip, 
    u_int8_t *data, 
    int len);

int PingCheckReply(
    struct ip *ip,
    struct icmp *icmp);