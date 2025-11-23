/*
 * Recur_posled_chisel_max.c
 * 
	Дана последовательность натуральных чисел,завершающаяся числом 0.
	Найдите максимум
	
 * 
 * 
 */

#include <stdio.h>
#include <inttypes.h>

uint32_t max_find(void) 
{
	uint32_t number, max;
	scanf("%u", &number);
	if(number==0)
		{
			return 0;
		}
	max = max_find();
	if (max<number)
		{
			max=number;
		}
	return max;
}

int main()
{
	uint32_t a;
	a = max_find();
	printf("%d", a);
	return 0;
}
