/*/*
 * C12.c
 * 
	Вычислить sin
	
	Составить функцию, которая вычисляет синус как сумму ряда (с точностью 0.001)
	sin(x) = x - x3/3! + x5/5! - x7/7! + ...(x в радианах)
	float sinus(float x)

	Формат входных данных
	Одно число от 0 до 90
	
	Формат результата
	Вещественное число в формате "%.3f"
 * 
 * 
 */


#include <stdio.h>
#include <math.h>

#define eps 1e-3

float sinus(float);

int main()
{
    float a, b;
 
    if(scanf("%f", &a) != 1)
		{
			return 1;
		}
	b = sinus(a);
    printf("%.3g\n", b);
   // printf("sin(%g) = %.3g\n", grade, sin(grade * M_PI / 180));
 
    return 0;
}

float sinus(float x)
{
    float c, sum;
    sum = c = x *= (M_PI / 180);
    for( int n = 0; fabs(c) > eps; n++)
    {
        c  *= -x * x / (2 * n + 2) / (2 * n + 3);
        sum += c;
    }
    return sum;
}

