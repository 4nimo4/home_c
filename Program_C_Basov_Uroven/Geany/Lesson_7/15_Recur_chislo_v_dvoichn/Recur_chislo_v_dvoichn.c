/*
 * Recur_chislo_v_dvoichn.c
 * 
	Написать рекурсивную функцию перевод числа в двоичную систему
	счисления
	
 * 
 * 
 */



#include <stdio.h>
//
void dec_to_bin(int n)
{
	if(n >= 2)
		{
			dec_to_bin(n/2);
		}
	
	printf("%d", n%2);
 }

int main()
{
	int a;
	scanf("%d", &a);
	dec_to_bin(a);
	
	return 0;
}

