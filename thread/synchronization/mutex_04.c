
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define SIZE		256 

struct message {
	int id;
	char  buf[SIZE];
	pthread_mutex_t mutex;
};


//~ Функція для роботи в потоці
void *thread_function(void *args) {
	struct message * argument = ((struct message *) args);

	pthread_mutex_lock(&argument[0].mutex);
	sleep(1);
	memset(argument[0].buf, 48 + argument[0].id * 2 + 0, SIZE);
	printf("Потік %d зробив повідомлення 1:\n", argument[0].id);
	write(1, argument[0].buf, SIZE);
	printf("\n"); 
	pthread_mutex_unlock(&argument[0].mutex);


	pthread_mutex_lock(&argument[1].mutex);
	sleep(1);
	memset(argument[1].buf, 48 + argument[1].id * 2 + 1, SIZE);
	printf("Потік %d зробив повідомлення 2:\n", argument[1].id);
	write(1, argument[1].buf, SIZE);
	printf("\n"); 
	pthread_mutex_unlock(&argument[1].mutex);

	
}

int main() {
	pthread_t threads[2]; 

	struct message args[2];

	args[0].id = 0;
	pthread_mutex_init(&args[0].mutex, NULL);

	args[1].id = 1;
	pthread_mutex_init(&args[1].mutex, NULL);

	pthread_create(&threads[0], NULL, thread_function, &args);
	pthread_create(&threads[0], NULL, thread_function, &args);

	
	//~ Очікування завершення потоків
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL); 

	return 0;
}

