
// В даному прикладі початкове значення семафора рівне 0.
//~ Цей приклад використовує семафор, як механізм сигналізування потоків про дозвіл на продовження виконання.
//~ Потоки, що виконуюють thread_function_2 зустрічають sem_wait(&semaphore). Оскільки значення семафору = 0, то
//~ продовження роботи потоків заборонене - вони засинають.
//~ Потік, що виконує thread_function_1, зробивши певну паузу, починає будити потоки по одному, виконуючи sem_post(&semaphore) -
//~ тобто, збільшуючи значення семафору.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define NUM_THREADS 3
//~ Початкове значення семафора 
#define NTZ  0
//~ Об'єкт семафору - через нього відбувається контроль доступу до критичної секції коду.
//~ Цей об'єкт має бути глобальною змінною - всі потоки мають мати доступ до даного об'єкту
sem_t semaphore;

//~ Функція для роботи в потоці
void *thread_function_1(void *arg) {
	int id = *((int *) arg);
//~ Потік, що виконує thread_function_1, зробивши певну паузу, починає будити потоки по одному, виконуючи sem_post(&semaphore) -
//~ тобто, збільшуючи значення семафору.
	printf("Потік %d: через дві секунди перший потік.\n", id);
	sleep(2);
	sem_post(&semaphore);
	sleep(1);
	printf("Потік %d: через дві секунди другий потік.\n", id);
	sleep(2);
	sem_post(&semaphore);
		 
	pthread_exit(NULL);
}

//~ Функція для роботи в потоці
void *thread_function_2(void *arg) {
	int id = *((int *) arg); 
//~ Потоки, що виконуюють thread_function_2 зустрічають sem_wait(&semaphore). Оскільки значення семафору = 0, то
//~ продовження роботи потоків заборонене - вони засинають.
 	sem_wait(&semaphore);    
	printf("Потік %d: пробудився.\n", id); 

	pthread_exit(NULL);
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS]= {1, 2, 3};

	//~ Ініціалізація семафора 
	sem_init(&semaphore, 0, NTZ); // NTZ - початкове значення семафора

	pthread_create(&threads[0], NULL, thread_function_1, &thread_args[0]);
	pthread_create(&threads[1], NULL, thread_function_2, &thread_args[1]);
	pthread_create(&threads[2], NULL, thread_function_2, &thread_args[2]);
	
	//~ Очікування завершення потоків
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);

	//~ Знищення семафора
	sem_destroy(&semaphore);

	return 0;
}
