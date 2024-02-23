#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <semaphore.h>
#include <sys/epoll.h>

int ret_val;

void openingFileError(int *descriptor){
    if (*descriptor == -1) {
        perror("\nОшибка открытия файла!\n");
        
    }
}

void semaphoreOpeningError(sem_t *sem){
    if (sem == SEM_FAILED){
        perror("\nОшибка открытия или создания семафора\n");
    }

}

void epollCreateError(int *val){
    if (*val == -1){
        perror("\nОшибка создания epoll\n");
    }
}

void epollCTLError(int *ret_val){
    if (*ret_val == -1) {
        perror("\nОшибка функции управления epoll\n");
    }
}

void closingError(int *ret_val){
    if (*ret_val == -1){
        perror("\nОшибка закрытия\n");
    }
}

int main(){
    
    sem_t *sem;
    int descriptor;
    int wait = 0;
    

    sem = sem_open("/mysem", O_CREAT, 0666, 1);
    semaphoreOpeningError(sem);
    

    descriptor = open("example.txt", O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 0666);
    openingFileError(&descriptor);

    int epfd = epoll_create(1);
    epollCreateError(&epfd);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = 0;

    int e_ctl = epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);
    epollCreateError(&e_ctl);

    while(wait == 0){
        sem_wait(sem);
        for(int i = 0; i < 10; i++){
            write(descriptor, "1", 1);
            putchar('1');
            fflush(stdout);
            sleep(1);
        }
        sem_post(sem);
        wait = epoll_wait(epfd, &event, 1, 1000);

    }

    ret_val = close(descriptor);
    closingError(&ret_val);
    sem_close(sem);

    sem_unlink("/mysem");

}