/*
 * Opisanie_constant.c
 * 
	Описание целых констант
	
	Перечисления
	
	enum { N = 100 };
	
	Константы, объявле нные таким образом , подчиняются правилам
	видимости идентификаторов языка Си
	
	Описание констант через #define
	
	#define N 100
	
	// Хорошо
	
	#define NAME_LENGTH 100
	unsigned int age, height;
	char lastName[NAME_LENGTH], firstName[NAME_LENGTH];
 * 
 * 	// Плохо
 * 
	unsigned int a, h;
	char ln[25], f[25];
	
 * 	int myVar = 3; // плохо
	int g_myVar = 3; // уже лучше
 */


#include <stdio.h>

int main()
{
	
	return 0;
}

