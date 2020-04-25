#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void get_ifinfo(char *devname, struct ifreq *ifreq, int flavor)
{
    int iofd;

    if((iofd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("ioctl socket creation");
        exit(1);
    }

    memset(ifreq, '\0', sizeof(*ifreq));
    strcpy(ifreq->ifr_name, devname);

    if(ioctl(iofd, flavor, ifreq) < 0)
    {
        perror("ioctl");
        exit(1);
    }

    return;
}

int main(int argc, char *argv)
{
    struct ifreq ifreq;
    char *devname = "enp0s3";
    struct sockaddr_in saddr;
    // printf("*device : %s\n", *devname);
    // printf("device : %s\n", devname);
    // printf("device name : %s", *argv);


    get_ifinfo(devname, &ifreq, SIOCGIFINDEX);
    printf("interface index : %d\n", ifreq.ifr_ifindex);

    get_ifinfo(devname, &ifreq, SIOCGIFADDR);
    memcpy(&saddr, &(ifreq.ifr_addr), sizeof(saddr));
    printf("ipAddress :  %s\n", inet_ntoa(saddr.sin_addr));

    
}



