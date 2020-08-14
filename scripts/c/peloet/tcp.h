
int print_tcp(struct tcphdr *tcp);
int print_tcp_optpad(unsigned char *data,int size);
char *TcpStatusStr(int status);

u_int16_t TcpChecksum(
    struct in_addr *saddr,
    struct in_addr *daddr,
    u_int8_t proto,
    u_int8_t *data,
    int len
);

int TcpAddTable(u_int16_t port);
int TcpSearchTable(u_int16_t port);
int TcpShowTable();
u_int16_t TcpSearchFreePort();

int TcpSocketListen(u_int16_t port);
int TcpSocketClose(u_int16_t port);

int TcpSendSyn(int soc,int no,int ackFlag);
int TcpSendAck(int soc, int no);
int TcpSendFin(int soc,int no);
int TcpSendRst(int soc,int no);

int TcpSendRstDirect(
    int soc,
    struct ether_header *r_eh,
    struct ip *r_ip,
    struct tcphdr *r_tcp
);
int TcpConnect(
    int soc,
    u_int16_t sport,
    struct in_addr *daddr,
    u_int16_t dport
);
int TcpClose(int soc,u_int16_t sport);
int TcpReset(int soc,u_int16_t sport);
int TcpAllSocketClose(int soc);
int TcpSendData(int soc,u_int16_t sport,u_int8_t *data,int len);
int TcpSend(int soc,u_int16_t sport,u_int8_t *data,int len);

int TcpRecv(
    int soc, 
    struct ether_header *eh, 
    struct ip *ip,
    u_int8_t *data,
    int len
);