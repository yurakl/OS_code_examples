#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device/class.h>
#include <linux/fs.h>

#define MODULE_NAME 	"SKM_4"
#define DEVICE_NAME 	"SKM_4"
#define DEVICE_CLASS 	"SKM_4"
#define PAGE		4096


#define SIZE		100
static char kernel_buffer[SIZE]; 

struct task_struct *the_process;

struct class 		* skm_class; 
//~ static struct device	* skm_dev;
static int  		  majorNumber;

//~ Прототипи функцій для файлових операцій
static int		open_skm(struct inode *, struct file *);
static ssize_t	write_skm(struct file *, const char __user *, size_t, loff_t *);
static ssize_t	read_skm(struct file *, char __user *, size_t, loff_t *);
static int		release_skm(struct inode *, struct file *);

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
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME, "Open Callback", __FUNCTION__, __LINE__);
	return 0;
}

//~ Функція, що буде опрацьовувати системний виклик write для цього файлу
static ssize_t write_skm(struct file * this_file, const char __user * user_buf, size_t count, loff_t * fpos)
{
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME, "Write Callback", __FUNCTION__, __LINE__);

	return -EINVAL;
}

//~ Функція, що буде опрацьовувати системний виклик read для цього файлу
static ssize_t read_skm(struct file * this_file, char __user * user_buf, size_t count, loff_t * fpos)
{ 
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME, "Read Callback", __FUNCTION__, __LINE__);
 
	return 0;
}

//~ Функція, що буде опрацьовувати системний виклик close для цього файлу
static int release_skm(struct inode *this_inode, struct file *this_file)
{
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME, "Release Callback", __FUNCTION__, __LINE__); 
	return 0;
}  

//~ Функція, що виконується при завантаженні модуля ядра в операційну систему
static int init_function(void) {
	
	pr_info("%s: %s - %s: %d\n", DEVICE_NAME, "INIT", __FUNCTION__, __LINE__);
	//~ Те ж саме, що в попередньому модулі
	majorNumber = register_chrdev(0, DEVICE_NAME, &skm_fops);
	skm_class = class_create(THIS_MODULE, DEVICE_CLASS);
	device_create(skm_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

	
	rcu_read_lock();
	//~ Читаємо список процесів
	for_each_process(the_process) {
		//~ Функція for_each_process в кожній ітерації повертає вказівник the_process на новий процес.

		//~ Зчитуємо та виводимо у журнал повідомлень
		//~ PID процесу,
		//~ його стан,
		//~ пріоритет виконання,
		//~ розмір вірутальної пам'яті процесу
		//~ Фактичний розмір використаної фізичної пам'яті (деякі об'єкти можуть бути залучені у пам'ять кожного процесу
		//~ - наприклад, динамічно приєднувані біліотеки)
		//~ В структурі mm залучена пам'ять показана у сторінках, тому переводимо у кілобайти ( * PAGE / 1024)
		pr_info("PID:\t%d,\tState:\t%d,\tPrio:\t%d,\tVM Size:\t%lu,\tRSS:\t%lu\n",
				the_process->pid,
				the_process->__state,
				the_process->prio,
				the_process->mm == NULL ? 0 : the_process->mm->total_vm * PAGE / 1024, 
				the_process->mm == NULL ? 0 : the_process->mm->hiwater_rss * PAGE / 1024);
	}
	rcu_read_unlock();
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
