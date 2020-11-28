#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 20
#define NUMITEMS 30

typedef struct{
    int item[BUFFER_SIZE];
    int totalitems;
    int in, out;
    pthread_mutex_t mutex;
    pthread_cond_t full;
    pthread_cond_t empty;
}buffer_t;

buffer_t buffer = {{0,0,0,},0,0,0,
                   PTHREAD_MUTEX_INITIALIZER,
                   PTHREAD_COND_INITIALIZER,
                   PTHREAD_COND_INITIALIZER};

int produce_item(){
    int item = (int)(100.0 * rand()/(RAND_MAX + 1.0));
    sleep((unsigned long)(5.0*rand()/(RAND_MAX+1.0)));
    printf("produce item is ~ : item=%d\n", item);
    return item;
}

int insert_item(int item){
    int status;

    status = pthread_mutex_lock(&buffer.mutex);
    if(status != 0){
        return status;
    }

    while (buffer.totalitems >= BUFFER_SIZE && status == NULL){
        status = pthread_cond_wait(&buffer.empty, &buffer.mutex);
        if(status != 0) {
            pthread_mutex_unlock(&buffer.mutex);
            return status;
        }
    }
    buffer.item[buffer.in] = item;
    buffer.in = (buffer.in + 1)%BUFFER_SIZE;
    buffer.totalitems++;

    if(status = pthread_cond_signal(&buffer.full)){
        pthread_mutex_unlock(&buffer.mutex);
        return status;
    }
    return pthread_mutex_unlock(&buffer.mutex);
}

void consume_item (int item)
{
    sleep(1);
    printf("consume_item is~ : item=%d\n", item);
}

int remove_item (int *temp)
{
    int status;
    status = pthread_mutex_lock (&buffer.mutex);
    if (status != 0)
        return status;
    while (buffer.totalitems <= 0 && status == NULL)
        status = pthread_cond_wait (&buffer.full, &buffer.mutex);
    if (status != 0) {
        pthread_mutex_unlock(&buffer.mutex);
        return status;
    }
    *temp = buffer.item[buffer.out];
    buffer.out = (buffer.out + 1) % BUFFER_SIZE;
    buffer.totalitems--;
    if (status = pthread_cond_signal(&buffer.empty)) {
        pthread_mutex_unlock (&buffer.mutex);
        return status;
    }
    return pthread_mutex_unlock (&buffer.mutex);
}

void *producer(void *arg){
    int item;

    while(1){
        item = produce_item();
        insert_item(item);
    }
}

void *consumer(void *arg){
    int item;
    while(1){
        remove_item(&item);
        consume_item(item);
    }
}

int main(){
    int status;
    void *result;
    pthread_t producer_tid, consumer_tid;

    status = pthread_create(&producer_tid, NULL, producer, NULL);
    if(status != 0){
        perror("Create thread~");
    }
    status = pthread_create(&consumer_tid, NULL, consumer, NULL);
    if(status != 0){
        perror("Create thread");
    }
    status = pthread_join (producer_tid, NULL);
    if (status != 0) {
        perror("Join producer thread");
    }
    status = pthread_join (consumer_tid, NULL);
    if (status != 0) {
        perror("Join consumer thread");
    }
}
