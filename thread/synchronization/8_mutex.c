//~ Приклад використання м'ютекса
//~ Тут використовуємо функцію pthread_mutex_trylock
//~ Дана функція перевіряє доступ у критичну секцію, але на відміну від pthread_mutex_lock, не відправляє потік у сон,
//~ коли доступу до критичної секції немає. В такому випадку, вона повертає негативне значення.
//~ Використовуючи if-else, ми перевіряємо повернене функцією значення, та плануємо поведінку потоку
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define NUM_THREADS 5

//~ Об'єкт м'ютексу - через нього відбувається контроль доступу до критичної секції коду.
//~ Цей об'єкт має бути глобальною змінною - всі потоки мають мати доступ до даного об'єкту
pthread_mutex_t mymutex;

//~ Функція для роботи в потоці
void *thread_function(void *arg) {
	int id = *((int *) arg);

	//~ Перевірка доступу до критичної секції коду
	if(!pthread_mutex_trylock(&mymutex)) {
		printf("Потік %d: знаходиться в критичній секції.\n", id);
		sleep(3);
		printf("Потік %d: виходить.\n", id);
		//~ Відпускаємо м'ютекс навиході з критичної секцї коду
		pthread_mutex_unlock(&mymutex) ;
	} else {
		//~ Доступу до критичної секції немає - м'ютекс заблокований
		printf("Потік %d: не отримав доступу до критичної секції.\n", id);
	}
	
	pthread_exit(NULL);
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];

	//~ Ініціалізація м'ютекса
	//~ Тут NULL означає, що ми використовуємо налаштування м'ютексу за замовчуванням
	pthread_mutex_init(&mymutex, NULL);

	//~ Створення потоків
	for (int i = 0; i < NUM_THREADS; ++i) {
		thread_args[i] = i;
		pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
	}

	//~ Очікування завершення потоків
	for (int i = 0; i < NUM_THREADS; ++i) {
		pthread_join(threads[i], NULL);
	}

	//~ Знищення м'ютекса
	pthread_mutex_destroy(&mymutex);

	return 0;
}

