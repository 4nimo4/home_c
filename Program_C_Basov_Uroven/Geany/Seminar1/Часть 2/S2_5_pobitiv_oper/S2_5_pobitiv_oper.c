/*
 * S1_5_pobitiv_oper.c
 * 
 * Примеры побитовых операций
 * 
 * Поднять два младших бита в 4 байтном беззнаковом числе.
 * 
 * 
 */


#include <stdio.h>

int main()
{
	//unsigned int a=0xFFFFFF00, mask=-1;  // mask = 0xffffffff можно так
	unsigned int a=0xFFFFFF00, mask=~0;  // mask = 0xffffffff или  так
	mask >>= (32 - 2);		// Так делают редко
	//mask = 0b11;					// В основном делают так
	a |= mask;
	printf("a = %x", a);					// a = ffffff03
	return 0;
}

