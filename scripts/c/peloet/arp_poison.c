#include <stdio.h>
#include <pthread.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "sock.h"
#include "ether.h"
#include "arp.h"
#include "param.h"
#include "arp_poison.h"

extern PARAM Param;

int isPoisoning = 0;

struct in_addr target1;
struct in_addr target2;

void Poison(void)
{
    if(isPoisoning != 1){ return; }

    printf("poisoning now.\n");

}

void StartPoison(int soc, struct in_addr *tar1, struct in_addr *tar2)
{
    isPoisoning = 1;
    target1 = *tar1;
    target2 = *tar2;
}

void StopPoison(void)
{
    isPoisoning = 0;
}
