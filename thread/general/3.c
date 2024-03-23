//~ Ще приклад роботи із потоками, а саме передача аргументів та отримання результатів
//~ В даному прикладі ми створимо дві структури через динамічне виділення пам'яті.
//~ Одне поле структури - вказівник на масив.
//~ Інше поле структури - результат, в даному випадку сума елементів.
//~ Після обробки даних (масив, асоційований із кожною структурою) у потоці, ми зчитаємо результат

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//~ Кількість елементів масиву
#define SIZE 48
 
//~ Структура - наш тип даних
//~ Одне поле для id
//~ наступне - вказівник на масив
//~ третє - для запису результату - сума елементів масиву
struct my_type {
	int		id;
	int		*array;
	int		sum;
};

void * thread_func(void *arg) {
	//~ Приведення типу аргументу
	struct my_type * object = (struct my_type *) arg;
	//~ Присвоїмо елементам масиву певне значення та підрахуємо суму елементів
	for (int i = 0; i < SIZE; i++) {
		object->array[i] = 2 * object->id;
		object->sum += object->array[i];
	}
	return NULL;
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

	//~ Чекаємо на завершення потоків.
	//~ Тут NULL - ми не чекаємо, що функція потоку нам щось поверне
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	//~ Потоки завершились, виведемо суму елементів масивів, що асоційовані із кожною структурою
	printf("Потоки завершені - структури оброблені:\n");
	printf("Сума елементів масиву 1: %d\n", object[0].sum);
	printf("Сума елементів масиву 2: %d\n", object[1].sum);
	return 0;
}
