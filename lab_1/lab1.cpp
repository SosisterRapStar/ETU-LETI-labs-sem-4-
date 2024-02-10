#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>


static void * threadFunc_1 (void *flag){
    printf("Поток 1 начал свою работу\n");

    sleep(1);

    while (flag == 0) {
        putchar('1');
        fflush(stdout);
        sleep(1);
        
    }

    pthread_exit((void*)1);

}

static void * threadFunc_2 (void *flag){
    printf("Поток 2 начал свою работу\n");

    sleep(1);

    while (flag == 0) {
        putchar('2');
        fflush(stdout);
        sleep(1);
        
    }

    pthread_exit((void*)2);

}

void mainThreadErrorHandler(int* ret_val){
    if(*ret_val != 0) {
                perror("Ошибка создания потока"); 
        }
}


int main(){
    printf("Программа начала свою работу\n");

    int flag1 = 0;
    int flag2 = 0;

    int ret_val;

    pthread_t threadId_1;
    pthread_t threadId_2;

    ret_val = pthread_create(&threadId_1, NULL, threadFunc_1, (void *) flag1);
    mainThreadErrorHandler(&ret_val);
    ret_val = pthread_create(&threadId_2, NULL, threadFunc_2, (void *) flag2);
    mainThreadErrorHandler(&ret_val);

    printf("Программа ждет нажатия клавиши\n");

    getchar();

    printf("Клавиша нажата\n");

    flag1 = 1;
    flag2 = 1;

    int *exitcode1;
    int *exitcode2;

    ret_val = pthread_join(threadId_1, (void**)&exitcode1);
    ret_val = pthread_join(threadId_2, (void**)&exitcode2);

    printf("exitcode1 = %p\n, exitcode2 = %p\n", exitcode1, exitcode2);

    printf("Программа завершила свою работу");

}  
