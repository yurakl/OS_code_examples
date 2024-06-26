
// Приклад використання семафорів
//~ Семафор дозволяє виконувати певну секцію коду обмеженій кількості потоків одночасно
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//~ Створимо NUM_THREADS потоків
#define NUM_THREADS 5
//~ Кількість потоків, яким дозволено працювати в критичній секції коду
#define NTZ  3

//~ Об'єкт семафору - через нього відбувається контроль доступу до критичної секції коду.
//~ Цей об'єкт має бути глобальною змінною - всі потоки мають мати доступ до даного об'єкту
sem_t semaphore;

//~ Функція для роботи в потоці
void *thread_function(void *arg) {
	int id = * (int *) arg;

	//~ Вхід в критичну секцію - тут перевіряється значення лічильника семафору.
	//~ Якщо N > 0 потік допускається до критичної секції
	//~ Якщо N = 0 потік засинає, доки один інший не вийде із критичної секції
	sem_wait(&semaphore);
	
	printf("Потік %d: знаходиться в критичній секції\n", id);
	//~ Нехай потоки в критичній секції імітують бурхливу діяльність - просто сплять певний час
	sleep(1 + id);
	printf("Потік %d: виходить \n", id);
	//~ Тут потік виходить із критичної секції - збільшує лічильник семафора N та будить потік, що був недопущений до цієї секції
	sem_post(&semaphore);
	
	pthread_exit(NULL);
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];

	//~ Ініціалізація семафора
	//~ NTZ - кількість потоків, що можуть працювати в критичній секції одночасно
	sem_init(&semaphore, 0, NTZ); // NTZ - початкове значення семафора

	//~ Створення потоків
	for (int i = 0; i < NUM_THREADS; ++i) {
		thread_args[i] = i;
		pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
	}

	//~ Очікування завершення потоків
	for (int i = 0; i < NUM_THREADS; ++i) {
		pthread_join(threads[i], NULL);
	}

	//~ Знищення семафора
	sem_destroy(&semaphore);

	return 0;
}
