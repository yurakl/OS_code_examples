//~ Приклад використання семафору, подібний до попередніх
//~ Тут використовується функція sem_getvalue, що дозволяє отримати значення лічильника семафора -
//~ кількасть вільних мість в критичній секції коду
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
	int id = *((int *) arg);

	//~ Перевірка доступу до критичної секції коду
	sem_wait(&semaphore);
	//~ Змінна, куди запишемо кількість вільних місць в критичній секції коду
	int slots;
	//~ Отримання значення лічильника семафору - кількість вільних місць в критичній секції коду
	sem_getvalue(&semaphore, &slots);
	printf("Потік %d: знаходиться в критичній секції. Залишилось вільних місць: %d\n", id, slots);
	sleep(1 + id);
	printf("Потік %d: виходить \n", id);
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
