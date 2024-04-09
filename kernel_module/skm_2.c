#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device/class.h>
#include <linux/fs.h>

//~ Назва модуля та символьного пристрою
#define MODULE_NAME 	"SKM_2"
#define DEVICE_NAME 	"SKM_2"
#define DEVICE_CLASS 	"SKM_2"

//~ Службова інформація для модуля
struct class 		* skm_class; 
//~ static struct device	* skm_dev;

//~ Major Number - ціле число для типу пристрою, кожний тип модуля має свій ідентифікатор
static int  		  majorNumber;


//~ Розмір буфера для файлових операцій.
#define SIZE		256
//~ Буфер для запису 
char kernel_buffer[SIZE];

//~ Прототипи функцій для файлових операцій
static int		open_skm(struct inode *, struct file *);
static ssize_t	write_skm(struct file *, const char __user *, size_t, loff_t *);
static ssize_t	read_skm(struct file *, char __user *, size_t, loff_t *);
static int		release_skm(struct inode *, struct file *);

//~ Прапорець для позначення, що буфер вже зчитувався
int is_read = 0;

//~ Визначаємо функції для файлових операцій
static const struct file_operations skm_fops = {
	.owner 	= THIS_MODULE,
	.open 	= open_skm,
	.write	= write_skm,
	.read		= read_skm,
	.release	= release_skm,
};

//~ Функція, що буде опрацьовувати системний виклик open для цього файлу
static int open_skm(struct inode * this_inode, struct file * this_file)
{
	//~ Виводимо просте повідомлення, коли відкриваємо файл.
	//~ __FUNCTION__  __LINE__ макроси для функції та рядка коду - вставиться повідомлення
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME, "Open Callback", __FUNCTION__, __LINE__);
	return 0;
}

//~ Функція, що буде опрацьовувати системний виклик write для цього файлу
static ssize_t write_skm(struct file * this_file, const char __user * user_buffer, size_t count, loff_t * fpos)
{
	//~ Повідомлення про те, що був викликаний системний виклик write для цього файлу
	pr_info("%s: %s - %s: %lu count: %d\n", DEVICE_NAME, "Write Callback", __FUNCTION__, __LINE__, count);
	//~ Якщо хочемо записати більше, ніж розмір буфера, повертаємо помилку
	if(count > SIZE) {
		return -ENOMEM;
	} else {
		//~ Якщо хочемо записати менше, ніж розмір буфера - записуємо у буфер та повертаємо кількість байтів, які записали
		copy_from_user(kernel_buffer, user_buffer, count);
	}
	return -EPERM;
}

//~ Функція, що буде опрацьовувати системний виклик read для цього файлу
static ssize_t read_skm(struct file * this_file, char __user * user_buffer, size_t count, loff_t * fpos)
{ 
	//~ Повідомлення про те, що був викликаний системний виклик read для цього файлу
	pr_info("%s: %s - %s: %d, count: %lu\n", DEVICE_NAME, "Read Callback", __FUNCTION__, __LINE__, count); 
	//~ Якщо буфер ще не зчитувався, то записуємо у буфер користувача ввесь буфер із ядра 
	if (!is_read) {
		copy_to_user(user_buffer, kernel_buffer, SIZE);
		Тут прапорець доданий для наступного:
		//~ Користувач може виділитивеликий буфер для зчитування, наприклад команда cat  виділяє 12 КБ для читання із файлу.
		//~ Даний обробник виклику буде виконуватись циклічно, поки в сумі не буде скопійоввано 12 КБ, або не повернено кінець файлу (0).
		//~ Оскільки ми копіюємо користувачу ввесь буфер за раз, нам достатньо прапорця на проведення зчитування, без відстеження позиції у буфері
		//~ Прапорець, що буфер зчитали
		is_read++;
		//~ повертаємо кількість байтів, яку ми передали користувачу
		return SIZE;
	} else {
		//~ Якщо буфер вже читався у одному відкритті файлу, то повертаємо 0 - кінець файлу
		//~ 0 = end of file;
		return 0;
	}
	return -EPERM;
}

//~ Функція, що буде опрацьовувати системний виклик close для цього файлу
static int release_skm(struct inode *this_inode, struct file *this_file)
{
	//~ Закриваємо файл, і ставимо прапорець зчитуванняв 0
	is_read = 0;
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME, "Release Callback", __FUNCTION__, __LINE__); 
	return 0;
}  

//~ Функція, що виконується при завантаженні модуля ядра в операційну систему
static int init_function(void) {
	
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME, "INIT", __FUNCTION__, __LINE__);
	//~ Просимо систему виділити унікальний ідентифікатор для нашого модуля
	majorNumber = register_chrdev(0, DEVICE_NAME, &skm_fops);
	//~ Створюємо клас пристрою
	skm_class = class_create(THIS_MODULE, DEVICE_CLASS);
	//~ Створюємо файл пристрою в системі /dev
	device_create(skm_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	
	return 0;
	
}
//~ Функція, що виконується при вивантажені модуля ядра в операційну систему
static void exit_function(void) {

	//~ Видалення інформації про модуль у операційній системі
	device_destroy(skm_class, MKDEV(majorNumber, 0));		// remove the device
	class_unregister(skm_class);						// unregister the device class
	class_destroy(skm_class);						// remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);			// unregister the major number
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME,  "EXIT", __FUNCTION__, __LINE__);

}


module_init(init_function);
module_exit(exit_function); 
MODULE_AUTHOR("YK");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simle kernel module.");
