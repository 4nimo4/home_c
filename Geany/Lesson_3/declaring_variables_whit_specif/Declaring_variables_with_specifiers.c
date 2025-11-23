/*
 Declaring_variables_with_specifiers.c
	Объявление переменных со спецификаторами
 */


#include <stdio.h>

int main()
{
	
	unsigned int a=3; // объявление целочисленной переменной
	unsigned long long int b=3; // объявление целочисленной переменной
	float f; // объявление вещественной переменной
	unsigned long long int su7=2, prime=7, five=5; // объявление переменных с инициализацией
	float pi=3.14; // объявление переменных с инициализацией
	unsigned char c, c2='A', m=10; // объявление символьных переменных с инициализацией
	long double big_pi; // объявление вещественной переменной двойной точности
	
	printf("%lld",sizeof(big_pi)); // забегая вперед
	return 0;
}

