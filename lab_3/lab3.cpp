#include <netdb.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

int flag_reader = 0;
int flag_writer = 0;
int file_descriptor[2];
int ret_val;
ssize_t wr_ret_val; 
int global_net_error;

void readHandler(ssize_t* ret_val);
void writeHandler(ssize_t* ret_val);
void getnetError(int* ret_val); 

static void * threadReader (void *flag_reader){
    char buff[100];
    int *flag = (int*) flag_reader;

    while(*flag == 0){
        memset(buff, 0, sizeof(buff));

        wr_ret_val = read (file_descriptor[0], &buff, 100);
        readHandler(&wr_ret_val);

        printf ("%s", buff);
        sleep(1);
    }

    pthread_exit((void*)1);

}

static void * threadWriter (void *flag_writer){
    
    int *flag = (int*) flag_writer;
    
    struct netent result_buf;
    struct netent *result;
    
    char buff[100];
    int r = getnetent_r(&result_buf, buff, sizeof(buff), &result, &global_net_error);
    getnetError(&r);

    while(*flag == 0){
      wr_ret_val = write (file_descriptor[1], &buff, 100);
      writeHandler(&wr_ret_val);

      sleep(1);
    }

    pthread_exit((void*)2);

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


void threadCreatorErrorHandler(int* ret_val){
    if(*ret_val != 0) {
                perror("Ошибка создания потока"); 
        }
}

void pipeCreatorError(int* ret_val){
    if(*ret_val != 0) {
                perror("Ошибка создания канала"); 
        }
}

void readHandler(ssize_t* ret_val){
    if (*ret_val > 0){
        printf("\nДанные прочитаны\n");
        return;
    }

    switch(*ret_val){
        case 0:
            printf("Достигнут конец файла\n");
            break;
        case EBADF:
            perror("Невалидный дескриптор файла\n");
            break;
        case EFAULT:
            perror("Указатель на буффер вне зоны доступного пользовательского адрессного пространства\n");
            break;
        case -1:
            perror("Ресурс временно не доступен так как буфер пуст\n");
            break;
        
        default:
            perror("Возникла ошибка чтения\n");
            break;
    }

    
}

void writeHandler(ssize_t* ret_val){

    if (*ret_val > 0){
        printf("\nДанные записаны\n");
        return;
    }

    switch(*ret_val){
        case EBADF:
            perror("Невалидный дескриптор файла\n");
            break;
        case EFAULT:
            perror("Указатель на буффер вне зоны доступного пользовательского адрессного пространства\n");
            break;
        case EFBIG:
            perror("Была предпринята попытка записи файла слишком большого размера, не предусмотренный реализацией метода\n");
            break;
        case EINVAL:
            perror("Дескриптор привязан к объекту, который не предназначен для записи\n");
            break;
        case ENOSPC:
            perror("Объект привязанный к файловому дескриптору не имеет достаточно пространства для записи\n");
            break;
        case EPERM:
            perror("Запись невозможна из-за F_SEAL_WRITE или других параметров fcntl\n");
            break;
        case EPIPE:
            perror("Канал для записи закрыт\n");
            break;
        case -1:
            perror("Ресурс временно не доступен так как буфер полон\n");
            break;
        default:
            perror("Возникла ошибка записи\n");
            break;
    }
}

void sig_handler(int signo)
{
    printf("\nСигнал завершения %d\n", signo);
    flag_writer = 1;
    flag_reader = 1;

    close(file_descriptor[0]);
    close(file_descriptor[1]);

    exit(0);
}



int main(int argc, char *argv[]){
    signal(SIGINT, sig_handler);

    pthread_t threadId_writer;
    pthread_t threadId_reader;

    
    if (!strcmp(argv[1], "pipe2")){
        printf("Открытие неблокируещего канала средствами pipe2\n");
        ret_val = pipe2(file_descriptor, O_NONBLOCK);
        pipeCreatorError(&ret_val);
    }

    else if (!strcmp(argv[1], "fcntl")){
        printf("Открытие неблокируещего канала средствами fcntl\n");
        ret_val = pipe(file_descriptor);
        pipeCreatorError(&ret_val);
        fcntl(file_descriptor[0], F_SETFL, O_NONBLOCK);
        fcntl(file_descriptor[1], F_SETFL, O_NONBLOCK);
    }

    else{
        printf("Открытие неблокируещего канала средствами pipe\n");
        ret_val = pipe(file_descriptor);
        pipeCreatorError(&ret_val);
    }

    ret_val = pthread_create(&threadId_writer, NULL, threadWriter, &flag_writer);
    threadCreatorErrorHandler(&ret_val);
    ret_val = pthread_create(&threadId_reader, NULL, threadReader, &flag_reader);
    threadCreatorErrorHandler(&ret_val);

    sleep(1.75);

    printf("\nПрограмма ждет нажатия клавиши\n");
    getchar();
    
    printf("\nКлавиша нажата\n");

    flag_reader = 1;
    flag_writer = 1;

    int *exitcode1;
    int *exitcode2;

    ret_val = pthread_join(threadId_reader, (void**)&exitcode1);
    ret_val = pthread_join(threadId_writer, (void**)&exitcode2);

    printf("exitcode1 = %p\nexitcode2 = %p\n", exitcode1, exitcode2);
    
    close(file_descriptor[0]);
    close(file_descriptor[1]);
    
}
