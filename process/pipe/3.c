//~ Ще один приклад використання конвеєру
//~ Конвеєр - це файл, який присутній у оперативній пам'яті, і який два процеси використовують для комунікації
//~ Конвеєр описується двома файловими дескрипторами.
//~ Один файловий дескриптор використовується для запису, інший - для читання
//~ Кожний файловий дескриптор зв'язаний із певним значенням - позиція у файлі.
//~ Припустимо, що fd_1 використовується для запису, fd_2 - для читання.

					//~ fd_1 > pos \/ 
	//~ Конвеєр (файл)  |.......................................|
				//~ fd_2 > pos  /\

//~ Якщо читання наздожене запис ( fd_2 > pos == fd_1 > pos ) та процес, що читає, захоче ще зчитати з конвеєру - він буде відправлений у сон,
//~ доки не надійдуть нові дані в конвеєр - поки не просунеться fd_1 > pos  вперед


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


#define KBYTE	1024

int main(int argc, char * argv[]){


	int 	pfd[2]; 	//	pfd[0] для читання
				//	pfd[1] для запису
  
	// Створення конвеєру
	if ( pipe(pfd) < 0){
		perror("Створення конвеєру");
		return 1;
	}

	//~ Створення нового процесу шляхом дублювання поточного
	pid_t cpid = fork();

	if( cpid == 0 ){
		//~ Дочірній процес
		//~ Закриваємо кінець конвеєра для запису - тут ми тільки зчитуємо
		//~ кінець конвеєра для запису - файловий дескриптор, що використовується для запису
		close(pfd[1]);
		//~ Виділимо буфер для читання із конвеєру розміром 2 КБ.  В батьківському процесі запис буде йти по 1 КБ
		char buffer[2 * KBYTE];

		//~ Лічильник для кількості зчитувань з конвеєра
		int i = 1;
		//~ Змінна для зберігання кількості зчитаних байтів - те, що повертає функція read
		ssize_t  read_size;


		//~ Якщо зчитування захоче випередити запис, то процес, що читає, буде відправлений спати до появи нових даних в конвеєрі
		
		while((read_size = read(pfd[0], buffer, 2 * KBYTE)) > 0) {
			//~ Видрукуємо  номер повідомлення, та скільки було зчитано байтів
			printf("Повідомлення №%d: Розмір: %ld\n", i, read_size);

			//~ Тут ми виводимо вміст буферу - те, що зчитали із конвеєра.
			//~ В даному випадку в нас буфер для читання  = 2КБ, а запис в батькіському процесі йде по 1 КБ
			//~ Тому, зчитування в дочірньому процесі буде теж по 1 КБ. Тобто, read зчитає порцію нових даний (1КБ), а не чекатиме заповнення всього буфера (2 КБ)
			write(1, buffer, read_size);
			
			putc('\n', stdout);
			i++;
		}

		puts("Дочірні процес: зчитування з конвеєру завершено");
		
		close(pfd[0]);
		//~ Вихід з процесу
		_exit(0);
		
	}else if ( cpid > 0){
		//~ Батьківський процес
		//~ Закриємо кінець конвеєеру для читання - тут ми тільки записуємо
		//~ Кінець конвеєеру для читання - файловий дескриптор, який використовується для читання
		close(pfd[0]);

		//~ Зробимо деякий буфер у батьківському процесів, з якого будемо записувати дані в конвеєр
		//~ Буфер обсягом 5 КБ: перший КБ заповнений літерою 'a', другий - 'b', третій - 'c', четвертий - 'd', п'ятий - 'e'
		char buffer[5 * 1024] = {	[0 * KBYTE ... 1 * KBYTE - 1] = 'a',
							[1 * KBYTE ... 2 * KBYTE - 1] = 'b',
							[2 * KBYTE ... 3 * KBYTE - 1] = 'c',
							[3 * KBYTE ... 4 * KBYTE - 1] = 'd',
							[4 * KBYTE ... 5 * KBYTE - 1] = 'e'};

		//~ Будемо робити запис по 1 КБ, тобто буде записаний певний підмасив... в нашому випадку заповнений однією літерою
		//~ Між кожним записом робимо павзу, щоб побачити, чи буде чекати на нові дані процес, що зчитує
		sleep(3);
		//~ Записуємо перший КБ заповнений літерою 'a'
		write(pfd[1], buffer + 0 * KBYTE, KBYTE);
		//~ чекаємо кілька секунд
		sleep(3);
		//~ Записуємо другий КБ заповнений літерою 'b'
		write(pfd[1], buffer + 1 * KBYTE, KBYTE);
		sleep(3);
		write(pfd[1], buffer + 2 * KBYTE, KBYTE);
		sleep(3);
		write(pfd[1], buffer + 3 * KBYTE, KBYTE);
		sleep(3);
		write(pfd[1], buffer + 4 * KBYTE, KBYTE);
		//~ Після запису всього буферу у конвеєр очікуємо ще певний час
		sleep(10);
		
		close(pfd[1]);
		puts("Батьківський процес: закриває конвеєр"); 
		//~ Чекаємо на завершення дочірнього процесу
		wait(NULL);

	}else{
		/* ERROR */
		perror("fork");
		return 1;
	}

	return 0;
}
