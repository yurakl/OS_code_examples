//~ Вихід із використанням exit() - вб'є процес і всі потоки. Така ж поведінка буде і при використанні _exit()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//~ Функція для роботи в потоці
void *thread_func(void *thread_id) {

	int tid = * (int *) thread_id;

	//~ Зробимо розгалуження:
	//~ один потік вийде через exit(0),
	//~ інші поставимо на паузу, вони вже не вийдуть із  потоку звичним шляхом - exit() вб'є всіх
	
	if (tid == 1) {
		sleep(1);
		printf("Потік %d завершує роботу всередині функції потоку, використовуючи exit()\n", tid);
		exit(0);
	} else if (tid == 2) {
		sleep(3);
		printf("Потік %d завершує роботу всередині функції потоку, використовуючи return NULL\n", tid);
		return NULL;
	}
	sleep(3);
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
