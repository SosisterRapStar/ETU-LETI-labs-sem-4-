#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    printf("lab4_1.c called by execvp(): %s", argv[1]);
    printf("\n");
    sleep(1);
    exit(32);
}