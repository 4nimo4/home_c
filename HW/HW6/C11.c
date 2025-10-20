/*
 * C11.c
 * 
	НОД
	
	Составить функцию, которая определяет наибольший общий делитель двух 
	натуральных и привести пример ее использования. int nod(int a, int b)

	Формат входных данных
	Два целых положительных числа

	Формат результата
	Одно целое число - наибольший общий делитель.
 * 
 * 
 */


#include <stdio.h>
int nod(int, int);
int main()
{
	int x, y, z;
	scanf("%d%d", &x, &y);
	z = nod(x, y);
	printf("%d\n",z);
	return 0;
}

int nod(int a, int b)
{	
	int t;
//	printf("a=%d,b=%d,",a,b);
	while (b != 0)
		{
			t = b;
			b = a % b;
			a = t;
		}
	return a;
}
