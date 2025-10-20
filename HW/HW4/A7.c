/*
 * A7.c
 * 
	Наибольшее из двух чисел
	Ввести два числа и вывести их в порядке возрастания.

	Input format
	Два целых числа

	Output format
	Два целых числа
 * 
 */


#include <stdio.h>

int main()
{
	int a, b, max, min;
	scanf("%d%d", &a, &b);
	max = a>b ? a : b;
	min = a<b ? a : b;
	printf("%d %d\n", min, max);
	
	return 0;
}

