// file: lab4-pthread.c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 4
#define NUM_ITERATIONS 1000000

int counter = 0;

// TODO3: define global variables (~1 line)
sem_t semaphore; // 互斥锁变量

void *count_up(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // TODO3: Protect the counter increment operation to prevent race conditions
        sem_wait(&semaphore); // 等待信号量
        counter++;
        sem_post(&semaphore); // 释放信号量
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];

    // TODO3: init condition variables/semphore (~1 line)
    sem_init(&semaphore, 0, 1); // 初始值为1，表示只有一个线程可以进入临界区

    for (int i = 0; i < NUM_THREADS; i++) {
        // TODO1: Create multiple threads to execute count_up (~1 line)
        pthread_create(&threads[i], NULL, count_up, NULL); // Create multiple threads
    }

    for (int i = 0; i < NUM_THREADS; i++) {
		// TODO2: wait all the threads to finish running (~1 line)
        pthread_join(threads[i], NULL);
    }

    // TODO3: free condition variables/semphore (~1 line)
    sem_destroy(&semaphore);

    printf("Final counter value: %d\n", counter);
    printf("Expected counter value: %d\n", NUM_ITERATIONS * NUM_THREADS);

    return 0;
}