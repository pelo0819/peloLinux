#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    char *fname;
    char buf[1024];
    char *ptr;
    char *savaptr;
    fname = argv[1];

    if((fp = fopen(fname, "r")) == NULL)
    {
        printf("%s cannot open.\n", fname);
        return -1;
    }
    while(1){
        fgets(buf, sizeof(buf), fp);

        printf(buf);    
        if(feof(fp))
        {
            break;
        }

        ptr = strtok_r(buf, "=", &savaptr);
        if(ptr!=NULL)
        {
            if(strcmp(ptr,"IP-TTL") == 0)
            {
                if((ptr = strtok_r(NULL, "\r\n", &savaptr)) != NULL)
                {
                    printf(ptr);
                }
            }
        }
    }

    // while(1){
    //     int getf = fgetc(fp);
    //     if(getf == EOF) break;
    //     printf("%c\n" , getf);
    // }
    
    
    fclose(fp);

    // char test[80], blah[80];
    // char *sep ="\\/:;=-";
    // char *word, *phrase, *brkt, *brkb;

    // strcpy(test, "This;is.a:test:of");
    // word = strtok_r(test, sep, &brkt)
    // printf("%s\n", word);
    // word = strtok_r(test, sep, &brkt)
    // printf("%s\n", word);
    // word = strtok_r(test, sep, &brkt)
    // printf("%s\n", word);
    // word = strtok_r(test, sep, &brkt)
    // printf("%s\n", word);

    // for(word = strtok_r(test, sep, &brkt); word; word = strtok_r(NULL, sep, &brkt))
    // {
    //     strcpy(blah, "blad:blat:balb:blag");
    //     for(phrase = strtok_r(blah, sep, &brkb); phrase; phrase = strtok_r(NULL, sep, &brkb))
    //     {
    //         printf("So far we are at %s:%s\n", word, phrase);
    //     }
    // }



    return 0;
}