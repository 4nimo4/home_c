/*
 * D2.c
 * 
	Сумма чисел от 1 до N
	
	Составить рекурсивную функцию, которая определяет сумму всех чисел от 1 до N

	Формат входных данных
	Одно натуральное число
	
	Формат результата
	Сумма чисел от 1 до введенного числа
 * 
 * 
 */


#include <stdio.h>
 
int rec_n(int);
 
int main()
{
	int a, c;
    scanf("%d", &a);
    c = rec_n(a);
    printf("%d ", c);
    return 0;
}
int rec_n(int b) 
{
    // printf("%d ", b);
    if (b == 1)
		{
			return b;
		}
	else
		{
			b += rec_n(b-1);
			return b;
		}
    
}

