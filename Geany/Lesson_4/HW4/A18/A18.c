/*
 * A18.c
 * 
	Above, less, equal
	Ввести два числа. Если первое число больше второго, 
	то программа печатает слово Above. Если первое число меньше второго, 
	то программа печатает слово Less. 
	А если числа равны, программа напечатает сообщение Equal.

	Input format
	Два целых числа
	
	Output format
	Одно единственное слово: Above, Less, Equal


 * 
 * 
 */


#include <stdio.h>

int main()
{
	int a, b;
	scanf("%d%d", &a, &b);
	
	if (a>b)
		{
			printf("Above\n"); // a>b
			return 0;
		}
		
	if (a<b)
		{
			printf("Less\n"); // a<b
			return 0;
		}
			

	if (a==b)
		{
			printf("Equal\n"); // a=b
			return 0;
		}
		

	return 0;
}
