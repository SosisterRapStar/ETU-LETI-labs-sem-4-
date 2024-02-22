#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>




int main(){
    pid_t	pid	=	fork();
    int status;

    char path[] = "./lab4_1";
    char message[] = "415 база ответьте";

    char *args[] = {path, message, NULL};

    if (pid == 0){
        execv(args[0], args);
        
    }
    else if (pid > 0){
        while (waitpid(pid, &status, WNOHANG) == 0) {
            sleep(1);
            printf("Главный процесс ждет\n");
            
        }

    }
    else{
        perror("Ай бля больно в ноге");
    }
    
}