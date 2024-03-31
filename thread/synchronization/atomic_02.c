//~ Даний приклад робить все те саме, однак тепер використано атомарну змінну для лічильника:
//~ _Atomic int counter = ATOMIC_VAR_INIT(0);

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

//~ Кількість потоків
#define NUM_THREADS 10
//~ Кожний потік LOCAL_COUNT разів збільшує значення лічильника на 1.
#define LOCAL_COUNT 100000

//~ Глобальний лічильник
_Atomic int counter = ATOMIC_VAR_INIT(0);

//~ Функція для роботи в потоці
void *thread_function(void *arg) {
	int id = *((int *) arg);
	int i = LOCAL_COUNT;
	while (i > 0) {
		counter++;
		i--;
	}
	pthread_exit(NULL);
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];

	//~ Створення потоків
	for (int i = 0; i < NUM_THREADS; ++i) {
		thread_args[i] = i;
		pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
	}

	//~ Очікування завершення потоків
	for (int i = 0; i < NUM_THREADS; ++i) {
		pthread_join(threads[i], NULL);
	}
	printf("Лічильник: %d\n", counter);
	
	return 0;
}

