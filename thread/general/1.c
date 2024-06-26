//~ Приклад створення потоків
//~ В даній програмі ми створимо два потоки.
//~ В головній функції оголосимо змінні, які передамо у функцію потоку як аргументи.

//~ В даному прикладі ми передаємо як аргумент функції потоку АДРЕСУ змінних, оголошених у функції MAIN,
//~ тобто ці дані розміщені на стеці функції MAIN
//~ Оскільки, функція MAIN "працює" ввесь час проботи потоків та після їх завершення
//~ Потік може редагувати ці дані, не боячись, що стек функції MAIN буде "очищений" та перезаписаний іншими даними

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


//~ Глобальна змінна - видима для всіх потоків
int global_var = 3;

//~ Функія для роботи в потоці: потік починається із виконання даної функції, а по завершенню її виконання, потік знищується
void * thread_func(void *arg) {
	//~ Статичне приведення типів: вказівник void * не асоційований із жодним типом даних - просто містить адресу змінної, без прив'язки до її типу
	//~ Оскільки ми очікуємо, що вказівник буде типу int, тому вкажемо, що вказівник вказує на змінну типу int
	int *var = (int *) arg;
	//~ Наступна операція - розіменування вказівника.
	//~ Припустимо ми маємо вказівник на цілочисельний тип - int * var - var містить адресу змінної, а не значення змінної.
	//~ Нам потрібно змінити значення змінної, маючи лише вказівник (адресу змінної)
	//~ * pointer - розіменування вказівника - отримання значення змінної за її адресою
	//~ Тут ми просто помножимо отриманий аргумент на значення глобальної змінної
	*var *= global_var;
	//~ Виведемо адресу змінної та її значення: ту це значення вказівника та розіменована величина вказівника - отримали значення змінної за її адресою
	printf("Адреса та значення змінної після її зміни (var * global_var) в функції потоку: %p\t%d\n", var, *var);
	//~ Тут адреса змінної має бути така ж як і у функції MAIN: ми оголосили змінні у функції MAIN - вони розташовані на основному стеці програми (MAIN).
	//~ Ми передали як  аргументи адреси змінних, а не їх значення. Тому потік змінює не власні копії даних, а ті самі, створені в MAIN.

	//~ Пауза для перевірки:
	//~ Поки програма чекає, в іншому терміналі можна виконати наступну команду: ps -eLf
	//~ В списку процесів буде три, що відповідаються за цю програму - основний процес та два створені потоки.
	//~ Ці три процеси матимуть однаковий PID. Однак програма ps як значення PID показує TGID - Thread Group ID. Цей ID відпоідає ID головного процесу.
	//~ LWP - Light Weight Process - показує ID потоку.
	//~ PID та TGID - поля дескриптора процесу в систему LINUX - task_struct
	getc(stdin);
	//~ Завершуємо роботу потоку
	//~ В наступних прикладах дізнаємось, чому тут повертаємо NULL
	return NULL;
}

int main() {
	//~ Отримуємо ID процесу та друкуємо його в термінал
	pid_t init_pid = getpid();
	printf("ID процесу: %d\n", init_pid);
	//~ Ідентифікатори потоку - це не те саме, що ID. 
	pthread_t threads[2];

	//~ Оголосимо деякі змінні типу int, ядреси яких передамо в функцію потку як аргумент
	int num[2] = {1,2};

	//~ Виведемо адреси цих хмінних та їх значення
	//~ В функції потоки ми поставили точку очікування.
	//~ Доки програма очіку, можна вивести в окремому терміналі вміст адресного простору процесу: pmap PID  - PID ми виводимо в консоль на початку програми
	printf("Адреса та значення змінної num[0] в функції MAIN (Розміщена на стеці MAIN): %p\t%d\n", &num[0], num[0]);
	printf("Адреса та значення змінної num[1] в функції MAIN (Розміщена на стеці MAIN): %p\t%d\n", &num[1], num[1]);
	//~ Можна звірити адреси даних, які ми ми оголосили в MAIN  та подивитись у виводі pmap в якій області адресного простору процесу ці дані збережені

	//~ Створимо потоки з використанням pthread_create. Ця функція повертає 0, коли потоки успішно створені
	//~ Аргументи функції pthread_create:
	//~ threads[0] - адреса ідентифікатора потоку - сюди запишеться ідентифікаційний номер новоствореного потоку (НЕ PID)
	//~ NULL - на цьому місці могла би бути адреса струкутури, яка задає певні параметри потоків.
	//~ NULL - Значить, що ми використовуємо налаштування потоків по замовчуванню
	//~ thread_func - функція, яка буде працювати у потоці
	//~ (void *) &num[0] - &num[0] адреса змінної num[0]
	//~ (void *) - статичне приведення типів. Оскільки в Сі не має динамічного приведення, функція в потоці має отримати вказівник -
	//~ адреса пам'яті, що не зв'язана із жодним типом даних
	//~ У функції потоку ми назад приведемо тип змінної, на яку вказує вказівник
	pthread_create(&threads[0], NULL, thread_func, (void *) &num[0]);
	pthread_create(&threads[1], NULL, thread_func, (void *) &num[1]);

	//~ Чекаємо на завершення потоків.
	//~ Тут NULL - ми не чекаємо, що функція потоку нам щось поверне
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	//~ В кінці програми виведемо адреси та значення змінних, адреси яких ми передали як аргумент у потоки,
	//~ щоб пересвідчитись, що наші потоки працювали із даними оголошеними в MAIN, а не з власними копіями цих даних
	printf("Адреса та значення змінної num[0] в функції MAIN (Розміщена на стеці MAIN): %p\t%d\n", &num[0], num[0]);
	printf("Адреса та значення змінної num[1] в функції MAIN (Розміщена на стеці MAIN): %p\t%d\n", &num[1], num[1]);
	return 0;
}
