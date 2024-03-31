//~ Приклад використання функції pthread_mutex_timedlock
//~ Використання цієї функції дозволяє очікувати на доступ до критичної секції коду лише певний час
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

	//~ Необхідно оголосити структуру для роботи з часом
	struct timespec ts;
	//~ Функція pthread_mutex_timedlock має мати як аргумент абсолютний час, до якого вона чекатиме доступ до секції коду.
	//~ Тому спочатку береться абсолютний час - системний час
	clock_gettime(CLOCK_REALTIME, &ts);
	//~ Та додається час необхідний для очікування. Тут це 3 секунди
	ts.tv_sec += 3;

	if(!pthread_mutex_timedlock(&mymutex, &ts)) {
		printf("Потік %d: знаходиться в критичній секції %d секунд.\n", id, 5);
		//~ Поставимо на паузу потік в критичній секції коду
		sleep(5);
		printf("Потік %d: виходить \n", id);
		pthread_mutex_unlock(&mymutex);
	} else {
		printf("Потік %d: не дочекався доступу.\n", id);
	}
	
	//~ Якщо один потік виходить з критичної секції, а наступний заходить, то таймер очікування в інших потоках в черзі не оновлюється.
	//~ Наприклад, всі потоки чекають 3 секунди. Перший потік працював у критичній секції 2 секунди і вийшов,
	//~ зайшов наступний і теж 2 секунди працює. Всі інші вже не потраплять в цю секцію (2+2 = 4 > час очікування 3 секунди)
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

