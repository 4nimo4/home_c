/*
 * A11.c
 * 
	Сумма максимума и минимума
	Напечатать сумму максимума и минимума.

	Input format
	Пять целых чисел через пробел
	
	Output format
	Одно целое число - сумма максимума и минимума
 * 
 * 
 */


#include <stdio.h>

int main()
{
	int a, b, c, d, e, max, min;
	scanf("%d%d%d%d%d", &a, &b, &c, &d, &e);
	
	max = a>b ? a : b;
	max = max>c ? max : c;
	max = max>d ? max : d;
	max = max>e ? max : e;
	
	min = a<b ? a : b;
	min = min<=c ? min : c;
	min = min<=d ? min : d;
	min = min<=e ? min : e;
	
	max += min;
	printf("%d\n", max);
	return 0;
}

