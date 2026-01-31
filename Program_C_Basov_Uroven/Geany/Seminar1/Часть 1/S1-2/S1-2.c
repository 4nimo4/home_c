/*
 * S1-2.c
 * 
	Целой переменной k присвоить значение, равное первой цифре 
	дробной части в записи вещественного положительного числа x..
 * 
 * 
 */


#include <stdio.h>

int main(void)
{
	float f=123.567;
	int k, fint;
	fint = f; 	// fint = 123
	fint *= 10; // fint = 1230
	f *=10;		// f = 1235.67
	k = f - fint; // k=5
	printf("%d", k);	
	return 0;
}

