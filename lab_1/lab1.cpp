#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>

void setScheduleParamsError(int* ret_val);
void mainThreadErrorHandler(int* ret_val);
void getScheduleParamsError(int* ret_val);





static void * threadFunc_1 (void *flag1){
    
    
    sleep(2);

    int *flag = (int*) flag1;
    printf("Поток 1 начал свою работу\n");

    while (*flag == 0) {
        putchar('1');
        fflush(stdout);
        sleep(1);
    }

    pthread_exit((void*)1);

}

static void * threadFunc_2 (void *flag2){
    


    sleep(2); 
    printf("Поток 2 начал свою работу\n");

    int *flag = (int*) flag2;

    while (*flag == 0) {
        putchar('2');
        fflush(stdout);
        sleep(1);
        
    }

    pthread_exit((void*)2);

}

void mainThreadErrorHandler(int* ret_val){
    if(*ret_val != 0) {
                perror("Ошибка создания потока"); 
        }
}

void setScheduleParamsError(int* ret_val){
    if(*ret_val != 0) {
                perror("Ошибка изменения параметров потока: "); 
        }
}

void getScheduleParamsError(int* ret_val){
    if(*ret_val != 0) {
                perror("Ошибка получения параметров потока: "); 
        }
}


// возвращает метод планирования
void getSchedulePolicy(int* schedulePolicy){
    switch (*schedulePolicy) {

        // no-real-time
        
        case SCHED_OTHER:
            printf("Стандартный планировщик SCHED_OTHER\n");
            break;
            // стандартное выполнение задач на основе приоритета (относительно real-time задач)
            // приоретет sched_other и других non-real-time задач будет 0

        // real-time
        case SCHED_FIFO:
            printf("real-time стратегия планирования по приоритетам SCHED_FIFO\n");
            break;
        case SCHED_RR:
            printf("real-time стратегия планирования по времени SCHED_RR\n");
            break;
    }

}


// возвращает приоретет потока
void getSchedulingParam(sched_param* param){
    printf("Приоретет потока = %d\n", param->sched_priority);
}


int main(){ 
    printf("Программа начала свою работу\n");
    printf("Структура sched_param содержит в себе одно поле, которое отвечает за изменение приоритета потока\n");


    int flag1 = 0;
    int flag2 = 0;
    int ret_val;



    pthread_t threadId_1;
    pthread_t threadId_2;
  
    

    struct sched_param paramId_1;
    struct sched_param paramId_2;
    struct sched_param paramMain;
    int priority;
    int policy;


    
    printf("\n");





    ret_val = pthread_create(&threadId_1, NULL, threadFunc_1, &flag1);
    mainThreadErrorHandler(&ret_val);
    
    ret_val = pthread_create(&threadId_2, NULL, threadFunc_2, &flag2);
    mainThreadErrorHandler(&ret_val);


    printf("Изначальные параметры и планировщик для потока 1\n");
    ret_val = pthread_getschedparam(threadId_1, &policy, &paramId_1);
    getScheduleParamsError(&ret_val);
    getSchedulingParam(&paramId_1);
    getSchedulePolicy(&policy);

    printf("\n");

    printf("Изначальные параметры и планировщик для потока 2\n");
    ret_val = pthread_getschedparam(threadId_2, &policy, &paramId_2);
    getScheduleParamsError(&ret_val);
    getSchedulingParam(&paramId_2);
    getSchedulePolicy(&policy);

    printf("\n");

    

    


    printf("Изменение метода планирования и параметров для потока 1\n");
    printf("Поток 1 получает наибольший приоретет\n");
    paramId_1.sched_priority = 10;
    ret_val = pthread_setschedparam(threadId_1, SCHED_FIFO, &paramId_1);
    setScheduleParamsError(&ret_val);

  
    printf("\n");


    printf("Новые параметры и планировщик для потока 1\n");
    ret_val = pthread_getschedparam(threadId_1, &policy, &paramId_1);
    getScheduleParamsError(&ret_val);
    getSchedulingParam(&paramId_1);
    getSchedulePolicy(&policy);

    sleep(2.75);

    
    printf("Программа ждет нажатия клавиши\n");
    getchar();
    

    

    printf("Клавиша нажата\n");

    flag1 = 1;
    flag2 = 1;

    int *exitcode1;
    int *exitcode2;

    ret_val = pthread_join(threadId_1, (void**)&exitcode1);
    ret_val = pthread_join(threadId_2, (void**)&exitcode2);

    printf("exitcode1 = %p\nexitcode2 = %p\n", exitcode1, exitcode2);

    printf("Программа завершила свою работу\n");


}  
