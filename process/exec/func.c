// Програма func, яку завантажить системний виклик execv, викликаний з main, в адресний простір процесу

#include <stdio.h>
#include <unistd.h>

int main(int a, int b)
{
	// Отримуємо PID процесу - він має бути таким же як і в main
	pid_t pid = getpid();
	printf("We are in: %d\n", pid);
	// Пауза
	getc(stdin);
	return 0;
}
