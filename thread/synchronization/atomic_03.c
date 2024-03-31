//~ Програма перевірки атомарності структур
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdatomic.h> 

//~ Структура розміром 8 байтів - розмір int = 4 байти
typedef _Atomic struct {
	int a[2]; 
} type_1;

//~ Структура розміром 12 байтів - розмір int = 4 байти
typedef _Atomic struct {
	int a[3];
} type_2;

//~ Структура розміром 16 байтів - розмір int = 4 байти
typedef _Atomic struct {
	int a[4];
} type_3;

//~ Структура розміром 32 байти -  розмір int = 4 байти
typedef _Atomic struct {
	double a[8];
} type_4;

typedef struct {
	_Atomic int a[4];  
} type_5;

//~ Створимо об'єкти даних типів
type_1 type_1_object_1 = {
	.a = {0, 1}, 
};

type_1 type_1_object_2 = {
	.a = {2, 3}, 
};

type_2 type_2_object_1 = {
	.a = {0, 1, 2},
};
type_2 type_2_object_2 = {
	.a = {3, 4, 4},
};

type_3 type_3_object = {
	.a = {0},
};
type_4 type_4_object = {
	.a = {0},
};
type_5 type_5_object = {
	.a = {0},
};
int main() {

	

	printf("Розмір type_1: %ld, Тип атомарний?: %s\n", sizeof(type_1), atomic_is_lock_free(&type_1_object_1) ? "Так" : "Ні");
	printf("Розмір type_2: %ld, Тип атомарний?: %s\n", sizeof(type_2), atomic_is_lock_free(&type_2_object_1) ? "Так" : "Ні"); 
	printf("Розмір type_3: %ld, Тип атомарний?: %s\n", sizeof(type_3), atomic_is_lock_free(&type_3_object) ? "Так" : "Ні");
	printf("Розмір type_4: %ld, Тип атомарний?: %s\n", sizeof(type_4), atomic_is_lock_free(&type_4_object) ? "Так" : "Ні");
	printf("Розмір type_4: %ld, Тип атомарний?: %s\n", sizeof(type_5), atomic_is_lock_free(&type_5_object.a[1]) ? "Так" : "Ні");

	atomic_exchange(&type_1_object_1, type_1_object_2);
	atomic_exchange(&type_2_object_1, type_2_object_2);
	return 0;
}

