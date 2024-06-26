//~ Приклад для перевірки перезаписування в конвеєрі
//~ Стандартний розмір конвеєра 64КБ, якщо запис доходить до кінця файлу - почнеться запис з початку
//~ Тобто будуть перезписуватись дані конвеєра.
//~ Однак, на наступному колі, запис не може обігнати зчитування.

//~ Припустимо, що fd_1 використовується для запису, fd_2 - для читання.

								//~ fd_1 > pos \/ 
	//~ Конвеєр (файл)  |.......................................|
				//~ fd_2 > pos  /\

	//~ Записування в конвеєр продовжується заново із початку конвеєру (файлу)

			//~ fd_1 > pos \/ 
	//~ Конвеєр (файл)  |.......................................|
				//~ fd_2 > pos  /\

	//~ Якщо зчитування не відбувається, і запис наздогнав читання на наступному колі:

				 //~ fd_1 > pos \/ 
	//~ Конвеєр (файл)  |.......................................|
				//~ fd_2 > pos  /\

	//~ То записування буде призупинено, поки не просунеться зчитування вперед - дані які не зчитались не можуть бути перезаписані


//~ В даній програмі ми запишемо кілька порцій інформації, сумарно 64 КБ, у конвеєр. Тобто заповнимо конвеєр повністю...
//~ Зчитування не будемо виконувати - затримаємо на певний час, щоб заповнити конвеєр повністю.
//~ Після заповнення конвеєру спробуємо записати ще порцію даних.
//~ Однак, оскільки зчитування не відбулося, ми не можемо на другому колі обігнати позицію для зчитування.
//~ Як результат, процес, що хоче дописати дані у конвеєр буде відправлений у сон,
//~ доки не просунеться позиція для зчитування (поки не зчитаємо порцію даних з конвеєера).
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

//~ Величини для маніпаляцій з масивами
//~ Кілобайт
#define KBYTE	1024
//~ Сторінка = 4 КБ
#define PAGE	(4 * KBYTE)

int main(int argc, char * argv[]){


	int 	pfd[2]; 	//	pfd[0] для читання
				//	pfd[1] для запису
  
	//~ Створення конвеєру
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
		//~ Виділимо буфер для читання із конвеєру розміром 4 КБ.  В батьківському процесі запис буде йти по 1 КБ
		//~ Вивід покаже, зчитається тільки 2 КБ, а не 4, як розмір буфера - це обмеження системи.
		char buffer[PAGE];

		//~ Лічильник для кількості зчитувань з конвеєра
		int i = 1;
		//~ Змінна для зберігання кількості зчитаних байтів - те, що повертає функція read
		ssize_t  read_size;
		//~ Почекаємо доки заповниться конвеєр
		sleep(15);
		//~ Якщо зчитування захоче випередити запис, то процес, що читає, буде відправлений спати до появи нових даних в конвеєрі
		while((read_size = read(pfd[0], buffer, 2 * KBYTE)) > 0) {
			//~ Видрукуємо  номер повідомлення, та скільки було зчитано байтів
			printf("Зчитано з конвеєра порцію даних №%d: Розмір: %ld\n", i, read_size);
			//~ sleep(1);
			//~ Розкоментуйте наступні рядки, якщо хочете побачити вміст повідомлень
			//~ write(1, buffer, read_size);
			//~ putc('\n', stdout);
			i++;
		}

		//~ Виведемо повідомлення, коли закінчиться зчитування - "зчитається" кінець файлу - функція read поверне 0
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
		//~ Буфер обсягом 17 сторінок = 17 * 4 КБ = 68 КБ:
		//~ перші 4 сторінки (16 КБ) заповнені літерою 'a', другі 4 сторінки - 'b',
		//~ треті 4 сторінки - 'c', четверті 4 сторінки - 'd', остання, 17-та, сторінка заповнена літерою 'e'
		char buffer[17 * PAGE] = {	[0 * PAGE ... 4 * PAGE - 1] = 'a',
							[4 * PAGE ... 8 * PAGE - 1] = 'b',
							[8 * PAGE ... 12 * PAGE - 1] = 'c',
							[12 * PAGE ... 16 * PAGE - 1] = 'd',
							[16 * PAGE ... 17 * PAGE - 1] = 'e'};

		//~ Оголисимо лічильник, що підраховує кількість записаних даних
		unsigned int counter = 0;
		//~ Будемо робити запис по 4 сторнки, тобто буде записаний певний підмасив... в нашому випадку заповнений однією літерою
		//~ Між кожним записом робимо павзу
		//~ sleep(3);
		//~ Записуємо перші 4 сторінки, 16 КБ, заповнені літерою 'a'
		write(pfd[1], buffer + 0 * PAGE, 4 * PAGE);
		counter += 4 * PAGE;
		printf("Запис у конвеєр №1.\tСумарно записано: %d\n", counter);
		//~ чекаємо кілька секунд
		sleep(1);
		//~ Записуємо другу порцію (4 сторінки), заповнені літерою 'b'
		write(pfd[1], buffer + 4 * PAGE, 4 * PAGE);
		counter += 4 * PAGE;
		printf("Запис у конвеєр №2.\tСумарно записано: %d\n", counter);
		sleep(1);
		//~ Записуємо третю порцію (4 сторінки)
		write(pfd[1], buffer + 8 * PAGE, 4 * PAGE);
		counter += 4 * PAGE;
		printf("Запис у конвеєр №3.\tСумарно записано: %d\n", counter);
		sleep(1);
		//~ Записуємо четверту порцію (4 сторінки)
		write(pfd[1], buffer + 12 * PAGE, 4 * PAGE);
		counter += 4 * PAGE;
		printf("Запис у конвеєр №4.\tСумарно записано: %d\n", counter);
		//~ 16 сторінок (64 КБ) було записано - тепер конвеєр повністю заповнений. Зчитування ще не було
		//~ Запис останньої сторінки не буде виконано, доки не просунеться зчитування (там ми поставили павзу = 15 секунд)
		//~ Має зчитатись хоча б 1 сторінка - позиція зчитування просунеться вперед на 4 КБ, і ми зможемо записати ще 4 КБ.

		printf("... Тепер конвеєр повністю заповнений\n");

		write(pfd[1], buffer + 16 * PAGE, PAGE);
		printf("Запис у конвеєр: 5 ... Було проведено зчитування, можемо переписати початок файлу\n");
		//~ Після запису всього буферу у конвеєр очікуємо ще певний час
		sleep(5);
	 
		close(pfd[1]);
		puts("Батьківський процес: закрив конвеєр");
		//~ Чекаємо на завершення дочірнього процесу
		wait(NULL);

	} else {
		/* ERROR */
		perror("fork");
		return 1;
	}

	return 0;
}
