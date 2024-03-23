//~ Для кожного потоку створюється свій стек - по замовчуванню 8 МБ
//~ Ця область пам'яті виділяється у адресному просторі процесу
//~ В даному прикладі ми перевіримо, чи є доступ із одного потоку до стеку іншого потоку

//~ Доступ до стеку іншого потоку можливий - можемо редагувати дані.
//~ Однак такий підхід не рекомендований, оскільки стек змінюється із ходом програми.
//~ А робота потоків не є повністю синхронізованою
//~ Як результат - ми можемо зчитати вже перезаписані дані, або перезаписати дані, що не відповідають тим, які ми очікували переписати.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//~ Оголосимо вказівник на дані
//~ Одни потік оголосить змінну - вона де розташована на його стеці
//~ Цей потік присвоїть значення цьому вказівнику - адреса змінної
//~ Наступний потік спробує змінити змінну за цим вказівником
int * p;

void *thread_func(void *thread_id) {
	int tid = * (int *) thread_id;
	
	if (tid == 1) {
		//~ Цей блок виконає перший потік
		//~ В одному потоці оголосимо змінну та присвоїмо їй якесь значення
		int a = 22;
		//~ Можна поставити потік на паузу та знайти положення змінної в адресному просторі процесу (pmap PID)
		printf("Потік: %d\tАдреса змінної на стеці та її значення: %p %d\n", tid, &a, a);
		//~ Присвоїмо значення вказівнику
		p = &a;
		//~ Поставимо на паузу, щоб наступний потік встиг відредагувати цю змінну
		//~ Можна видалити час очікування тут, тоді при зчитуванні із наступного потоку значення за вказівником,
		//~ тобто спроба отримати значення a, ми побачимо, що отримаємо іншу величину, не 22
		//~ Це пов'язано із тим, що даний потік завершив роботу, дані на його стеці в цьому місці були перезаписані
		sleep(3);
	} else {
		//~ Цей блок виконає другий потік
		//~ Якщо ми тут дамо більше значення очікування ніж в блоці для першого потоку,
		//~ ми в цьому потоці не отримаємо очікуване значення змінної із першого потоку
		sleep(1);
		//~ В другому потоці пробуємо читати змінну за вказівником та редагувати її
		printf("Потік: %d\tСпроба читання зі стеку іншого потоку: %p %d\n", tid, p, * p);
		* p = 33;
		printf("Потік: %d\tСпроба запису у стек іншого потоку: %p %d\n", tid, p, * p);
	}
	pthread_exit(NULL);
}

int main() {
	//~ Отримуємо ID процесу та друкуємо його в термінал
	pid_t init_pid = getpid();
	printf("ID процесу: %d\n", init_pid);
	pthread_t threads[2];
	//~ Оголосимо деякі змінні типу int, ядреси яких передамо в функцію потку як аргумент
	int num[2] = {1,2};
	
	//~ Створюємо два потоки та передамо їм адреси змінної
	pthread_create(&threads[0], NULL, thread_func, (void *) &num[0]);
	pthread_create(&threads[1], NULL, thread_func, (void *) &num[1]);
	
	//~ Чекаємо на завершення потоків.
	//~ Тут NULL - ми не чекаємо, що функція потоку нам щось поверне
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	return 0; 
}