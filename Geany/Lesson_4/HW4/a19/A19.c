/*
 * A19.c
 * 
	Существует ли треугольник
	Даны стороны треугольника a, b, c. 
	Определить существует ли такой треугольник.

	Input format
	Три целых числа. Стороны треугольника a, b, c.
	
	Output format
	YES или NO



 * 
 * 
 */


#include <stdio.h>

int main()
{
	int a, b, c;
	scanf("%d%d%d", &a, &b, &c);
	
	if (((a+b)<=c) || ((a+c)<=b) || (b+c)<=a)
		{
			printf("NO\n"); 
			return 0;
		}		

	printf("YES\n"); 

	return 0;
}


