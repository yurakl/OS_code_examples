#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> 
#include <math.h> 
//~ Кількість спроб
#define SIZE 	1000000
#define PI 		3.14159265359

//~ Глобальний лічильник спроб
int counter	= 0;
//~ Глобальний лічильник вдалих спроб
int success = 0;


int main() {

	//~ Величина для генератора випадкових чисел - абсолютний часу в секундах
	int seed = (int) time(NULL);
	//~ Радіус кулі
	const float radius =	1.0;
	//~ Діапазон для генерування координат
	float xmin = 	0.0;
	float xmax = 	radius;

	float ymin = 	0.0;
	float ymax = 	radius;

	float zmin = 	0.0;
	float zmax = 	radius;
	
	//~ Цикл по лічильнику - кількість спроб
	while (counter < SIZE) { 
		//~ генеруємо випадкові координати
		float rand_x = xmin + ((float) rand_r(&seed) / RAND_MAX) * (xmax - xmin);
		float rand_y = ymin + ((float) rand_r(&seed) / RAND_MAX) * (ymax - ymin);
		float rand_z = ymin + ((float) rand_r(&seed) / RAND_MAX) * (zmax - zmin);

		//~ Знаходимо відстань до точки від початку координат
		float length = pow(pow(rand_x, 2.0) + pow(rand_y, 2.0) + pow(rand_z, 2.0), 0.5);
		
		//~ Відстань до точки від початку координат: відстань =  sqrt(x^2 +y^2 + z^2)
		//~ Ми генеруємо точку з випадковими координатами, знаходимо відстань від початку координат до точки
		//~ та порівнюємо із радіусом сфери - чи точка в середині сфери
		
		//~ Координата всередині сфері? 
		if (length < radius) {
			//~ Успішна спроба
			success++;
		}
		//~ Збільшуємо лічильник спроб
		counter++;
	}
	 
	//~ Точний об'єм кулі
	float	volume_true = 4.0 / 3.0 * PI * pow(radius, 3.0);
	//~ Об'єм куба в якому генерувались координати
	float cube_volume = (xmax - xmin) * (ymax - ymin) * (zmax - zmin);
	//~ Знаходимо об'єм сектора кулі, що є частиною об'єму куба
	//~ Об'єм сфери значходиться із пропорції: об'єм куба - це всі точки, об'єм сфери - це успішні спроби

	float volume_approx = cube_volume * ((float) success /  counter);
	
	//~ Друкуємо результати
	printf("Кількість спроб: %d,\t Успішних спроб: %d\n", counter, success);
	printf("Точний об'єм: %f,\t Розрахований об'єм: %f\n", volume_true, 8.0 * volume_approx);

	// Домножуємо на 8, оскільки ми досліджували одну квартиль і отримали об'єм одного із 8 секторів кулі.
	
	return 0;
}
