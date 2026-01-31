/*
 * A17.c
 * 
	Какое время года
	Ввести номер месяца и вывести название времени года.

	Input format
	Целое число от 1 до 12 - номер месяца.
	
	Output format
	Время года на английском: winter, spring, summer, autumn

 * 
 * 
 */


#include <stdio.h>

int main()
{
	int a;
	scanf("%d", &a);
	
	if ((a==12) || ((a>=1) && (a<=2)))
		{
			printf("winter\n"); // зима 12,1,2
			return 0;
		}
		
	if ((a>=3) && (a<=5))
		{
			printf("spring\n"); // весна 3,4,5
			return 0;
		}
			

	if ((a>=6) && (a<=8))
		{
			printf("summer\n"); // лето 6,7,8
			return 0;
		}
		
	if ((a>=9) && (a<=11))
		{
			printf("autumn\n"); // осень 9,10,11
			return 0;
		}
	
	return 0;
}

