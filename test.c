#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

static char *nextFreeBlock(char *bp)
{
    return ((char **) bp)[1];
}

int main(int argc, char *argv[]) 
{
   char s[100] = "01234567890123456789";
   printf("%p\n", s);
   printf("%p\n", ((char **) s)[1]);
}