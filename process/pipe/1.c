// Приклад програми, що зчитує рядок із стандртного потоку вводу, та обчислює його довжину
// Використання eco "hello" | 1.c
#include <stdio.h>
#include <string.h>
int main()
{
	char str[1000];
	scanf("%s", str);
	int len = strlen(str);
	printf("Len: %d\n", len);

	return 0;
}
