/*
 * S1_6_Otlich_logich_ot_pobit_oper.c
 * 
 * Отличие побитовых и логических операций
 * 

 * 
 * 
 */


#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>



int main(void)
{
	int32_t x=3, y=8;								//0b0011=3, 0b1000=8
	//Ниже-побитовая операция
	(x & y) ? printf("True ") : printf("False ");	//Получим False - 
													//0b0011 & 0b1000 = 0b0000
	//А здесь ниже-логическая операция - ЭТО КАК УСЛОВИЕ и ЛУЧШЕ ИСПОЛЬЗОВАТЬ ЕГО!!
	(x && y) ? printf("True ") : printf("False ");	//Получим True - 
													//0b0011 && 0b1000 = 0b0001
	return 0;
}

