/*
 * S1-1.c
 * 
	Целой переменной k присвоить значение, равное третьей от конца 
	цифре в записи целого положительного числа x.
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

