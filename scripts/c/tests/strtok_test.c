#include <stdio.h>
#include <string.h>

u_int8_t mac[6];

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

int main(int argc, char *argv[])
{
    printf("hello world.\n");
    char *str = "e8:e0:b7:9c:3e:54";

    my_ether_aton(str, mac);

    for(int i=0;i<sizeof(mac);i++)
    {
        printf("no.%d:%d ", i, mac[i]);
    }
    print("\n");

}