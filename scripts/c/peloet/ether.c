#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h> // strtol()
#include <string.h> // strdup(), strtok_r()
#include <limits.h>
#include <time.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <linux/if.h>
#include <arpa/inet.h>

#include "sock.h"
#include "param.h"
#include "ether.h"
#include "arp.h"
#include "arp_poison.h"
#include "ip.h"
#include "icmp.h"

extern PARAM Param;

u_int8_t AllZeroMac[6] = {0,0,0,0,0,0};
u_int8_t BcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// u_int8_t : 8ビット符号付き整数
int my_ether_aton(char *str, u_int8_t *mac)
{
    char *ptr = NULL;
    char *saveptr = NULL;
    int c;
    // 文字列を複製
    char *tmp = strdup(str);

    for(c=0, ptr = strtok_r(tmp, ":", &saveptr); c<6; c++, ptr = strtok_r(NULL, ":", &saveptr))
    {
        if(ptr == NULL)
        {
            free(tmp);
            return -1;
        }
        // 16進数で表記されたMACアドレスを整数に変換
        mac[c] = strtol(ptr, NULL, 16);
    }
    free(tmp);
    return 0;
}

void print_ether_header(struct ether_header *eh)
{
    char buf1[80];
    printf("--- ether header ---\n");
    printf("[\n");
    printf("ether_shost = %s\n", my_ether_ntoa_r(eh->ether_shost, buf1));
    printf("ether_dhost = %s\n", my_ether_ntoa_r(eh->ether_dhost, buf1));

    printf("ether-type = %02X", ntohs(eh->ether_type));
    switch (ntohs(eh->ether_type))
    {
        case ETHERTYPE_PUP:
            printf("(Xerox PUP)\n");
            break;
        case ETHERTYPE_IP:
            printf("(IP)\n");
            break;
        case ETHERTYPE_ARP:
            printf("(Address resolution)\n");
            break;
        case ETHERTYPE_REVARP:
            printf("(Reverse ARP)\n");
            break;
        default:
            printf("(unknown)\n");
            break;
    }
    printf("]\n");
    // printf("ehter header-------------------------------------\n");

    return;
}


char *my_ether_ntoa_r(u_int8_t *hwaddr, char *buf)
{
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
     hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
    return buf;
}

int EtherRecv(int soc, u_int8_t *in_ptr, int in_len)
{
    struct ether_header *eh;
    u_int8_t *ptr = in_ptr;
    int len = in_len;
    
    eh = (struct ether_header *)ptr;
    ptr += sizeof(struct ether_header);
    len -= sizeof(struct ether_header);

    // if(ntohs(eh->ether_type) != ETHERTYPE_ARP)
    // {
    //     print_ether_header(eh);
    // }

    // ブロードキャストでも自分宛て(設定ファイル)でもなければ無視
    // TODO:無視するかしないか、IPアドレスの指定を動的に行えるようにせよ
    // if(memcmp(eh->ether_dhost, BcastMac, 6) != 0 && memcmp(eh->ether_dhost, Param.vmac, 6) != 0)
    // {
    //     return -1;
    // }

    //char t_ip = "192.168.3.9";
    //struct in_addr t_addr;
    //inet_aton("192.168.3.9", &t_addr);
    // u_int8_t t_mac[6];
    // ArpSearchTable(&t_addr, t_mac);
    
    //if(memcmp(eh->ether_dhost, t_mac, 6) != 0 && memcmp(eh->ether_shost, t_mac, 6) != 0)
    //{
    //    return -1;
    //}

    TransferPacket(eh, ptr, len, in_ptr, in_len);

    if (ntohs(eh->ether_type) == ETHERTYPE_ARP) 
    {
        ArpRecv(soc, eh, ptr, len);
    }
    else if(ntohs(eh->ether_type) == ETHERTYPE_IP)
    {
        IpRecv(soc, in_ptr, in_len, eh, ptr, len);
    }
    else
    {
        // printf("Recv packet not Arp and not IP\n");
    }
    
    
    return 0;
}

/*
イーサネットヘッダを送信する関数
args
 soc:ファイルディスクリプタ
 smac:送信元MACアドレス
 dmac:送信先MACアドレス
 type:L3層でどのプロトコルを使用しているか
 *data:ペイロード
 len:ペイロード長
*/
int EtherSend(
    int soc, 
    u_int8_t smac[6], 
    u_int8_t dmac[6],
    u_int16_t type, 
    u_int8_t *data, 
    int len)
{
    struct ether_header *eh;
    u_int8_t *ptr;
    // Etherパケットの最大値1500byte分のメモリを確保
    u_int8_t sbuf[sizeof(struct ether_header) + ETHERMTU];
    int padlen;

    // ペイロード長が1500byte以上だったら大きすぎるのでリターン
    if(len > ETHERMTU)
    {
        printf("EtherSend:data too long:%d\n", len);
        return -1;
    }

    // ptrにetherパケットの先頭のアドレスを代入
    ptr = sbuf;

    // etherパケットの先頭からether_headerのサイズ分を
    // ether_headerの記憶方法に変換
    eh = (struct ether_header *)ptr;
    
    // ether_header分のメモリの中身を0にセット
    memset(eh, 0, sizeof(struct ether_header));
    
    // あとは普通にether_headの中身を設定していく
    // 送信先MACアドレスを設定
    memcpy(eh->ether_dhost, dmac, 6);
    
    // 送信元MACアドレスを設定
    memcpy(eh->ether_shost, smac, 6);
    
    // L3層のプロトコルタイプを設定
    eh->ether_type = htons(type);

    // ether_headerの設定が完了したので、
    // アドレスをether_header分ズラス
    ptr += sizeof(struct ether_header);

    // ずらした位置からペイロードをセット
    memcpy(ptr, data, len);

    // ペイロード分アドレスをズラス
    ptr += len;

    // 現在のアドレスと最初のアドレスの差分をみて、
    // 規定の最小パケットサイズ(60byte)未満でないかを確認
    // 小さいようであれば、60byteに達するまで0で埋める
    if((ptr - sbuf) < ETH_ZLEN)
    {
        padlen = ETH_ZLEN - (ptr - sbuf);
        memset(ptr, 0, padlen);
        ptr += padlen;
    }

    // 送信するパケットが完成したので、あとは送る
    write(soc, sbuf, ptr - sbuf);
    //print_ether_header(eh);

    return 0;

}

int EtherTransfer(
    int soc, 
    struct ether_header *eh_def,
    u_int8_t mac[6],
    u_int8_t *data,
    int len
)
{
    struct ether_header *eh;
    u_int8_t *ptr;
    // Etherパケットの最大値1500byte分のメモリを確保
    u_int8_t sbuf[sizeof(struct ether_header) + ETHERMTU];
    int padlen;

    // ptrにetherパケットの先頭のアドレスを代入
    ptr = sbuf;

    // etherパケットの先頭からether_headerのサイズ分を
    // ether_headerの記憶方法に変換
    eh = (struct ether_header *)ptr;

    printf("ether transfer1\n");
    
    // ether_header分のメモリの中身を0にセット
    memset(eh, 0, sizeof(struct ether_header));
    
    // あとは普通にether_headの中身を設定していく
    // 送信先MACアドレスを設定
    memcpy(eh->ether_dhost, mac, 6);
    
    // 送信元MACアドレスを設定
    memcpy(eh->ether_shost, Param.vmac, 6);
    
    // L3層のプロトコルタイプを設定
    eh->ether_type = eh_def->ether_type;

    // ether_headerの設定が完了したので、
    // アドレスをether_header分ズラス
    ptr += sizeof(struct ether_header);

    // ずらした位置からペイロードをセット
    memcpy(ptr, data, len);

    printf("ether transfer2\n");

    // ペイロード分アドレスをズラス
    ptr += len;

    printf("ether transfer3\n");

    // 現在のアドレスと最初のアドレスの差分をみて、
    // 規定の最小パケットサイズ(60byte)未満でないかを確認
    // 小さいようであれば、60byteに達するまで0で埋める
    if((ptr - sbuf) < ETH_ZLEN)
    {
        padlen = ETH_ZLEN - (ptr - sbuf);
        printf("padlen=%s\n", padlen);
        memset(ptr, 0, padlen);
        ptr += padlen;
    }

    printf("ether transfer4\n");


    // 送信するパケットが完成したので、あとは送る
    write(soc, sbuf, ptr - sbuf);
    printf("ether transfer5\n");

    // print_ether_header(eh);
    // char buff[80];
    // printf("EtherTransfer to %s !!!!\n", my_ether_ntoa_r(eh->ether_dhost, buff));

    return 0;

}

int print_hex(u_int8_t *data,int size)
{
int	i,j;

	for(i=0;i<size; ){
		for(j=0;j<16;j++){
			if(j!=0){
				printf(" ");
			}
			if(i+j<size){
				printf("%02X",*(data+j));
			}
			else{
				printf("  ");
			}
		}
		printf("    ");
		for(j=0;j<16;j++){
			if(i<size){
				if(isascii(*data)&&isprint(*data)){
					printf("%c",*data);
				}
				else{
					printf(".");
				}
				data++;i++;
			}
			else{
				printf(" ");
			}
		}
		printf("\n");
	}

	return(0);
}

/*
受信した生パケットデータを表示する関数
*/
void print_rawdata(u_int8_t raw_data[], int len)
{
    int i;
    int j = 0;
    int raw = 0;
    printf("rawdata----------------------------------------------\n");
    printf("     ");
    for(i = 0; i < 16 ; i++)
    {
        printf(" %x ", i);
    }
    printf("\n=====================================================\n");

    for(i = 0; i < len; i++)
    {
        // 行番号の表示
        if(j == 0)
        {
            printf("%04x  ", raw);
            raw += 16;
        }
        printf("%.2x ", raw_data[i]);
        if(j==15)
        {
            // 改行
            printf("\n");
            j = 0;
        }
        else
        {
            j++;
        }
    }
    printf("\n");
}



