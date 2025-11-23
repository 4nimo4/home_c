/*
 * Primeryi_pobit_operac_1.c
 * 
 * «Упаковать» четыре символа в беззнаковое целое. Размер беззнакового целого - 4
	байта.
	
 * 	unsigned int n=0;
	unsigned char a=0xaa,b=0xbb,c=0xcc,d=0xdd;
	n = a;
	n = (n<<8) | b;
	n = (n<<8) | c;
	n = (n<<8) | d;
	printf("n = %x", n);
	// n = aabbccdd
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{
	uint32_t n=0;
	uint8_t a=0xaa,b=0xbb,c=0xcc,d=0xdd;
	n = a;	// n=0xaa
	n = (n<<8) | b; // n=0xaa00 | 0xbb = 0xaabb
	printf("n = %x00\n", n);
	n = (n<<8) | c; // n=0xaabb00 | 0xcc = 0xaabbcc
	printf("n = %x00\n", n);
	n = (n<<8) | d; // n=0xaabbcc00 | 0xdd = 0xaabbccdd
	printf("n = %x\n", n);
	// n = aabbccdd

	return 0;
}

