/*
 * C2.c
 * 
	Возвести в степень
	
	Составить функцию, возведение числа N в степень P. int power(n, p) 
	и привести пример ее использования.

	Формат входных данных
	Два целых числа: N по модулю не превосходящих 1000 и P ≥ 0
	
	Формат результата
	Одно целое число
 * 
 * 
 */


#include <stdio.h>

int vozved_v_step(int, int);

int main()
{
	int a=0, b=0;
	scanf("%d%d", &a, &b);
	int c = vozved_v_step(a, b);
	printf("%d\n", c);
	return 0;
}

int vozved_v_step(int x, int y)
{
	int z=x;
//	printf("x=%d y=%d\n", x, y);
	if(y == 0) // Если степень числа (Не равного 0 )равна 0, то на выходе 
				// всегда 1. Если само число равно 0, то оно в любой стпени
				// будет равно 0
		return 1;
	for(int i=1; i<y; i++)
		{
			z = z*x;
//			printf("%d\n", z);
		}
//	printf("CANSEL\n");
	return z;
}
