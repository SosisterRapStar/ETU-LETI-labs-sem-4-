#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int ret_val;

void setenvError(int* ret_val){
    if (*ret_val == -1){
        perror("\nОшибка настройки PATH\n");
    }
}
    

void execError(int* ret_val){
    if (*ret_val == -1){
        perror("\nОшибка вызова программы дочернего процесса\n");
    }
}


int main(int argc, char *argv[]){
    pid_t pid = getpid();
    pid_t ppid = getppid();

    printf("\nPID основной программы %d\n", pid);
    printf("\nPID родителя основной программы %d\n", ppid);

    pid	= fork();
    int status;
    
    char cwd[200];
    getcwd(cwd, sizeof(cwd));

    char *envp[] = {
        "ENV_VAL = 1",
        "ENV_VAL_2 = 2",
        "HL = HELLO_WORLD",
        NULL,
        };

    ret_val = setenv("PATH", cwd, 1);
    setenvError(&ret_val);

    if (pid == 0){

        ret_val = execvpe("lab4_1", argv, envp);
        execError(&ret_val);
        
    }
    else if (pid > 0){

        while (waitpid(pid, &status, WNOHANG) == 0) {
            sleep(0.5);
        }

        if (WIFEXITED(status)) {

            status = WEXITSTATUS(status);
            printf("\nКод завершения дочерней программы: %d\n", status);

        } else {
            printf("\nДочерний процесс завершился ненормально\n");
        }

    }
    else{
        perror("\nОшибка fork\n");
    }
    
}