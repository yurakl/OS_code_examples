//~ Елементарний модуль ядра Лінукс
//~ Виводить повідомлення в журнал операційної системи при завантаженні та вивантаженні
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h> 

//~ Назва модуля
#define MODULE_NAME "Simple Kernel Module"

//~ Функція, що виконується при завантаженні модуля в ядро операційної системи
static int init_function(void) {
	
	pr_info("%s: %s\n", MODULE_NAME, "Hello world!");
		
	return 0;
}

//~ Функція, що виконується при вивантаженні модуля із ядра операційної системи
static void exit_function(void) {

	pr_info("%s: %s\n", MODULE_NAME, "End fo the world!");

}

//~ Вказуємо на функції, які будуть виконуватись при завантаженні та вивантаженні модуля
module_init(init_function);
module_exit(exit_function);
//~ Власник та ліцензія на модуль, GPL - General Public License
MODULE_AUTHOR("YK");
MODULE_LICENSE("GPL");
