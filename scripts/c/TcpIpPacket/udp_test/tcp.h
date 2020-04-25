
int print_tcp(struct tcphdr *tcp);

int TcpSearchTable(u_int16_t port);

u_int16_t TcpChecksum(
    struct in_addr *saddr,
    struct in_addr *daddr,
    u_int8_t proto,
    u_int8_t *data,
    int len
);

int TcpSocketClose(u_int16_t port);

int TcpRecv(
    int soc, 
    struct ether_header *eh, 
    struct ip *ip,
    u_int8_t *data,
    int len
);

int TcpSendAck(int soc, int no);