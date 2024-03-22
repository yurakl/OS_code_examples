// Приклад конвеєра, що працює із використанням стандартних потоків виводу (stdout) та вводу (stdin)
// Програма зчитує рядок із стандртного потоку вводу, та обчислює його довжину
// Використання echo "hello" | 1.c
#include <stdio.h>
#include <string.h>
int main()
{
	// Буфер для запису інформації із stdin 
	char str[1000];
	// Зчитування із stdin 
	scanf("%s", str);
	int len = strlen(str);
	printf("Len: %d\n", len);

	return 0;
}
