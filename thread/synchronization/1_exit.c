//~ Для виходу із потоків використовується два підходи:
//~ return NULL або pthread_exit(NULL);
//~ Якщо необхідно повернути якусь величину, то на місці NULL має бути адреса змінної або вказівник на змінну

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//~ Функція для роботи в потоці
void *thread_func(void *thread_id) {

	int tid = * (int *) thread_id;

	//~ Зробимо розгалуження:
	//~ один потік вийде через pthread_exit(NULL), інший через return NULL,
	//~ третій дійде до кінця функції потоку
	if (tid == 1) {
		sleep(1);
		printf("Потік %d завершує роботу всередині функції потоку, використовуючи pthread_exit(NULL)\n", tid);
		pthread_exit(NULL);
	} else if (tid == 2) {
		sleep(1);
		printf("Потік %d завершує роботу всередині функції потоку, використовуючи return NULL\n", tid);
		return NULL;
	}
	sleep(1);
	printf("Потік %d дійшов до кінця функції потоку. Вихід через return NULL\n", tid);
	return NULL;
}

int main() {
	
	//~ Отримуємо ID процесу та друкуємо його в термінал
	pid_t init_pid = getpid();
	printf("ID процесу: %d\n", init_pid);
	pthread_t threads[3];
	//~ Оголосимо деякі змінні типу int, ядреси яких передамо в функцію потку як аргумент
	int num[3] = {1, 2, 3};
	
	//~ Створюємо два потоки та передамо їм адреси змінної
	pthread_create(&threads[0], NULL, thread_func, (void *) &num[0]);
	pthread_create(&threads[1], NULL, thread_func, (void *) &num[1]);
	pthread_create(&threads[2], NULL, thread_func, (void *) &num[2]);
	
	//~ Чекаємо на завершення потоків.
	//~ Тут NULL - ми не чекаємо, що функція потоку нам щось поверне
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
	return 0; 
}
