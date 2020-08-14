#include <stdio.h>

void test_int(int *p);
void test_char(char *p);
void test_unsigned_char(unsigned char *p, int len);
void test_cast(void);

// struct PPP
// {
//     unsigned int a;
//     unsigned int b;
//     unsigned int c;
// };


int main()
{

    unsigned char c[4] ={4,5,2,0};
    test_unsigned_char(c, 4);

    test_cast();
}

void test_cast(void)
{
    printf("--- test_cast() ---\n");

    struct PPP
    {
        unsigned int a;
        unsigned int b;
        unsigned int c;
    };

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
    printf("--- test_init() ---\n");
    printf("*p=%d\n", *p);
    printf("p=%p\n", p);
    printf("&p=%p\n", &p);
}

void test_char(char *p)
{
    printf("--- test_char() ---\n");

    printf("%s\n", p);
}

void test_unsigned_char(unsigned char *p, int len)
{
    printf("--- test_unsigned_char() ---\n");

    unsigned char *ptr, *ptr2;
    ptr = p;
    printf("ptr:%p\n", ptr);
    ptr2 = p;
    printf("ptr2:%p\n", ptr2);

    for(int i=0;i<len;i++)
    {
        printf("no.%d:%u\n", i, *ptr);
        ptr++;
    }
    
    printf("ptr2:%p\n", ptr2);
    for(int i=0;i<len;i++)
    {
        unsigned char pp =*ptr2;
        pp <<= 2;
        printf("no.%d:%u\n", i, pp);
        ptr2++;
    }
}