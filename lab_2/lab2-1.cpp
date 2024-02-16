#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>


int flag1 = 0;
int flag2 = 0;

static void * threadFunc_1 (void *flag1){
    
    int *flag = (int*) flag1;
    printf("Поток 1 начал свою работу\n");

    while (*flag == 0) {
        for(int i; i < 10; i++){
            putchar('1');
            sleep(1);
        }
        sleep(1);
    }

    pthread_exit((void*)1);

}

static void * threadFunc_2 (void *flag2){
    


    sleep(2); 
    printf("Поток 2 начал свою работу\n");

    int *flag = (int*) flag2;

    
    while (*flag == 0) {
        for(int i; i < 10; i++){
            putchar('2');
            sleep(1);
        }
        sleep(1);
    }

    pthread_exit((void*)2);

}

void threadCreatorErrorHandler(int* ret_val){
    if(*ret_val != 0) {
                perror("Ошибка создания потока"); 
        }
}

// void setScheduleParamsError(int* ret_val){
//     if(*ret_val != 0) {
//                 perror("Ошибка изменения параметров потока: "); 
//         }
// }

// void getScheduleParamsError(int* ret_val){
//     if(*ret_val != 0) {
//                 perror("Ошибка получения параметров потока: "); 
//         }
// }




int main(){ 
    printf("Программа начала свою работу\n");
    
    int ret_val;



    pthread_t threadId_1;
    pthread_t threadId_2;

    ret_val = pthread_create(&threadId_1, NULL, threadFunc_1, &flag1);
    threadCreatorErrorHandler(&ret_val);


    ret_val = pthread_create(&threadId_2, NULL, threadFunc_2, &flag2);
    threadCreatorErrorHandler(&ret_val);

    getchar();

    flag1 = 1;
    flag2 = 1;

    int *exitcode1;
    int *exitcode2;

    ret_val = pthread_join(threadId_1, (void**)&exitcode1);
    ret_val = pthread_join(threadId_2, (void**)&exitcode2);

    printf("exitcode1 = %p\nexitcode2 = %p\n", exitcode1, exitcode2);

    printf("Программа завершила свою работу\n");


}  
