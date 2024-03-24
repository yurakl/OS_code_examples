//~ Приклад використання умовної змінної:
//~ два потоки виконують функцію sender - по TCOUNT разів інкрементують один лічильник (сумарно 2 * TCOUNT разів),
//~ третій виконує функцію waiter -  чекає доки не досягнеться значення лічильника = COUNT_LIMIT. 
//~ Коли один із "накручувачів" лічильника перевірив, що після додавання на лічильнику вже є значення COUNT_LIMIT,
//~ він надсилає сигнал третьому потоку.
//~ Третій потік, коли отримав сигнал, просинається та обнулює лічильник
//~ Перші два потоки і далі інкремментують лічильник, до вичерпання ліміту TCOUNT

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TCOUNT 10			// Кожний потік інкрементує лічильник TCOUNT разів
#define COUNT_LIMIT 12		// Ліміт лічильника

int count = 0;							// Лічильник
pthread_mutex_t  		mutex;				// М'ютекс для доступу до лічильника та умовної змінної
pthread_cond_t 		condition_variable;		// Умовна змінна

//~ Функція потоків, які інкрементуватимуть лічильник - два потоки, кожний потік TCOUNT разів збільшує значення лічильника.
//~ Коли лічильник досягне COUNT_LIMIT, той потік, за роботи якого це відбулось, надішле сигнал через умовну змінну третьому потоку,
//~ який виконує функцію waiter
void *sender(void *t)
{ 
	int my_id =  * (int *) t;

	//~ Локальний лічильник - TCOUNT - кожний потік TCOUNT разів збільшує глобальний лічильник count
	for (int i = 0; i < TCOUNT; i++) {

		//~ Один потік закриває вхід для іншого, поки працює із глобальними змінними
		pthread_mutex_lock(&mutex);

		//~ Збільшуємо значення лічильника

		count++;
		printf("Потік %d змінив лічильник: %d\n", my_id, count);
		//~ Перевіряємо значення лічильника

		if (count == COUNT_LIMIT) {
			//~ Якщо вже досягли ліміту - сигналізуємо третьому потоку
			printf("Потік %d, який змінює лічильник, дійшов до ліміту (%d). Надсилаємо сигнал тому, що очікує.\n", my_id, count);
			pthread_cond_signal(&condition_variable);
			printf("Сигнал відправлено.\n");
		}
		//~ Відкриваємо вхід у критичну секцію іншому потоку.
		pthread_mutex_unlock(&mutex);
		//~ Для тестів сповільнимо роботу
		sleep(1);
	}
	pthread_exit(NULL);
}

void *waiter(void *t)
{
	int my_id =  * (int *) t;

	printf("Потік %d очікує значення лічильника: %d\n", my_id, COUNT_LIMIT);
	//~ Заблокуємо м'ютекс для перевірки умовної змінної
	pthread_mutex_lock(&mutex);
	//~ Якщо сигнал не поступив від потоків, що виконують функцію sender, pthread_cond_wait відправить потік,
	//~ що виконує цю функцію, спати, а м'ютекс розблокує
	pthread_cond_wait(&condition_variable, &mutex);
	//~ Якщо сигнал поступив, нам треба розблокувати м'ютекс, що блокує доступ до умовної змінної, і продовжити роботу потоку
	pthread_mutex_unlock(&mutex);
	//~ Обнулення лічильника
	count = 0;
	printf("Потік %d дочекався значення лічильника COUNT_LIMIT та обнулив його\n", my_id);

	pthread_exit(NULL);
}


int main() {
	
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
	//~ Два потоки виконують функцію sender, третій - waiter
	pthread_create(&threads[0], NULL, sender, (void *) &num[0]);
	pthread_create(&threads[1], NULL, sender, (void *) &num[1]);
	pthread_create(&threads[2], NULL, waiter, (void *) &num[2]);
	
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

