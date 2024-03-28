//~ Приклад, подібний до попереднього, однак тут:
//~ Один виконує функцію sender -  потік змінює лічильник TCOUNT разів
//~ і звіряє значення лічильника із COUNT_LIMIT_1 та COUNT_LIMIT_2.
//~ В разі рівності із однією із величин, потік надсилає сигнал через умовну зміннну.
//~ Два інші потоки виконують кожен свою функцію: один - waiter_1, другий - waiter_2
//~ Перший потік (waiter_1) очікує на значення лічильника COUNT_LIMIT_1
//~ Другий потік (waiter_2) очікує на значення лічильника COUNT_LIMIT_2

//~ Умовна змінні - це тільки механізм надсилання сигналів про певну подію.
//~ Задача потоку, що очікує на подію, перевірити, чи це точно та подія, яка необхідна йому для продовження роботи
//~ Об'єкти м'ютекса та умовної змінної одні для всіх функцій - бо ми працюємо із одними даними

//~ В цьому прикладі поведінка потоків ідентична, однак замість pthread_cond_signal,
//~ що надсилає одиничний сигнал - пробуджує один потік,
//~ ми використовуємо функції pthread_cond_broadcast - пробуджує всі потоки,
//~ які заснули  через певну умовну змінну condition_variable. Всі потоки перевіряють свої умови, і, в залежності від її виконання,
//~ або продовжують роботу, або знову засинають.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TCOUNT		 	20					// Кожний потік інкрементує лічильник TCOUNT разів
#define COUNT_LIMIT_1 		6					// ліміт лічильника
#define COUNT_LIMIT_2 		12					// ліміт лічильника

int count = 0;								// Лічильник
pthread_mutex_t 			mutex;				// м'ютекс 
pthread_cond_t 			condition_variable;		// умовна змінна

void *sender(void *t)
{
	int my_id =  * (int *) t;

	for (int i = 0; i < TCOUNT; i++) {

		// Один потік закриває вхід для іншого, поки працює із глобальними змінними
		pthread_mutex_lock(&mutex);

		//	Запускаємо лічильник

		count++;
		printf("Потік %d змінив лічильник: %d\n", my_id, count);
		// Перевіряємо значення лічильника

		if (count == COUNT_LIMIT_1) {
			// якщо вже досягли ліміту - сигналізуємо іншому потоку
			printf("Потік %d, який змінює лічильник, дійшов до ліміту (%d). Надсилаємо сигнал тому, що очікує... ", my_id, COUNT_LIMIT_1);
			pthread_cond_broadcast(&condition_variable);
			printf("Сигнал відправлено.\n");
		} else if (count == COUNT_LIMIT_2) {
			// якщо вже досягли ліміту - сигналізуємо іншому потоку
			printf("Потік %d, який змінює лічильник, дійшов до ліміту (%d). Надсилаємо сигнал тому, що очікує... ", my_id, COUNT_LIMIT_2);
			pthread_cond_broadcast(&condition_variable);
			printf("Сигнал відправлено.\n");
		}
		// Відкриваємо вхід іншому потоку.
		pthread_mutex_unlock(&mutex);
		// для тестів сповільнимо роботу
		sleep(1);
	}
	pthread_exit(NULL);
}
//Перший обробник, який по ідеї би мав чекати на значення COUNT_LIMIT_1
void *waiter_1(void *t)
{
	//~ В даній програмі, потоки виконують мало операцій  перед перевіркою умовної змінної - вистачає часу
	//~ виділеного планувальником на один цикл роботи, щоб дійти до умовної змінної без переривань
	//~ Тут ми дамо паузу для імітації бурхливої рооботи потоку
	sleep(1);
	int my_id =  * (int *) t;

	
	//~ тут спимо до отримання сигналу
	while( count != COUNT_LIMIT_1) {
		printf("Потік %d очікує значення лічильника: %d\n", my_id, COUNT_LIMIT_1);
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condition_variable, &mutex);
		pthread_mutex_unlock(&mutex);
		printf("Потік %d (чекає на %d) отриимав сигнал. Перевіряє умову для продовження роботи...\n", my_id, COUNT_LIMIT_1);
	}
	printf("Потік %d (чекає на %d) дочекався значення лічильника %d\n", my_id, COUNT_LIMIT_1, count);

	pthread_exit(NULL);
}

//Другий обробник, який по ідеї би мав чекати на значення COUNT_LIMIT_2
void *waiter_2(void *t)
{
	//~ Тут ми дамо паузу для імітації бурхливої рооботи потоку
	sleep(1);
	int my_id =  * (int *) t;
	
	//~ тут спимо до отримання сигналу
	while( count != COUNT_LIMIT_2) {
		printf("Потік %d очікує значення лічильника: %d\n", my_id, COUNT_LIMIT_2);
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condition_variable, &mutex);
		pthread_mutex_unlock(&mutex);
		printf("Потік %d (чекає на %d) отриимав сигнал. Перевіряє умову для продовження роботи...\n", my_id, COUNT_LIMIT_2);
	}
	printf("Потік %d (чекає на %d) дочекався значення лічильника %d\n", my_id, COUNT_LIMIT_2, count);

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	//~ Отримуємо ID процесу та друкуємо його в термінал
	pid_t init_pid = getpid();
	printf("ID процесу: %d\n", init_pid);
	pthread_t threads[3];
	//~ Оголосимо деякі змінні типу int, ядреси яких передамо в функцію потку як аргумент
	int num[3] = {1, 2, 3};

	// Ініціалізуємо м'ютекси та умовну змінну
	pthread_mutex_init(&mutex, NULL); 
	pthread_cond_init (&condition_variable, NULL);
	
	//~ Створюємо три потоки та передамо їм адреси змінної
	//~ Один потік виконує функцію sender
	pthread_create(&threads[0], NULL, sender, (void *) &num[0]);
	 
	//~ Два інші потоки виконують функції waiter_1 та waiter_2, відповідно.
	pthread_create(&threads[1], NULL, waiter_1, (void *) &num[1]);
	pthread_create(&threads[2], NULL, waiter_2, (void *) &num[2]);
	
	//~ Чекаємо на завершення потоків.
	//~ Тут NULL - ми не чекаємо, що функція потоку нам щось поверне
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);

	// знищимо декскриптори об'єктів - не обов'язково, оскільки і так процес завершується
	pthread_mutex_destroy(&mutex); 
	pthread_cond_destroy(&condition_variable);
	return 0; 
}
