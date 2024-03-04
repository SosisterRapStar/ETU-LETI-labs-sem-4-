#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>       
#include <string.h> 
#include <sys/stat.h>  
#include <sys/ipc.h>
#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <sys/epoll.h>
#include "pthread.h"
#include <sys/msg.h>
#include <netdb.h>


int ret_val;
int flag_for_writer;
int msgid;

typedef struct {
    long mtype;
    char buff[256];
}TMessage;







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




static void * threadWriter (void *flag_writer){
    TMessage message;
    message.mtype = 1;

    int *flag = (int*) flag_writer;
    printf("Поток писателя начал работу\n");
    struct netent result_buf;
    struct netent *result;
    int ret;
    
    int r = getnetent_r(&result_buf, message.buff, sizeof(message.buff), &result, NULL);
    while(*flag == 0){
        
        
        while (msgsnd(msgid, &message, sizeof(message.buff), IPC_NOWAIT) == -1){
            // sleep(0.1); это была ошибка
            
            usleep(100000);
            
        }
        printf("Записано %s\n", message.buff);
        fflush(stdout);
        sleep(1);

    }
    pthread_exit((void*)1);
}



int main(){
    
    pthread_t threadWriter_ID;
    printf("Процесс писатель начал работу\n");
    key_t key = ftok("lab7_writer.o",'A');

    msgid = msgget(key,0);//открываем
    if (msgid < 0) {//ошибка открытия
        msgid = msgget(key, IPC_CREAT | 0666);//создаем
        printf("Открыт\n");
        perror("get");
    }
    else{
        printf("Открыт cразу\n");
    }

    ret_val = pthread_create(&threadWriter_ID, NULL, threadWriter, &flag_for_writer);
    threadCreatorErrorHandler(&ret_val);

    getchar();

    flag_for_writer = 1;
    
    int *exitcode;
    pthread_join(threadWriter_ID, (void**)&exitcode);
    msgctl(msgid, IPC_RMID, NULL); 

    
    
}