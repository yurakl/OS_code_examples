
//~ В даному прикладі використаємо функцію sem_timedwait - чекаємо на доступ до критичної секції коду певний час.
//~ Якщо час вийшов, а доступу немає - функція повертає помилку
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

//~ Створимо NUM_THREADS потоків
#define NUM_THREADS 2
//~ Кількість потоків, яким дозволено працювати в критичній секції коду
#define NTZ  1
//~ Об'єкт семафору - через нього відбувається контроль доступу до критичної секції коду.
//~ Цей об'єк має бути глобальною змінною - всі потоки мають мати доступ до даного об'єкту
sem_t semaphore;

//~ Функція для роботи в потоці
void *thread_function(void *arg) {
	int id = *((int *) arg);
	//~ Необхідно оголосити структуру для роботи з часом
	struct timespec ts;
	//~ Функція sem_timedwait має мати як аргумент абсолютний час, до якго вона чекатиме доступ до секції коду.
	//~ Тому спочатку береться абсолютний час - системний час
	clock_gettime(CLOCK_REALTIME, &ts);
	//~ Та додається час необхідний для очікування. Тут це 3 секунди
	ts.tv_sec += 3;
	//~ Перевірка доступу до критичної секції коду
	if(!sem_timedwait(&semaphore, &ts)) {
		printf("Потік %d: знаходиться в критичній секції %d секунд.\n", id, 5);
		//~ Поставимо на паузу потік в критичній секції коду
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
