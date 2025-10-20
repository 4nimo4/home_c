/*
 * D7.c
 * 
	От 1 до N
	
	Составить рекурсивную функцию, печать всех чисел от N до 1

	Формат входных данных
	Одно натуральное число
	
	Формат результата
	последовательность чисел от введенного числа до 1, через пробел
 * 
 * 
 */


#include <stdio.h>
 
void rec_num(int);
 
int main()
{
	int a;
    scanf("%d", &a);
    rec_num(a);
    return 0;
}
void rec_num(int b) 
{
		printf("%d ", b);
    if (b > 1)
        rec_num(b - 1);
    //printf("%d ", b);
}

