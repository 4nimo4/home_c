/*
 * Recurs_NOD.c
 * 
	Описать рекурсивную функцию вычисления НОД
	
 * 
 * 
 */


#include <stdio.h>
//
int gcd(int n, int m)
{
	if(n==m)
		{
			return n;
		}
	if (n<m)
		{
			return gcd(n,m-n);
		}
	else
		{
			return gcd(n-m,m );
		}
	
 }

int main()
{
	int a, b, c;
	scanf("%d%d", &a, &b);
	c = gcd(a,b);
	printf("%d", c);
	return 0;
}
