/*
 * S2_8.c
 * Псевдослучайная двоичная последовательность
 * 
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char* argv[]) 
{
	
//	uint16_t start = 0x08; // С какого числа мы начинаем PRBS 15 = x^15 + x^14 + 1
	uint8_t start = 0x02; // С какого числа мы начинаем PRBS 7 = x^7 + x^6 + 1
//	uint16_t a = start;	//PRBS 15
	uint8_t a = start;	//PRBS 7
	int i;
	for (i = 1;; i++) 
		{
//			int newbit = (((a >> 14) ^ (a >> 13)) & 1);	//PRBS 15
			int newbit = (((a >> 6) ^ (a >> 5)) & 1);	//PRBS 7
//			a = ((a << 1) | newbit) & 0x7fff;	//PRBS 15
			a = ((a << 1) | newbit) & 0x7f;		//PRBS 7
			printf("%x\n", a);
			if (a == start) 
				{
					printf("repetition period is %d\n", i);
					break;
				}
		}
}
