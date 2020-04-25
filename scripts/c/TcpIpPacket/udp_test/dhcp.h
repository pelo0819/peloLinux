#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

//Ethernet header + IP header +UDP header
#define DHCP_UDP_OVERHEAD (14 +  20 + 8)
#define DHCP_SNAME_LEN 64
#define DHCP_FILE_LEN 128
#define DHCP_FIXED_NON_UDP 236
#define DHCP_FIXED_LEN (DHCP_FIXED_NON_UDP + DHCP_UDP_OVERHEAD)
#define DHCP_MTU_MAX 1500
#define DHCP_OPTION_LEN (DHCP_MTU_MAX - DHCP_FIXED_LEN)

struct dhcp_packet
{
    // メッセージタイプ（1 = BOOTREQUEST, 2 = BOOTREPLY）
    u_int8_t op;
    // ハードウェアアドレスタイプ（10Mイーサネットの場合は 1 ）
    u_int8_t htype;
    // ハードウェアアドレス長（10Mイーサネットの場合は 6 ）
    u_int8_t hlen;
    // DHCPリレーエージェントを使用する場合に利用される
    // DHCPリレーエージェントとは
    // DHCPサーバとDHCPクライアントが異なるサブネットに存在しても
    // DHCPクライアントから受信したブロードキャストをユニキャストに変換
    u_int8_t hops;
    // トランザクションID
    // クライアントとサーバ間の一連の通信で共通で使用されるID
    u_int32_t xid;
    // アドレスの更新を開始してからの秒数
    u_int16_t secs;
    // 上位3bits : ブロードキャストフラグ
    // 下位15bits: 常に0をセット
    u_int16_t flags;
    // クライアントがIPアドレスを取得している場合のアドレス
    struct in_addr ciaddr;
    // クライアントに割り当てらるIPアドレス
    struct in_addr yiaddr;
    // サーバーのIPアドレス
    struct in_addr siaddr;
    // リレーエージェントIPアドレス
    struct in_addr giaddr;
    // クライアントMACアドレス
    u_int8_t chaddr[16];
    // サーバーのホスト名
    char sname[DHCP_SNAME_LEN];
    // ブートファイル名
    char file[DHCP_FILE_LEN];
    // オプション
    u_int8_t options[DHCP_OPTION_LEN];
};

// メッセージタイプ候補
#define BOOTREQUEST  1
#define BOOTREPLY    2

// ハードウェアアドレスタイプ候補
#define HTYPE_ETHER  1
#define HTYPE_IEEE802 6
#define HRYPE_FDDI   8

// Magic Cookie DHCP
// Magic Cookieをオプションに明記することでDHCPパケットであることが分かる
// DCHPパケットのfileまではbootpプロトコルと違いはない
#define DHCP_OPTIONS_COOKIE "\143\202\123\143"

// オプション候補
#define DHCPDISCOVER 1
#define DHCPOFFER    2
#define DHCPREQUEST  3
#define DHCPDECLINE  4
#define DHCPACK      5
#define DHCPNAK      6
#define DHCPRELEASE  7
#define DHCPINFORM   8

#define OPTION_STR_MAX 64

typedef struct  {
	int	no;
	char	kind;
	char	*data;
	int	len;
}OPTION;

int print_dhcp(struct dhcp_packet *pa,int size);

u_int8_t *dhcp_set_option(
    u_int8_t *ptr,
    int opno,
    int size,
    u_int8_t *buf
);

int dhcp_get_option(
    struct dhcp_packet *pa,
    int size,
    int opno,
    void *val
);

int MakeDhcpRequest(
    struct dhcp_packet *pa,
    u_int8_t mtype,
    struct in_addr *ciaddr,
    struct in_addr *req_ip,
    struct in_addr *server
);

int DhcpSendDiscover(int soc);

int DhcpSeneRequest(
    int soc,
    struct in_addr *yiaddr,
    struct in_addr *server
);

int DhcpSendRequestUni(int soc);

int DhcpSendRelease(int soc);

int DhcpRecv(
    int soc,
    u_int8_t *data,
    int len,
    struct ether_header *eh,
    struct ip *ip,
    struct udphdr *udp
);

int DhcpCheck(int soc);



