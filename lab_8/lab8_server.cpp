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

using namespace std;

vector <string> msglist;
int discriptor_listener;
int discriptor_worker;
int ret_val;
struct sockaddr_in serverAddr;
struct sockaddr_in clientAddr;

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

    while(flag == 0){
        char buff[128];

        int recBytes = recv(discriptor_worker, buff, sizeof(buff), 0);
        if (recBytes == -1){
            perror("Receive error: ");
            sleep(1);
        
        }
        else if (recBytes == 0) {
            //разъединение;
            sleep(1);
        }
        else{
            // захватить мьютекс
            shutdown(discriptor_worker ,SHUT_RDWR);
            string received(buff);
            msglist.push_back(received);

        }

        
    }
}


static void * threadSender (void *flag_sender){
    int *flag = (int*) flag_sender;

    while(flag == 0){
        // захватить мьютекс
        if (!msglist.empty()){
            string S = msglist.back();
            msglist.pop_back();
            // освободить мьютекс   
            char buff[128];
            struct netent result_buf;
            struct netent *result;
            int r = getnetent_r(&result_buf, buff, sizeof(buff), &result, NULL);
            getnetError(&r);

            string buffString(buff);
            string message = S + "\n" + buffString;

            int bytesSended = send(discriptor_worker, message.c_str(), message.length(), 0);
            if (bytesSended == -1){
                perror("Sending error: ");
            }
            else{
                printf("\nСообщение отправлено\n");
            }
        }
        else{
            // освободить мьютекс
            sleep(1);
        }

    }
}

static void * threadConnector (void *flag_connector){
    int *flag = (int*) flag_connector;

    clientAddr.sin_family =	AF_INET;
    clientAddr.sin_port = htons(8000);
    clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    unsigned int clientSockLen = sizeof(clientAddr);

    while(flag == 0){
        discriptor_worker = accept(discriptor_listener, (struct sockaddr *) &clientAddr, &clientSockLen);
        if (discriptor_worker != -1){
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

    
    discriptor_listener = socket(AF_INET, SOCK_STREAM, 0);
    sockCreateCheck(&discriptor_listener);

    serverAddr.sin_family =	AF_INET;
    serverAddr.sin_port = htons(7000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");



    ret_val = bind(discriptor_listener, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    bindingCheck(&ret_val);

    int optval = 1;
    ret_val = setsockopt(discriptor_listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    socketOptionsFuncCheck(&ret_val);

    ret_val = listen(discriptor_listener, 1);
    listenCheck(&ret_val);


    printf("Нажмите любую кнопку");
    getchar();

    flag_receiver = 1;
    flag_sender = 1;
    flag_connector = 1;


    

    

    


}