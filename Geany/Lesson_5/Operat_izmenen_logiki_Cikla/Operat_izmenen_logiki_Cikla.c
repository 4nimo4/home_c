/*
 * Operat_izmenen_logiki_Cikla.c
 * 
	Оператор continue
	
	Continue – используется для немедленного перехода к следующей
	итерации цикла, пропуская оставшуюся часть тела цикла, при этом
	выражение expression3 не пропускается и выполняется до начала
	следующей итерации
	
	int i;
	for(i=1; i<5; i++) 
		{
			if (i==3)
			continue;
			printf("i = %d\n",i);
		}
		
	i = 1
	i = 2
	i = 4
	
	Оператор break
	
	Оператор break завершает выполнение ближайшего внешнего оператора
	do, for , switch или while , в котором он находится. Управление передаётся
	оператору, который расположен после завершенного оператора.
	
	char c;
	for(;;) 
		{
			printf( "\nPress any key, Q to quit: " ); // Convert to character value
			scanf("%c", &c);
			if (c == 'Q')
				break; // Loop exits only when 'Q' is pressed
		}

 * 
 * 
 */


#include <stdio.h>

int main()
{
	/*
	// Оператор continue
	int i;
	for(i=1; i<5; i++) 
		{
			if (i==3)
				continue;
			printf("i = %d\n",i);
		}
	*/
	
	// Опрератор break
	char c;
	for(;;) // while (1)
		{
			printf( "\nPress any key, Q or q to quit: " ); // Convert to character value
			scanf("%c", &c);
			printf("%x\n", c);
			if (c == 'Q' || c =='q')
				break; // Loop exits only when 'Q' is pressed
		}

	return 0;
}

