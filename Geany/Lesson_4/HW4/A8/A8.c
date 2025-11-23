/*
 * A8.c
 * 
	Наибольшее из трех чисел
	Ввести три числа и найти наибольшее из них

	Input format
	Три целых числа через пробел

	Output format
	Одно наибольшее целое число


 * 
 * 
 */


#include <stdio.h>

int main()
{
	int a, b, c, max;
	scanf("%d%d%d", &a, &b, &c);
	max = a>b ? a : b;
	max = max>c ? max : c;
	printf("%d\n", max);
	return 0;
}

