#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef __FAVOR_BSD
# define __FAVOR_BSD
#endif
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <err.h>

int sendInputData(int sd, char *msg);

/* 
エントリーポイント 
argv[0] : 
argv[1] : 送信元IPアドレス
argv[2] : 送信先IPアドレス
argv[3] : 送信先UDPポート番号
argv[4] : データ文字列
*/
int main(int argc, char *argv[])
{
    int sd = 0;
    struct sockaddr_in to = {0};
    char *dst_ip = argv[1];
    int dst_port = atoi(argv[2]);
    char *msg = argv[3];

    // 引数の数が5でなかったら入力エラー
    if(argc != 4) 
    {
        fprintf(stderr, "Usage: %s <dst IP> <dst Port> <Message>\n");
        exit(1);
    }

    printf("sending to IPAdress %s , port %s\n", argv[1], argv[2]);

    if((sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket failed.\n ---THE BAD END---\n");
        return -1;
    }

    printf("socket is made successfully.\n");
    bzero((char *)&to, sizeof(to));
    to.sin_family = PF_INET;
    to.sin_addr.s_addr = inet_addr(dst_ip);
    to.sin_port=htons(dst_port);

    if(connect(sd, (struct sockaddr *)&to, sizeof(to)) > 0)
    {
        printf("connetion failed.");
        close(sd);
        exit(1);
    }

    sendInputData(sd, msg);
    close(sd);
    printf("--- you send tcp packet successfully---\n --- GOOD NIGHT ---\n");
    return 0;
}

int sendInputData(int sd, char *msg)
{
    int ret = 0;
    int msg_len = strlen(msg) + 1;
    
    if((ret = send(sd, msg, msg_len, 0)) != msg_len)
    {
        printf("send Data failed\n");
        return -1;
    }
    return ret;
}