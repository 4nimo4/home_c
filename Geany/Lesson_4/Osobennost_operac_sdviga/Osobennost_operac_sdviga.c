/*
 * Osobennost_operac_sdviga.c
 *  
	Особенности операции сдвига
	
	int a=1,b=-1;
	printf("a = %d b = %d", a>>32, b>>-1);
	// a = 0 b = -1
	
	Сдвиг на отрицательное число бит или на число, превосходящее размер первого
	операнда, не определен.
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{

	int a=1,b=-1;
	printf("a = %d b = %d", a>>32, b>>-1);
	// a = 0 b = -1
	
	
	return 0;
}
