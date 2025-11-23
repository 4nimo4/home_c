/*
 * Neyavnoe_preveden_tipov_volatile.c
	Неявное приведение типов
 */


#include <stdio.h>

int main()
{	
	float f;
	f = 3/2; // целочисленное деление f=1.0
	printf("%f\n", f);
	f = 3./2; // вещественное деление f=1.5		
	printf("%f\n", f);
	int a = 7;
	float x;
	x = a / 4; // 1
	printf("%f\n", x);
	x = 4 / a; // 0
	printf("%f\n", x);
	x = (float)a / 4; // 1.75
	printf("%f\n", x);
	x = 1.*a / 4; // 1.75
	printf("%f\n", x);
	x = a / 4.; // 1.75
	printf("%f\n", x);
	
	
	unsigned int u = 50;
	int i = -500;
	int answer = i / u;
	printf("%d\n", answer); // answer = 85899335
	/* i будет приведен к беззнаковому типу и его значение
	 * будет равно 2^32-500 вместо -500, а результат деления
	 * будет приведен обратно к знаковому типу int
	*/
	// Чтобы решить проблему приводим явно unsigned int u к int
	answer = i / (int)u;
	printf("%d\n", answer); // answer = -10
	return 0;
}


