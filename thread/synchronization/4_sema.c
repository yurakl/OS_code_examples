
// Приклад використання семафорів
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#define NUM_THREADS 2

#define NTZ  1
// дескриптор семафору
sem_t semaphore;

// Функція для роботи в потоці
void *thread_function(void *arg) {
    int id = *((int *) arg);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 3;
    // Забезпечення доступу до змінної counter за допомогою семафора
    if(!sem_timedwait(&semaphore, &ts)) {
        printf("Потік %d: знаходиться в критичній секції %d секунд.\n", id, 5);
        sleep(5);
        printf("Потік %d: виходить \n", id);
        sem_post(&semaphore);
    } else {
        printf("Потік %d: не дочекався доступу.\n", id);
    }


    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];

    // Ініціалізація семафора
    sem_init(&semaphore, 0, NTZ); // 1 - початкове значення семафора

    // Створення потоків
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
    }

    // Очікування завершення потоків
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Знищення семафора
    sem_destroy(&semaphore);

    return 0;
}
