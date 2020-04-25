#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *param);
void *thread_func2(void *param);

int cnt=0;

int main(int arg, char *args[])
{
    pthread_t thread1;
    pthread_t thread2;
    int ret1 = 0;
    int ret2 = 0;

    ret1 = pthread_create(&thread1, NULL, thread_func, (void *)NULL);
    if(ret1 != 0)
    {
        printf("failed create thread1\n");
        exit(1);
    }

    ret2 = pthread_create(&thread2, NULL, thread_func2, (void *)NULL);
    if(ret2 != 0)
    {
        printf("failed create thread2\n");
        exit(1);
    }

    ret1 = pthread_join(thread1, NULL);
    if(ret1 != 0)
    {
        printf("failed pthread_join 1\n");
        exit(1);
    }

    ret2 = pthread_join(thread2, NULL);
    if(ret2 != 0)
    {
        printf("failed pthread_join 2\n");        
        exit(1);
    }

    return 0;
}

void *thread_func(void *param)
{
    int i=0;
    for(i = 0; i < 10; i++)
    {
        char s[256];
        sprintf(s, "%d" ,cnt);
        fprintf(stdout, "i : %s\n", s);
        sleep(2);
    }
    return NULL;
}

void *thread_func2(void *param)
{
    int i = 0;
    for(i = 0; i < 10; i++)
    {
        printf("hello\n");
        sleep(1);
        cnt++;
    }
    return NULL;
}