/*
 * Cifryi_v_chislo.c
 * Дана строка, состоящая из английских букв и цифр. Составьте из цифр в
	данной строке одно число и поместите его в целочисленную
	переменную

 */


#include <stdio.h>
int main()
{
	char c;
//	int Number=0;	// Если тип int, то можно словить переполнение, тогда 
					//1. можно изменить тип unsigned long long
					//2. проверять число на переполнение
	unsigned long long Number=0;				
	while( (c=getchar())!='\n') //спец символ новой строки
		{
			if(c>='0' && c<='9')
				{
				Number = Number*10+c-'0';
				}
		}
//	printf("%d",Number);		//1. можно изменить тип unsigned long long
	printf("%lld",Number);
 return 0;
}


