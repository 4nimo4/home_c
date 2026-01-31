/*
 * Modifikator_static.c
 * 
 * Модификатор static переменных
 * 
 */


#include <stdio.h>


float InputFloat(char* message)
{
	float number;
	static int counter = 0; // если бы не было модификатора static пред int
		//, то при каждом входе в функцию float InputFloat(char* message)
		// счетчик counter обнулялся бы, а так его значение будет приростать 
		// при каждом новом входе в функцию
	counter++;
	printf("%d,%s",counter,message);
	scanf("%f",&number);
	return number;
}

int main(int argc, char **argv)
{
	float InputFloat(char* message);
	return 0;
}

