/*
 * S1_12_Cikl.c
 * 
	Дано натуральное число n(n>0). Получить все его натуральные делители.
 * 
 * 
 */

// Что то не то---------------------------------------------------
#include <stdio.h>
#include <inttypes.h>
int main(void)
{
    uint32_t n, i=2;
    scanf("%" PRIu32 , &n);
    while(i<=n) 
		{
			if(n%i == 0) 
				{
					printf("%" PRIu32 " ",i);
					n/=i;
				} 
			else 
				{
					i++;
				}
		}
    printf("\n");
    return 0;
}
