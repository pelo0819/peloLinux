#include <stdio.h>
#include <stdlib.h>// exit()
// Only BSD
#include <string.h>// strtok_r()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>

#include "param.h"
#include "ether.h"

extern PARAM Param;
// 読込ファイル名
static char *ParamFname = NULL;



int SetDafultParam()
{
    Param.MTU = DEFAULT_MTU;
    Param.IpTTL = DEFAULT_IP_TTL;

    return 0;
}

int ReadParam(char *fname)
{
    FILE *fp;
    char buf[1024];
    char *ptr;
    char *saveptr;

    ParamFname = fname;

    // ファイルの読込
    // arg[0]:ファイル名, arg[1]:読取専用
    if((fp = fopen(fname, "r")) == NULL)
    {
        printf("%s cannot read\n", fname);
        return -1;
    }

    // 読込ファイルの解析
    while(1)
    {
        // bufに1行ずつ格納
        fgets(buf, sizeof(buf), fp);
        
        // 最終行まで行ったらwhile抜ける
        if(feof(fp)) break;
        
        // 行をトークン
        ptr = strtok_r(buf, "=", &saveptr);
        if(ptr != NULL)
        {
            if(strcmp(ptr, "IP-TTL") == 0)
            {
                // 次のトークンへ進む、かつ、改行だったらパラメータを設定
                if((ptr = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
                {
                    Param.IpTTL = atoi(ptr);
                }
            }
            else if(strcmp(ptr, "MTU") ==0)
            {
                if((ptr = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
                {
                    Param.MTU = atoi(ptr);
                    if(Param.MTU > ETHERMTU)
                    {
                        printf("ReadParam : MTU(%d) <= ETHERMTU(%d)\n", Param.MTU, ETHERMTU);
                        Param.MTU = ETHERMTU;
                    }
                }
            }
            else if(strcmp(ptr,"MSS")==0)
            {
				if((ptr = strtok_r(NULL,"\r\n",&saveptr)) != NULL)
                {
					Param.MSS=atoi(ptr);
					if(Param.MSS>ETHERMTU)
                    {
						printf("ReadParam:MSS(%d) <= ETHERMTU(%d)\n", Param.MSS, ETHERMTU);
						Param.MSS = ETHERMTU;
					}
				}
			}
            else if(strcmp(ptr, "gateway") ==0)
            {
                if((ptr = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
                {
                    // Ipアドレスを示した文字列をバイナリに変換
                    Param.gateway.s_addr = inet_addr(ptr);
                }
            }
            else if(strcmp(ptr, "device") ==0)
            {
                if((ptr = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
                {
                    // 複製した文字列ポインタ、ptrは更新されちゃうから？
                    Param.device = strdup(ptr);
                }
            }
            else if(strcmp(ptr, "vmac") ==0)
            {
                if((ptr = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
                {
                    my_ether_aton(ptr, Param.vmac);   
                }
            }
            else if(strcmp(ptr, "vip") ==0)
            {
                if((ptr = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
                {
                    // Ipアドレスを示した文字列をバイナリに変換
                    Param.vip.s_addr = inet_addr(ptr);
                }
            }
            else if(strcmp(ptr, "vmask") ==0)
            {
                if((ptr = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
                {
                    // Ipアドレスを示した文字列をバイナリに変換
                    Param.vmask.s_addr = inet_addr(ptr);
                }
            }
            else if(strcmp(ptr, "DhcpRequestLeaseTime") == 0)
            {
                if((ptr = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
                {
                    Param.DhcpRequestLeaseTime = atoi(ptr);
                }
            }
        }
    }

    fclose(fp);
    return 0;
}

/*
ターゲット(引数)IPアドレスが自分か確認する関数
arg
 ターゲットIPアドレス
return
 自分なら1、違うなら0
*/
int isTargetIPAddr(struct in_addr *addr)
{
    if(Param.vip.s_addr == addr->s_addr)
    {
        return 1;
    }
    return 0;
}


// ビット演算子で上位3バイトが同一であるかをチェック
// 同一なら１を返す
// struct in_addr{
// in_addr_t s_addr;   
//}
// typedef u_int32_t in_addr_t
int isSameSubnet(struct in_addr *addr)
{
    if((addr->s_addr & Param.vmask.s_addr) == (Param.vip.s_addr & Param.vmask.s_addr))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}