#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;


int descriptor_listener;
int descriptor_worker;
int ret_val;
struct sockaddr_in socketAddr;
struct sockaddr_in serverAddr;

int flag_receiver = 0;
int flag_sender = 0;
int flag_connector = 0;

pthread_t threadReceiver_Id;
pthread_t threadSender_Id;
pthread_t threadConnector_Id;





void sockCreateCheck(int *ret_val){
    if (*ret_val == -1){
        perror("Socket error: ");
    }
}

void bindingCheck(int *ret_val){
    if (*ret_val == -1){
        perror("Binding check erorr: ");
    }
}

void socketOptionsFuncCheck(int *ret_val){
    if (*ret_val == -1){
        perror("Sockopt error: ");
    }
}

void listenCheck(int *ret_val){
    if (*ret_val){
        perror("Listnening error: ");
    }
}

void acceptCheck(int *ret_val){
    if (*ret_val){
        perror("Accept conn error: ");
    }
}

void threadCreatorErrorHandler(int* ret_val){
    if(*ret_val != 0) {
                perror("Thread creating error: "); 
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



static void * threadReceiver (void *flag_receiver){
    int *flag = (int*) flag_receiver;

    while(*flag == 0){
        char buff[128];

        int recBytes = recv(descriptor_listener, buff, sizeof(buff), 0);

        if (recBytes == -1){
            // perror("Receive error: ");
            // sleep(1);
        
        }else{
            string response(buff);
            cout << "Ответ сервера: " << response << "\n";
            sleep(1); 
        }
        
    }
     pthread_exit((void*)2);
}


static void * threadSender (void *flag_sender){
    int *flag = (int*) flag_sender;
    int counter = 1;
    while(*flag == 0){
            counter += 1;
            string str_counter = to_string(counter);
            string request = " запрос " + str_counter;
            cout << "Отправлен запрос номер " << counter << " " << '\n';
            send(descriptor_listener, request.c_str(), request.length(), 0);
            sleep(1);
        }
    pthread_exit((void*)3);
    }


static void * threadConnector (void *flag_connector){
    int *flag = (int*) flag_connector;
    printf("\nПоток подключения к серверу начал работу\n");

    
    serverAddr.sin_family =		AF_INET;
    serverAddr.sin_port =		htons(7000);
    serverAddr.sin_addr.s_addr =	inet_addr("127.0.0.1");
    

    while(*flag == 0){
        ret_val = connect(descriptor_listener, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
        if (ret_val == 0){
            printf("Соединение установлено");
            ret_val = pthread_create(&threadReceiver_Id, NULL, threadReceiver, &flag_receiver);
            threadCreatorErrorHandler(&ret_val);

            ret_val = pthread_create(&threadSender_Id, NULL, threadSender, &flag_sender);
            threadCreatorErrorHandler(&ret_val);

            pthread_exit((void*)1);
        }
        
    }
    pthread_exit((void*)1);
}



int main(){

    // typedef struct sender_args
    // {
    //     int *flag;
    //     vector <string> *queue;
    // };

    // typedef struct receiver_args
    // {
    //     int *flag;
    //     vector <string> *queue;
    // };
    

    // typedef struct connection_args
    // {
    //     int *flag;
    //     pthread_t *threadReceiver_Id_P;
    //     pthread_t *threadSender_Id_P;
    //     struct receiver_args;
    //     struct sender_args;
        
    // };

    
    descriptor_listener = socket(AF_INET, SOCK_STREAM, 0);
    sockCreateCheck(&descriptor_listener);

    socketAddr.sin_family =	AF_INET;
    socketAddr.sin_port = htons(8000);
    socketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");



    ret_val = bind(descriptor_listener, (struct sockaddr *) &socketAddr, sizeof(socketAddr));
    bindingCheck(&ret_val);

    int optval = 1;
    ret_val = setsockopt(descriptor_listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    socketOptionsFuncCheck(&ret_val);

    ret_val = pthread_create(&threadConnector_Id, NULL, threadConnector, &flag_connector);
    threadCreatorErrorHandler(&ret_val);


    printf("Нажмите любую кнопку");
    getchar();

    flag_receiver = 1;
    flag_sender = 1;
    flag_connector = 1;

    close(descriptor_listener);
    close(descriptor_worker);
    int *exitcode;

    
    pthread_join(threadConnector_Id, (void**)&exitcode);
    pthread_join(threadReceiver_Id, (void**)&exitcode);
    pthread_join(threadSender_Id, (void**)&exitcode);


}