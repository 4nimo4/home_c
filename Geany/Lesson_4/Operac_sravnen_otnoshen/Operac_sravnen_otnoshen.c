/*
 * Operac_sravnen_otnoshen.c
	Operacii_sravneniya_otnosheniya
 * 
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{
	int a, b;
	a = 5 == 3; // a=0 - ложь
	b = 100 >= 1; // b=1
	printf("a=%d b=%d\n", a, b);
	printf("a==b %d\n", a==b);
	printf("a!=b %d\n", a!=b);
		
	return 0;
}

