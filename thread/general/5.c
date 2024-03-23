//~ Приклад подібний до 3.c, однак тут спробуємо повернути значення із функції потоку


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//~ Кількість елементів масиву
#define SIZE 48
 
//~ Структура - наш тип даних
//~ Одне поле для id
//~ наступне - вказівник на масив
struct my_type {
	int		id;
	int		*array; 
};

//~ Функція для роботи в потоці - вона має повернути або вказівник (змінна, що містить адресу) на певну змінну, або її адресу
//~ ! Змінна не має бути на стеці потоку
void * thread_func(void *arg) {
	//~ Приведення типу аргументу
	struct my_type * object = (struct my_type *) arg;

	//~ Оголосимо локальну змінну, значення якої ми хочемо повернути із потоку
	//~ Із функції потоку ми можемо повернути адресу змінної
	//~ Якщо ми спробуємо повернути адресу локальної змінної потоку - ми отримаємо помилку
	int sum;
	
	//~ Для повернення значення змінної із потоку, її потрібно розмістити поза потоком
	//~ Закоментуйте рядки із sum, та розкоментуйте із psum - в такому варіанті програма працюватиме без помилок
	
	//~ int * psum = (int *) malloc(sizeof(int));
	
	//~ Присвоїмо елементам масиву певне значення та підрахуємо суму елементів
	for (int i = 0; i < SIZE; i++) {
		object->array[i] = 2 * object->id;
		sum += object->array[i];
		//~ *psum += object->array[i];
	}
	
	return &sum;
	//~ return psum;
}

int main() {
	//~ Отримуємо ID процесу та друкуємо його в термінал
	pid_t init_pid = getpid();
	printf("ID процесу: %d\n", init_pid);
	pthread_t threads[2];

	//~ Виділимо пам'ять під дві структури
	struct my_type *object =  (struct my_type *) malloc(sizeof(struct my_type) * 2);

	//~ Виділимо пам'ять під масив int, на який вказуватиме вказівник із структури
	object[0].array = (int *) malloc(SIZE * sizeof(int));
	object[1].array = (int *) malloc(SIZE * sizeof(int));
	//~ Присвоїмо id кожній структурі
	object[0].id = 1;
	object[1].id = 2;
 
	//~ Створюємо два потоки та передамо їм адреси структур
	pthread_create(&threads[0], NULL, thread_func, (void *) &object[0]);
	pthread_create(&threads[1], NULL, thread_func, (void *) &object[1]);

	int sum[2];
	int *p; 
	//~ Чекаємо на завершення потоків.
	//~ Тут NULL - ми не чекаємо, що функція потоку нам щось поверне
	pthread_join(threads[0], (void **) &p);
	sum[0] = *p;
	pthread_join(threads[1], (void **) &p);
	sum[1] = *p;
	//~ Потоки завершились, виведемо суму елементів масивів, що асоційовані із кожною структурою
	printf("Потоки завершені - структури оброблені:\n");
	printf("Сума елементів масиву 1: %d\n", sum[0]);
	printf("Сума елементів масиву 2: %d\n", sum[1]);
	return 0;
}
