
// Приклад використання семафорів
//~ В даному прикладі використовується функція sem_trywait
//~ Ця функція повертає 0, якщо доступ до критичної ділянки коду доступна, -1 - доступ неможливий

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//~ Створимо NUM_THREADS потоків
#define NUM_THREADS 5
//~ Початкове значення семафора - оскільки кожний потік на вході в критичну секцію зменшує значення лічильника,
//~ а на виході збільшує - то NTZ потоків зможуть працювати в критичній секції.
#define NTZ  3
//~ Об'єкт семафору - через нього відбувається контроль доступу до критичної секції коду.
//~ Цей об'єкт має бути глобальною змінною - всі потоки мають мати доступ до даного об'єкту
sem_t semaphore;

//~ Функція для роботи в потоці
void *thread_function(void *arg) {
	int id = *((int *) arg);

	//~ Перевірка доступу до критичної секції коду
	if(!sem_trywait(&semaphore)) {
	    
		printf("Потік %d: знаходиться в критичній секції\n", id);
		sleep(1 + id);
		printf("Потік %d: виходить \n", id);
        
	} else {
		perror("Не вдалось увійти в критичну секцію");
	}

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
