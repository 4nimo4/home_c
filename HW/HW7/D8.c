/*
 * D8.c
 * 
	От A до B
	
	Составить рекурсивную функцию, Выведите все числа от A до B включительно, 
	в порядке возрастания, если A < B, или в порядке убывания в противном случае.

	Формат входных данных
	Два целых числа через пробел.

	
	Формат результата
	Последовательность целых чисел.
 * 
 * 
 */


#include <stdio.h>
 
void rec_num_max_a(int, int);
void rec_num_max_b(int, int);
 
int main()
{
	int a,b;
    scanf("%d%d", &a, &b);
    if(a > b)
		{
			rec_num_max_a(a,b);
		}
    else
		{
			rec_num_max_b(a,b);
		}
    return 0;
}
void rec_num_max_a(int c, int d) 
{
    if (c >= d)
		{
			printf("%d ", c);
			rec_num_max_a(c - 1, d);
		}
}

void rec_num_max_b(int f, int e)
{
	if(f <= e)	
		{
			rec_num_max_b(f, e - 1);
			printf("%d ", e);
		}
}


