/*
 * A16.c
 * 
	В порядке возрастания
	Ввести три числа и определить, верно ли, что они вводились в порядке возрастания.

	Input format
	Три целых числа
	
	Output format
	Одно слово YES или NO
 * 
 * 
 */


#include <stdio.h>

int main()
{
	int a, b, c, min;
	scanf("%d%d%d", &a, &b, &c);
	
	min = a<b ? a : b;
	if ((min!=a) || (a==b))
		{
			printf("NO\n");
			return 0;
		}
		
	min = b<c ? b : c;
	if ((min!=b) || (b==c))
		{
			printf("NO\n");
			return 0;
		}
			
	printf("YES\n");
	
	return 0;
}

