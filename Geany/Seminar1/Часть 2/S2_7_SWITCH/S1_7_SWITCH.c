/*
 * S1_7_SWITCH.c
 
 * 
 * 
 */


#include <stdio.h>

int main()
{
	
	switch (expr)
	{
		case 2: a *= 2 ; // Если expr == 2, то выполнится a += 5; из следующей  ветки 
		case 3: a += 5; 
		break; // А здесь произойдет выход 
		case 4: a -= b; 
		break;
		default: 
		break;
	} 
	return 0;
}

