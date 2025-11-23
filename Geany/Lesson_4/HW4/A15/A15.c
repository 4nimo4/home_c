/*
 * A15.c
 * 
	Уравнение прямой
	Определить уравнение прямой по координатам двух точек. Уравнение вида

	y=k*x+b
	Input format
	Четыре целых числа через пробел. Координаты X1 Y1 X2 Y2
	Output format
	Два числа K,B в формате "%.2f %.2f"

	k=(Y2-Y1)/(X2-X1)
	b=y-k*x

 * 
 * 
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{
	float X1, Y1, X2, Y2, k, b;
	scanf("%f%f%f%f", &X1, &Y1, &X2, &Y2);
	
	k = (Y2-Y1)/(X2-X1);
	b = Y1 - k*X1;
	
	printf("%0.2f %0.2f\n", k, b); 

	return 0;
}
