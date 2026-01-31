/*
 * S2_9.c
 * Задача вычисления Sin через ряд Тейлора
 * 
 * Составить функцию, которая вычисляет синус как
	сумму ряда (с точностью 0.001
	sin(x) = x - x3/3! + x5/5! - x7/7! + ...(x в радианах)
 */
#include <stdio.h>
#include <math.h>
const float PI = 3.1415926535;
const float EPS = 0.000001;

double sinx(double x, double eps) //
{
	double Xn = x;
	double sum = 0.0;
	int i = 1;
	double SqrX = x*x;
	do
		{
			sum += Xn;
			Xn *= -1.0 * SqrX / ((2 * i) * (2 * i + 1));
			i++;
		}
	while (fabs(Xn) > eps);
 return sum;
}
int main()
{
 double x;
 scanf("%lf",&x);
 x *= PI/180.0;
 //printf("sinx =%lf\tlibSin =%f",sin(x),sinx(x,EPS));
 printf("%.6f",sinx(x,EPS));
 return 0;
}
