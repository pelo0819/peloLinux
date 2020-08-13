#include <stdio.h>

void test_int(int *p);
void test_char(char *p);

struct PPP
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
};


int main()
{
    // unsigned int *i;
    // int ii = 5;

    // char s[] = "hello world";
    // char *ss;

    // i= &ii;

    // ss =s;
    // printf("i=%d\n", *i);
    // printf("%s\n", &s);

    // test_int(&ii);
    // test_char(ss);


    unsigned int p[3]={1,2,3};
    int si = sizeof(p)/sizeof(unsigned int);
    printf("size=%d\n", si);
    printf("p=%p\n", p);

    for(int i=0;i<si;i++)
    {
        printf("$p[%d]:%p\n", i, &p[i]);
        printf(" p[%d]:%u\n", i, p[i]);
    }

    struct PPP *ppp;
    ppp =(struct PPP *)p;
    
    printf("ppp->a:%u\n", ppp->a);
    printf("ppp->b:%u\n", ppp->b);
    printf("ppp->c:%u\n", ppp->c);

}


void test_int(int *p)
{
    printf("*p=%d\n", *p);
    printf("p=%p\n", p);
    printf("&p=%p\n", &p);
}

void test_char(char *p)
{
    printf("%s\n", p);
}