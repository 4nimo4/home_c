/*
 * Digits_razlozhi_chislo.c
 * 
 * 
 */


#include <stdio.h>

int main(void)
{
	int input, n, count, sum=0;
	printf("Input number: ");
	scanf("%d", &input);
	count = 0;
	n = input;
	while (n != 0) // n>0
		{
			count++;
			sum += n%10;
			n /=  10; // Отбросили одну цифру
		}
	printf("In %d fount %d digits sum=%d ", input, count, sum);
	return 0;
}

