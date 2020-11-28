#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 3


pthread_mutex_t mutex;
int sum;
int stat = 0;
int call_th = 0;


pthread_t    tid[5];
pthread_mutex_t    mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t    cond = PTHREAD_COND_INITIALIZER;

void *child_thread(void *arg) {
    while (1) {
        //pthread_mutex_lock(&mutex);
        sleep(1);
        pthread_mutex_lock(&mutex);
        int random = rand() % 100;
        if (random < 10){
            printf("\n%d th child process send to msg call!\n", *((int *)arg));
            stat = 1;
            call_th = *((int *)arg);
        }
        pthread_cond_wait(&cond, &mutex);
        printf("%d th child process receive msg!\n", *((int *) arg));
        printf("%d th child process receive msg!\n", *((int *) arg));

        pthread_mutex_unlock(&mutex);
    }
}

void *parent_thread(void *arg){
    while(1){
        sleep(2);
        pthread_cond_broadcast(&cond);
        printf("\nmsg call is send!\n\n");
        sleep(1);
        if (stat == 1) {
            pthread_mutex_lock(&mutex);
            pthread_cond_broadcast(&cond);
            printf("\nParent process : receive msg call !\n\n");
            stat = 0;
            pthread_mutex_unlock(&mutex);
        }
    }
}


void main() {
    srand(time(NULL));
    int i;
    int id[5];
    int call_th = 0;

    pthread_create(&tid[0], NULL, parent_thread, (void*)&id[0]);
    for(i = 1; i < 5; i++){
        id[i] = i;
        pthread_create(&tid[i],NULL,child_thread,(void*)&id[i]);
    }
    for(i = 1; i < 5 ; i++){
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
