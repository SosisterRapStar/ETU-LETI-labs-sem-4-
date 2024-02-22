#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[], char *envp[])
{
    int exit_status = 5;

    int pid = getpid();
    int ppid = getppid();

    printf("\nPID дочерней программы %d\n", pid);
    printf("\nPID родителя дочерней программы %d\n", ppid);

    int i = 0;
    printf("\nДочерняя программа выводит переменные окружения\n");
    while(envp[i] != NULL){
        sleep(1);
        printf("%s\n", envp[i]);
        i++;
    }

    i = 0;
    printf("\nДочерняя программа выводит аргументы командной строки\n");
    while(argv[i] != NULL){
        sleep(1);
        printf("%s\n", argv[i]);
        i++;
    }
    

    printf("Дочерний процесс завершает работу, код завершения: %d", exit_status);
    exit(exit_status);
}