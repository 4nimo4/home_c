/*
 * A14.c
 * 
	Максимальная цифра
	Дано трехзначное число, напечатать макисмальную цифру

	Input format
	Целое положительное трехзначное число
	
	Output format
	Одна цифра
 * 
 * 
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{
	int max, a, b, c;
	scanf("%d", &max);
	
	a = (max/100)%10; //сотни
	b = (max/10)%10; //десятки 
	c = max%10; //единицы
	
	max = a>b ? a : b;
	max = max>c ? max : c;
	
	printf("%d", max);

	return 0;
}

