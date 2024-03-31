//~ Приклад перевірки спільного доступу потоків до спільних ресурсів - глобальної змінної типу int.
//~ Програма створює NUM_THREADS потоків, кожний з яких виконує функцію thread_function.
//~ В цій функції кожний потік LOCAL_COUNT разів інкрементує глобальний лічильник counter.
//~ Тобто, по завершенню потоків значення лічильника має бути NUM_THREADS * LOCAL_COUNT.
//~ Однак тестові запуски програми показують, що це не завжди отримуємо значення NUM_THREADS * LOCAL_COUNT

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//~ Кількість потоків
#define NUM_THREADS 10
//~ Кожний потік LOCAL_COUNT разів збільшує значення лічильника на 1.
#define LOCAL_COUNT 100000 
//~ Глобальний лічильник
int counter = 0;

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

