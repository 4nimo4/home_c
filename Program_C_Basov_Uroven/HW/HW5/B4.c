/*
 * B4.c
	Ровно три цифры
	Ввести целое число и определить, верно ли, что в нём ровно 3 цифры.

	Input format
	Целое положительное число
	
	Output format
	Одно слов: YES или NO


 */


#include <stdio.h> //
#include <locale.h>


int main()
{
	int a, b=99, c=1000;
	scanf("%d", &a);
	printf("a=%d\n", a);
	if ((a>b) && (a<c))
		{
			printf("YES\n");
			return 0;
		}
			
	printf("NO\n");
	return 0;
}
