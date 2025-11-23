/*
 * Zamena_cikla_na_Recursiyu_2.c
 * 
 * Замена цикла на рекурсию 2
 * 
 * 	------------------- Используем только один параметр в функции!!!------------------------------
 
 * Необходимо реализовать с помощью рекурсии данный цикл:
 * 	for(i=1;i<n;i++)
 * 		{
			printf("%d ",i);
		}
 */


#include <stdio.h>

void interFor(int n) 
{
	for(int i=0; i<n; i++)
		{
			printf("%d ",i);
		}
}		

void recFor(int n) 
		{
			static int i=0;	//-------------------------------------
			if(i==n)
				{
					return;
				}
			printf("%d ", i++);
			recFor(n);
		}
		
int main()
{
	recFor(5);
	printf("\n");
	interFor(5);
	return 0;
}
