/*
 * Zamena_cikla_na_Recursiyu_1.c
 * 
 * Замена цикла на рекурсию 1
 * 
 * Необходимо реализовать с помощью рекурсии данный цикл:
 * 	for(i=1;i<n;i++)
 * 		{
			printf("%d ",i);
		}
 */


#include <stdio.h>

void interFor(int start, int n) 
{
	for(int i=start; i<n; i++)
		{
			printf("%d ",i);
		}
}		

void recFor(int i, int n) 
		{
			if(i==n)
				{
					return;
				}
			printf("%d ", i);
			recFor(i+1, n);
		}
		
int main()
{
	recFor(0,5);
	printf("\n");
	interFor(0,5);
	return 0;
}

