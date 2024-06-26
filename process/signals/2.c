//~ Ще один приклад використання сигналів.
//~ Створюємо два процеси шляхом дублювання (fork).
//~ Дочірній процес надсилає сигнал SIGUSR1 батьківському.
//~ Обробник даного сигналу інкрементує лічильник при кожному отриманому сигналі
//~ Батьківський процес засинає на 10 секунд, а потім вбиває дочірній процес та виводить остаточний вміст лічильника.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

//~ Лічильник
//~ Цей лічильник буде змінювати значення тільки в батьківському процесі, оскільки дочірній процес отримує власну копію ресурсів.
int counter = 0;

//~ Обробник сигналу SIGUSR1 - виводимо повідомлення про отриманий сигнал та інкрементуємо лічильник;
void sigusr_handler(int signum) {
    printf("Resived SIGUSR1\n");
    counter++;
}
 

int main() {

	//~ Створення нового процесу
	pid_t c_pid = fork();
	if( c_pid == 0 ){
		//~ Дочірній процес
		//~ Отримуємо PID батьківського процесу - ppid - PARENT PROCESS ID
		pid_t pp = getppid();

		//~ Надсилаємо сигнал батьківському процесу, поки цей (дочірній) не буде вбитий
		while(1) {
			kill(pp, SIGUSR1);
			sleep(1);
			//~ Наступний вивід можна додати, щоб переконатись, що значення лічильника не змінюється в дочірньому процесі,
			//~ оскільки він отримав власну копію ресурсів батьківського процесу
			//~ printf("Дочірній: Значення лічильника: %d\n", counter); 
		}

	} else if (c_pid > 0){

		//~ Батьківський процес
		//~ Встановлюємо реакцію на сигнал
		signal(SIGUSR1, sigusr_handler); 
		//~ Чекаємо, поки не обробимо 10 сигналів. Батьківський процес буде отримувати сигнал від дочірнього
		while(counter < 10);
		
		//~ Вбиваємо дочірній процес
		kill(c_pid, SIGKILL);
		//~ Виводимо остаточне значення лічильника
		printf("Значення лічильника: %d\n", counter); 

	} else {
		//~ Помилка створення процесу
		perror("fork failed");
		_exit(2);
	}
	return 0;
}
