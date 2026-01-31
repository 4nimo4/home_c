/*
 * Summa_cifr_natur_chisla.c

 * Найти сумму цифр натурального числа
 */

//Нерекурсивный способ:
int sumIter(int num) 
{
	int sum = 0;
	while(num > 0) 
		{
			sum += num % 10;
			num /= 10;
		}
		return sum;
}

//Рекурсивный способ:
int sumRec(int num) 
{
	if (num > 0)
		{
			return num % 10 + sumRec(num / 10);
		}
	else
		{
			return 0;
		}
}

#include <stdio.h>

int main(void)
{
	printf("%d\n", sumIter(123));
	printf("%d\n", sumRec(123));
	return 0;
}

