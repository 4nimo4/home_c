/*
 * C3.c
 * 
	Среднее арифметическое чисел
	
	Написать функцию, которая возвращает среднее арифметическое двух 
	переданных ей аргументов (параметров). int middle(int a, int b)

	Формат входных данных
	Два целых не отрицательных числа
	
	Формат результата
	Одно целое число
 * 
 * 
 */


#include <stdio.h>

int sredn_arifmetic(int, int);

int main()
{
	int a=0, b=0;
	scanf("%d%d", &a, &b);
	int c = sredn_arifmetic(a, b);
	printf("%d\n", c);
	return 0;
}

int sredn_arifmetic(int x, int y)
{
	int z=x;
//	printf("x=%d y=%d\n", x, y);

	z = (x+y)/2;
//			printf("%d\n", z);

//	printf("CANSEL\n");
	return z;
}

