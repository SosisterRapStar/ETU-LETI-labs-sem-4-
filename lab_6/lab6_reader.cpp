#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>       
#include <string.h> 
#include <sys/stat.h>  
#include <sys/mman.h>     
#include <semaphore.h>
#include <sys/epoll.h>
#include "pthread.h"
#include <netdb.h>

int ret_val;
int flag_reader;
sem_t *sem_reader;
int descriptor;
int global_net_error;
char buff[50];
void *addr;

off_t offset = 0; 

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

// объявить локальный адрес;




void semPostError(int* ret_val){
    if (*ret_val == -1){
        perror("\nОшибка освобождения семафора\n");
    }
}

void getnetError(int* ret_val){
    if (*ret_val == -1){
        perror("\nОшибка получения данных о сети\n");
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


static void * threadReader (void *flag_reader){
    int *flag = (int*) flag_reader;
    printf("\nПоток начал свою работу\n");

    while (*flag == 0) {
        memset(buff, 0, sizeof(buff));
        sem_wait(sem_reader);
        memcpy(buff, addr, sizeof(buff));
       

        sleep(1);   

        printf("%s\n", buff);
        fflush(stdout);

     
    }

    pthread_exit((void*)2);

}


int main(){
    
    
    pthread_t threadReader_ID;

    descriptor = shm_open("/memname", O_CREAT | O_RDWR, 0666);
    openingSharedError(&descriptor);

    ret_val = ftruncate(descriptor, sizeof(buff));
    ftruncError(&ret_val);

    addr = mmap(NULL, sizeof(buff), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, offset);
     if (addr == MAP_FAILED)
               handle_error("mmap");
    

    sem_reader = sem_open("/mysem", O_CREAT, 0666, 0);
    semaphoreOpeningError(sem_reader);

   
    ret_val = pthread_create(&threadReader_ID, NULL, threadReader, &flag_reader);
    threadCreatorErrorHandler(&ret_val);

    printf("Нажмите любую кнопку");
    getchar();

    flag_reader = 1;

    int *exitcode;
    pthread_join(threadReader_ID, (void**)&exitcode);

    sem_close(sem_reader);
    sem_unlink("/mysem");

    ret_val = close(descriptor);
    closingError(&ret_val);
    
    munmap(addr, sizeof(buff));

}