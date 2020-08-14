#include <stdio.h>
#include <stdlib.h> //srandom
#include <string.h>
#include <unistd.h> //sleep()

// Only Berkeley Software Distribution
#include <string.h>// strtok_r()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>// ifreq
#include <sys/ioctl.h>
#include <signal.h>
#include <pthread.h> // pthread関連メソッド
#include <poll.h> // POLLIN, POLLERR

#include "param.h"
#include "ether.h"
#include "ip.h"
#include "sock.h"
#include "icmp.h"
#include "cmd.h"
#include "udp.h"
#include "dhcp.h"

// 終了フラグ　1になったら終了
int EndFlag = 0;

PARAM Param;

// ソケットのファイルディスクリプタ(ファイルハンドル)
int DeviceSoc;

void *MyEthThread(void *arg);
void *StdInThread(void *arg);
void sig_term(int sig);
int ending();
int show_ifreq(char *name);
void signals();
void print_rawdata(u_int8_t raw_data[], int len);


int main(int argc, char *argv[])
{
    char buf1[80];
    int paramFlag;
    
    // スレッド属性オブジェクト
    // threadの属性(どんなスレッドにするか)を設定する構造体pthread.hにて定義
    pthread_attr_t attr;
    pthread_t thread_id; //pthread.hにて定義

    SetDafultParam();

    paramFlag = 0;
    // argcは引数の個数
    int i;
    for(i = 1; i < argc; i++)
    {
        if(ReadParam(argv[1]) == -1)
        {
            exit(-1);
        }
        paramFlag = 1;
    }

    if(paramFlag == 0)
    {
        if(ReadParam("./MyEth.ini") == -1)
        {
            exit(-1);
        }
    }

    printf("IP-TTL = %d\n", Param.IpTTL);
    printf("MTU = %d\n", Param.MTU);

    srandom(time(NULL));

    IpRecvBufInit();

    if((DeviceSoc=init_sock(Param.device)) == -1)
    {
        exit(-1);
    }

    printf("device = %s\n", Param.device);
    printf("------------------------------\n");
    show_ifreq(Param.device);
    printf("------------------------------\n");

    printf("vmac     = %s\n", my_ether_ntoa_r(Param.vmac, buf1));
    printf("vip      = %s\n", inet_ntop(AF_INET, &Param.vip, buf1, sizeof(buf1)));
    printf("vmask    = %s\n", inet_ntop(AF_INET, &Param.vmask, buf1, sizeof(buf1)));
    printf("gateway  = %s\n", inet_ntop(AF_INET, &Param.gateway, buf1, sizeof(buf1)));
    printf("DHCP request lease time=%d\n", Param.DhcpRequestLeaseTime);

    signals();

    // スレッド属性オブジェクトの初期化
    pthread_attr_init(&attr);
    // スレッドのスタックメモリサイズを設定
    pthread_attr_setstacksize(&attr, 102400);
    // スレッドをdetached状態に設定
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    // スレッドの作成
    if(pthread_create(&thread_id, &attr, MyEthThread, NULL) !=0)
    {
        printf("pthread_create:error\n");
    }

    if(pthread_create(&thread_id, &attr, StdInThread, NULL) !=0)
    {
        printf("pthread_create:error\n");
    }

    // DHCPサーバーからIPアドレスを割り当てられていない場合
    if(Param.vip.s_addr == 0)
    {
        int count = 0;
        do
        {
            count++;
            if(count > 5)
            {
                // DHCPからのIPアドレス割り当てが失敗とみなし終了
                printf("DHCP fail\n");
                return -1;
            }
            DhcpSendDiscover(DeviceSoc);
            sleep(1);
        }
        while(Param.vip.s_addr == 0);
    }


    if(ArpCheckGArp(DeviceSoc) == 0)
    {
        printf("GArp check fail\n");
        return -1;
    }

    while(EndFlag == 0)
    {
        sleep(1);
        if(Param.DhcpStartTime != 0)
        {
            DhcpCheck(DeviceSoc);
        }
    }

    ending();

    return 0;
}

// ネットワークデバイスの情報を出力
int show_ifreq(char *name)
{
    char buf[80];
    int soc;
    struct ifreq ifreq;
    struct sockaddr_in addr;

    if((soc = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        return -1;
    }
    printf("soc index in show_ifreq : %d\n", soc);

    strcpy(ifreq.ifr_name, name);

    if(ioctl(soc, SIOCGIFFLAGS, &ifreq) == -1)
    {
        perror("ioctl::flags");
        close(soc);
        return -1;
    }

    if(ifreq.ifr_flags & IFF_UP){printf("UP ");}
    if(ifreq.ifr_flags & IFF_BROADCAST){printf("BROADCAST ");}
    if(ifreq.ifr_flags & IFF_PROMISC){printf("PROMISC ");}
    if(ifreq.ifr_flags & IFF_MULTICAST){printf("MULTICAST ");}
    if(ifreq.ifr_flags & IFF_LOOPBACK){printf("LOOPBACK ");}
    if(ifreq.ifr_flags & IFF_POINTOPOINT){printf("P2P ");}
    printf("\n");

    if(ioctl(soc, SIOCGIFMTU, &ifreq) == -1)
    {
        perror("ioctl : mtu");
    }
    else
    {
        printf("mtu = %d\n", ifreq.ifr_mtu);
    }

    if(ioctl(soc, SIOCGIFADDR, &ifreq) == -1)
    {
        perror("ioctl : addr");
    }
    else if(ifreq.ifr_addr.sa_family != AF_INET)
    {
        printf("not AF_INET");
    }
    else
    {
        memcpy(&addr, &ifreq.ifr_addr, sizeof(struct sockaddr_in));
        printf("myip = %s\n", inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf)));
        Param.myip = addr.sin_addr;
    }

    close(soc);

    if(GetMacAddress(name, Param.mymac) == -1)
    {
        printf("GetMacAddress:error");
    }
    else
    {
        printf("mymac = %s\n", my_ether_ntoa_r(Param.mymac, buf));
    }

    return 0;
}

// 終了フラグを立てる
void sig_term(int sig)
{
    char *type;
    switch(sig)
    {
        case SIGINT:
            type = "SIGINT";
            break;
        case SIGTERM:
            type = "SIGTERM";
            break;
        case SIGQUIT:
            type = "SIGQUIT";
            break;
        default:
            type = "Some type";
            break;
    }
    printf("\nsent %s signal , bye\n", type);
    EndFlag = 1;
}

void signals()
{
    printf("set signals\n");
    // CTRL+Cが入力されたか、キーボードからの割り込みシグナルがあった場合
    // 難しくいうと「対話的なアテンションシグナルを受け取ったか」らしい
    signal(SIGINT, sig_term);
    // 強制シグナルがあった場合、killコマンド
    signal(SIGTERM, sig_term);
    // CTRL+\が入力されたか、キーボーロによる中止シグナルがあったか 
    signal(SIGQUIT, sig_term);
    // SIGPIPE : 切断されたネットワークソケットなどにデータを書き込もうとした場合のシグナル
    // ソケットプログラミングでは結構発生するらしいので、下記を実行してSIGPIPEシグナルを無視
    signal(SIGPIPE, SIG_IGN);
}

void *MyEthThread(void *arg)
{
    int nready;
    struct pollfd targets[1];
    u_int8_t buf[2048];
    int len;

    // 監視対象のファイルディスクリプタを指定
    targets[0].fd = DeviceSoc;
    // 検索するイベントを設定 
    // TODO : POLLERRは指定しなくても検索するらしいので実験せよ
    targets[0].events = POLLIN | POLLERR; 

    while(EndFlag == 0)
    {
        // arg1 : 監視対象のファイルディスクリプタ
        // arg2 : 監視対象の個数 arg1みてよ
        // arg3 : 待機時間(ミリsec)
        switch((nready = poll(targets, 1, 1000)))
        {
            case -1: // エラー発生時
                printf("poll error\n");
                break;
            case  0: // タイムアウト時 
                // printf("time out\n");
                break;
            default: // 準備完了 たぶんDeviceSocが帰ってくる
                // printf("MyEthThread:nready : %s\n", nready);
                if(targets[0].revents & (POLLIN | POLLERR))
                {
                    if((len = read(DeviceSoc, buf, sizeof(buf))) <= 0)
                    {
                        perror("read");
                    }
                    else
                    {
                        // print_rawdata(buf, len);
                        EtherRecv(DeviceSoc, buf, len);
                    }
                }
                break;
        }
    }
    return NULL;
}

void *StdInThread(void *arg)
{
    int nready;
    struct pollfd targets[1];
    u_int8_t buf[2048];
    int len;

    // 監視対象のファイルディスクリプタを指定
    targets[0].fd = fileno(stdin);
    // 検索するイベントを設定 
    // TODO : POLLERRは指定しなくても検索するらしいので実験せよ
    targets[0].events = POLLIN | POLLERR; 

    while(EndFlag == 0)
    {
        // arg1 : 監視対象のファイルディスクリプタ
        // arg2 : 監視対象の個数 arg1みてよ
        // arg3 : 待機時間(ミリsec)
        switch((nready = poll(targets, 1, 1000)))
        {
            case -1: // エラー発生時
                printf("poll error\n");
                break;
            case  0: // タイムアウト時 
                break;
            default: // 準備完了 たぶんDeviceSocが帰ってくる
                if(targets[0].revents & (POLLIN | POLLERR))
                {
                    fgets(buf, sizeof(buf), stdin);
                    DoCmd(buf);
                }
                break;
        }
    }
    return NULL;
}


int ending()
{
    struct ifreq if_req;

    printf("ending\n");

    if(Param.DhcpServer.s_addr != 0)
    {
		DhcpSendRelease(DeviceSoc);
	}

    if(DeviceSoc != -1)
    {
        strcpy(if_req.ifr_name, Param.device);
        if(ioctl(DeviceSoc, SIOCGIFFLAGS, &if_req) < 0)
        {
            perror("ioctl");
        }

        if_req.ifr_flags = if_req.ifr_flags & ~IFF_PROMISC;
        if(ioctl(DeviceSoc, SIOCSIFFLAGS, &if_req) < 0)
        {
            perror("ioctl");
        }

        close(DeviceSoc);
        DeviceSoc = -1;
    }
    return 0;
}