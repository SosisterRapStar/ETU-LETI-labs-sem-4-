#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>




static void * threadFunc_1 (void *flag1){
    
    printf("Поток 1 начал свою работу\n");
    sleep(1);
    int *flag = (int*) flag1;
    
    
    while (*flag == 0) {
        int k = 0;
        while (k < 10){

        // использование цикла while так как:
        // for ведет себя в потоках странно
        // при перехвате мьютекса он все равно дает другому потоку выполняться
        // при отсутствии мьютексов выполняется только 1 поток
        // возможно причины в использовании WSL


            k++;
            putchar('1');
            fflush(stdout);
            sleep(1);
        }
        sleep(1);
    }

    pthread_exit((void*)1);

}

static void * threadFunc_2 (void *flag2){
    printf("Поток 2 начал свою работу\n");
    sleep(1);
    int *flag = (int*) flag2;

    while (*flag == 0) {

        int k = 0;

        if (k < 10){
            k++;

            putchar('2');
            fflush(stdout);
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



int main(){ 
    printf("Программа начала свою работу\n");

    int flag1 = 0;
    int flag2 = 0;
    
    int ret_val;


    pthread_t threadId_1;
    pthread_t threadId_2;

    ret_val = pthread_create(&threadId_1, NULL, threadFunc_1, &flag1);
    threadCreatorErrorHandler(&ret_val);
    ret_val = pthread_create(&threadId_2, NULL, threadFunc_2, &flag2);
    threadCreatorErrorHandler(&ret_val);
    sleep(1.75);
    printf("Программа ждет нажатия клавиши\n");
    getchar();
    

    printf("Клавиша нажата\n");

    flag1 = 1;
    flag2 = 1;

    int *exitcode1;
    int *exitcode2;

    printf("Завершение работы потоков, перед завершением программы\n\n");
    
    ret_val = pthread_join(threadId_1, (void**)&exitcode1);
    ret_val = pthread_join(threadId_2, (void**)&exitcode2);

    printf("exitcode1 = %p\nexitcode2 = %p\n", exitcode1, exitcode2);

    printf("Программа завершила свою работу\n");


}  