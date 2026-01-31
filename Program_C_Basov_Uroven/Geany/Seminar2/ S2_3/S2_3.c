/*
 * S2_3.c
 * Статические переменные и указатели

 * 
 * 
 */


#include <stdio.h>
	int y=1;
int f(int *pa) 
	{
		static int x=3;
		x += y;
		*pa = x;
		return x + *pa + y;
	}
int main(void)
{
	int n = 10;
	y++;
	printf("Answer = %d\n", f(&n));
	printf("Answer = %d\n", f(&n));
}
