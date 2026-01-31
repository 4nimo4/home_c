/*
 * Primeryi_pobit_operac_3.c
 * 

	

	unsigned int a=5, b=7;
	a = a^b;
	b = a^b;
	a = a^b;
	printf("a = %d, b = %d", a,b);
	// a = 7 b =5
	
	
	
	
	unsigned int mask, n=0xAB; // n = 1010 1011
	mask = (1<<5)-1;
	n = n ^ mask;
	printf("n = %x", n);
	// n = b4 1011 0100

 */


#include <stdio.h>
#include <inttypes.h>

int main()
{
	/*
	// Не используя дополнительных переменных, поменять местами значения двух
	переменных типа int. 
	
	unsigned int a=5, b=7;
	a = a+b;
	b = a-b;
	a = a-b;
	printf("a = %d, b = %d", a,b);
	// a = 7 b =5
	// недостаток данной реализации в том, что 
	// может произойти переполнение
	*/
	
	/*
	// десь нет такого недостатка
	unsigned int a=5, b=7;
	a = a^b;
	b = a^b;
	a = a^b;
	printf("a = %d, b = %d", a,b);
	// a = 7 b =5
	*/
	
	// Инвертировать 5 младших битов переменной x типа unsigned int, остальные биты
	// оставить без изменения. 
	unsigned int mask, n=0xAB; // n = 1010 1011
	printf("n = %x\n", n);
	//~ mask = (1<<5) - 1;
	//~ mask = 0x1F;
	mask = 0b11111;
	n ^= mask;
	printf("n = %x\n", n);
	// n = b4 1011 0100
	
	
	return 0;
}

