#include <netinet/in.h>
#include <sys/socket.h>
// #include <netinet/ih.h>
#include <arpa/inet.h>

#include <stdio.h>

int main(int argc, char *argv[])
{
    struct in_addr addr;
    // char *addr_char = "192.168.3.50"; 
    char *addr_char = argv[1]; 

    printf("addr_char=%s\n", addr_char);
    // 文字形式のIPアドレス表記をバイナリ表記に変換
    addr.s_addr = inet_addr(addr_char);
    printf("addr(d)=%d\n", addr.s_addr);

    char buf[80];
    // inet_ntop()　バイナリ表記のIPアドレスを文字形式に変換
    printf("addr(s)=%s\n", inet_ntop(AF_INET, &addr, buf, sizeof(buf)));

    return 0;
}
