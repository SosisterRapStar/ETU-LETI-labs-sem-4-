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
int flag_reader;
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




static void * threadReader (void *flag_reader){
    TMessage message;
    message.mtype = 1;

    int *flag = (int*) flag_reader;
    printf("Поток читателя начал работу\n");
    
    int ret;
    
    while(*flag == 0){
        

        memset(message.buff,0,sizeof(message.buff));
        while(msgrcv(msgid,&message,sizeof(message.buff),message.mtype,IPC_NOWAIT) == -1){
            sleep(0.1);
        }
        
            
        
        printf("Прочитано %s\n", message.buff);
        fflush(stdout);
        
    }
    pthread_exit((void*)2);
}



int main(){
    
    pthread_t threadReader_ID;
    printf("Процесс писатель начал работу\n");
    key_t key = ftok("lab7_writer.o",'A');

    msgid = msgget(key,0);//открываем
    if (msgid < 0) {//ошибка открытия
        msgid = msgget(key, IPC_CREAT | 0666);//создаем
        printf("Очередь создана\n");
        perror("msgget");
    }
    else{
        printf("Очередь открыта\n");
    }

    ret_val = pthread_create(&threadReader_ID, NULL, threadReader, &flag_reader);
    threadCreatorErrorHandler(&ret_val);

    getchar();

    flag_reader = 1;
    
    int *exitcode;
    pthread_join(threadReader_ID, (void**)&exitcode);
    msgctl(msgid, IPC_RMID, NULL); 

}