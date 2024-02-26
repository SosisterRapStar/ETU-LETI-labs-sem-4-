#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>        
#include <string.h> 
#include <sys/stat.h>  
#include <sys/mman.h>     
#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <sys/epoll.h>
#include "pthread.h"
#include <netdb.h>

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)


int ret_val;
int flag_for_writer;
sem_t *sem_writer;
int descriptor;
int global_net_error;
char buff[100];
void *addr;

off_t offset = 0; 

// объявить локальный адрес;


void semPostError(int* ret_val){
    if (*ret_val == -1){
        perror("\nОшибка освобождения семафора\n");
    }
}

void getnetError(int* ret_val){
    
     switch(*ret_val){
        case 0:
            printf("Получение данных о сети, используя функцию getnet_r\n");
            break;
        // case ENOENT:
        //     perror("В хранилище больше нет записей\n");
        //     break;
        case ERANGE:
            perror("Буффер слишком мал, попробуйте заново, увеличив размер буфера\n");
            break;
        default:
            perror("Возникла ошибка\n");
            break;
    }
    
}

void openingSharedError(int *descriptor){
    if (*descriptor == -1) {
        perror("\nОшибка открытия файла!\n");
        
    }
}

void semaphoreOpeningError(sem_t *sem){
    if (sem == SEM_FAILED){
        perror("\nОшибка открытия или создания семафора\n");
    }

}

void threadCreatorErrorHandler(int* ret_val){
    if(*ret_val != 0) {
                perror("\nОшибка создания потока\n"); 
        }
}

void closingError(int *ret_val){
    if (*ret_val == -1){
        perror("\nОшибка закрытия\n");
    }
}

void ftruncError(int *ret_val){
    if (*ret_val == -1){
        perror("\nОшибка установки размера участка выделяемой памяти\n");
    }
}


static void * threadWriter (void *flag_writer){
    int *flag = (int*) flag_writer;
    printf("Поток читателя начал работу\n");


   
    struct netent result_buf;
    struct netent *result;
    int r = getnetent_r(&result_buf, buff, sizeof(buff), &result, &global_net_error);
    getnetError(&r);
    

    while (*flag == 0) {
        
        
        printf("%s\n", buff);
        
        fflush(stdout);
        memcpy(addr, buff, sizeof(buff));
        ret_val = sem_post(sem_writer);
        
        sleep(1);
    }

    pthread_exit((void*)1);

}


int main(){
    
    
    pthread_t threadWriter_ID;
    printf("Процесс писатель начал работу\n");

    descriptor = shm_open("/memname", O_CREAT | O_RDWR, 0666);
    openingSharedError(&descriptor);

    ret_val = ftruncate(descriptor, sizeof(buff));
    ftruncError(&ret_val);

    addr = mmap(NULL, sizeof(buff), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, offset);
    if (addr == MAP_FAILED)
               handle_error("mmap");



    sem_writer = sem_open("/mysem", O_CREAT, 0666, 0);
    semaphoreOpeningError(sem_writer);

    ret_val = pthread_create(&threadWriter_ID, NULL, threadWriter, &flag_for_writer);
    threadCreatorErrorHandler(&ret_val);

    getchar();

    flag_for_writer = 1;

    int *exitcode;

    pthread_join(threadWriter_ID, (void**)&exitcode);

    
    sem_close(sem_writer);
    sem_unlink("/mysem");

    ret_val = close(descriptor);
    closingError(&ret_val);
    
    munmap(addr, sizeof(buff));

}