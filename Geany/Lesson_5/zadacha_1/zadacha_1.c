/*
 * zadacha_1.c
 * 
 *	Напечатать квадраты целых чисел от 1 до 10.
 * 
 * 
 */


#include <stdio.h>

int main()
{
	// Используя цикл while
//	int i=1;
//	while (i<=10) 
//		{
//			printf("%d\n",i*i);
//			i++;
//		}
		
	// Используя цикл for
	//~ int i;
	//~ for(i=1; i<=10; i++) 
		//~ {
			//~ printf("%d\n",i*i);
		//~ }	
		
	// со стандарта C99 можно переменную i объявлять
	// прямо внутри скобок цикла for
	for(int i=1; i<=10; i++) 
		{
			printf("%d\n",i*i);
		}		
	return 0;
}

