/*
 * Primeryi_pobit_operac.c
 * 
 * Примеры побитовых операций
 * 
 * Переменная имеет тип unsigned int, необходимо обнулить 3 младших бита числа.
 * 
	unsigned int a=0xAABBCCFF;
	a = a & ~7;
	printf("a = %x", a);
	// a = aabbccf8
	
	Поднять два младших бита в 4 байтном беззнаковом числе
	
	unsigned int a=0xFFFFFF00;
	a = a | 3;
	printf("a = %x", a);
	// a = ffffff03
 * 
 */


#include <stdio.h>
#include <inttypes.h>

int main()
{
	// необходимо обнулить 3 младших бита числа.
	
//	unsigned int a=0xAABBCCFF;
//	a = a & ~7;
//	printf("a = %x", a); // a = aabbccf8
	
	// Поднять два младших бита в 4 байтном беззнаковом числе
	
//	unsigned int a=0xFFFFFF00;
//	a = a | 0b11;
//	printf("a = %x", a);
	// a = ffffff03
	
	int32_t u=0xaabbccdd;
	printf("0x%X\n",u); // 0aabbccdd
	u >>= 4; // арифметический сдвиг
	printf("0x%08x\n",u); // faabbccd
	if(u&(1<<3)) // 1248
		printf("Bit3");
	
	return 0;
}

