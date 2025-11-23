/*
 * Primeryi_pobit_operac_2.c
 * 
 * «Распаковать» беззнаковое целое число в четыре символа. Размер беззнакового
	целого - 4 байта
	
 * 	unsigned int n=0xaabbccdd;
	unsigned char a,b,c,d;
	d = n;
	c = n>>8;
	b = n>>16;
	a = n>>24;
	printf("a = %x, b = %x, c = %x, d = %x",
	a,b,c,d);
	// a = aa, b = bb, c = cc, d = dd
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{
	unsigned int n=0xaabbccdd;
	unsigned char a,b,c,d;
	d = n;
	c = n>>8;
	b = n>>16;
	a = n>>24;
	printf("a = %x, b = %x, c = %x, d = %x",
	a,b,c,d);
	// a = aa, b = bb, c = cc, d = dd

	return 0;
}

