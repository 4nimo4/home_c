/*
 * Reshenie.c
 * 
 * 
 */


#include <stdio.h>

int main()
{
	int n = 123;
	int sum = 0;
	sum = n % 10; // sum=3
	printf("%d\n", sum);
	sum += (n / 10)%10; // sum=2+3=5
	printf("%d\n", sum);
	sum += (n/100)%10; // sum=5+1=6
	printf("%d\n", sum);
	return 0;
}

