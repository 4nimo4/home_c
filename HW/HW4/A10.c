/*
 * A10.c
 * 
	Наименьшее из пяти чисел
	Ввести пять чисел и найти  наименьшее из них.

	Input format
	Пять целых чисел
	
	Output format
	Одно целое число
 * 
 * 
 */


#include <stdio.h>

int main()
{
	int a, b, c, d, e, min;
	scanf("%d%d%d%d%d", &a, &b, &c, &d, &e);
	min = a<b ? a : b;
	min = min<=c ? min : c;
	min = min<=d ? min : d;
	min = min<=e ? min : e;
	printf("%d\n", min);
	return 0;
}
