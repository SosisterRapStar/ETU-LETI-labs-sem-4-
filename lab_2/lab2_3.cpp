#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <signal.h>
#include <string.h>

int flag1 = 0;
int flag2 = 0;
pthread_mutex_t mutexId;
int ret_val;

void mutexPolling(int* flag, int thread_num);
void mutexUnlockError(int* ret_val);
void mutexLockError(int* ret_val);
void mutexInitError(int* ret_val);

static void * threadFunc_1 (void *flag1){
    int *flag = (int*) flag1;
    printf("Поток 1 начал свою работу\n");
    
    while (*flag == 0) {
        
        mutexPolling(flag, 1);

        printf("\nПроцесс 1 захватил мьютекс\n");

        int k = 0;
        while (k < 10){ 
            k++;
            putchar('1');
            fflush(stdout);
            sleep(1);
        }

        ret_val = pthread_mutex_unlock(&mutexId);
        mutexUnlockError(&ret_val);

        printf("\nПроцесс 1 освободил мьютекс\n");

        sleep(1);        
    }
    pthread_exit((void*)1);
}

static void * threadFunc_2 (void *flag2){
    int *flag = (int*) flag2;
    printf("Поток 2 начал свою работу\n");
    
    while (*flag == 0) {
     
        mutexPolling(flag, 2);

        printf("\nПроцесс 2 захватил мьютекс\n");

        int k = 0;
        while (k < 10){
            k++;
            putchar('2');
            fflush(stdout);
            sleep(1);
        }

        ret_val = pthread_mutex_unlock(&mutexId);
        mutexUnlockError(&ret_val);

        printf("\nПроцесс 2 освободил мьютекс\n");

        sleep(1);
    }
    pthread_exit((void*)2);
}

void mutexPolling(int* flag, int thread_num){
    while ( *flag == 0 ) {
	int rv = pthread_mutex_trylock(&mutexId);
	if (rv == 0) {
		return;
	}else{
		printf("\nОшибка потока %d %s\n", thread_num, strerror(rv));
		sleep(1);
    }
    }
}


void threadCreatorErrorHandler(int* ret_val){
    if(*ret_val != 0) {
                perror("\nОшибка создания потока\n"); 
        }
}

void mutexInitError(int* ret_val){
    if(*ret_val != 0) {
                perror("\nОшибка инициализации мьютекса\n"); 
        }
}

void mutexLockError(int* ret_val){
    if(*ret_val != 0) {
                perror("\nОшибка блокирования мьютекса\n"); 
        }
}

void mutexUnlockError(int* ret_val){
    if(*ret_val != 0) {
                perror("Ошибка разблокирования мьютекса"); 
        }
}


void sig_handler(int signo)
{
    printf("\nСигнал завершения %d\n", signo);
    printf("Удаление мьютекса\n");
    pthread_mutex_destroy(&mutexId);
    exit(0);
}



int main(){ 
    signal(SIGINT, sig_handler);
    printf("Программа начала свою работу\n");
    

    pthread_t threadId_1;
    pthread_t threadId_2;

    ret_val = pthread_mutex_init(&mutexId, NULL);
    mutexInitError(&ret_val);

    // pthread_mutex_lock(&mutexId);
    ret_val = pthread_create(&threadId_1, NULL, threadFunc_1, &flag1);
    threadCreatorErrorHandler(&ret_val);

    ret_val = pthread_create(&threadId_2, NULL, threadFunc_2, &flag2);
    threadCreatorErrorHandler(&ret_val);
    
    printf("\nПрограмма ждет нажатия клавиши\n");
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
    
    printf("Удаление мьютекса\n");
    pthread_mutex_destroy(&mutexId);

    printf("Программа завершила свою работу\n");
    
}  