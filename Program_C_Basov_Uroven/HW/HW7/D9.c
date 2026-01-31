/*
 * D9.c
 * 
	Сумма цифр числа
	
	Дано натуральное число N. Вычислите сумму его цифр. Необходимо составить 
	рекурсивную функцию. int sum_digits(int n)

	Формат входных данных
	Одно натуральное число
	
	Формат результата
	Целое число - сумма цифр введенного числа.
 * 
 * 
 */


#include <stdio.h>
 
int sum_digits(int);
 
int main()
{
	int a, e;
    scanf("%d", &a);
    if (a == 0)
		{	
			printf("%d", a);
			return 0;		
		}
	e = sum_digits(a); 
	printf("%d", e);
	return 0;
}

int sum_digits(int b) 
{	
	int d=0;
	static int c=0;	
    if (b > 0)
		{	
			d = b%10;
			b /= 10;
			//printf("%d ", d); //Вывод в обратном порядке

			sum_digits(b);	
			
		}
		c += d;
		return c;	
 }  
