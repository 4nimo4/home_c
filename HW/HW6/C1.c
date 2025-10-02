/*
 * C1.c
 * 
 * Модуль числа
 * 
 * Составить функцию, модуль числа и привести пример ее использования.
 * 
 * Формат входных данных
	Целое число
	
	Формат результата
	Целое не отрицательное число

 */


#include <stdio.h>
int abs(int num);

int main()
{
	int num;
	scanf("%d",&num);
	printf("%d",abs(num));
 return 0;
}

int abs(int num)
{
	return (num<0)?-num:num;
}
