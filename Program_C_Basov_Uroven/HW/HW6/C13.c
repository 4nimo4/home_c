/*/*
 * C13.c
 * 
	Вычислить cosin
	
	Составить функцию, которая вычисляет косинус как сумму ряда (с точностью 0.001)
	cos(x) = 1 - x2/2! + x4/4! - x6/6! + ... (x в радианах)
	
	sin(x) = x - x3/3! + x5/5! - x7/7! + ...(x в радианах)
	float cosinus(float x)

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

float cosinus(float);

int main()
{
    float a, b;
 
    if(scanf("%f", &a) != 1)
		{
			return 1;
		}
	b = cosinus(a);
    printf("%.3g\n", b);
   // printf("sin(%g) = %.3g\n", grade, sin(grade * M_PI / 180));
 
    return 0;
}

float cosinus(float x)
{
    float c, sum;
    sum = c = x *= (M_PI / 180);
    for( int n = 0; fabs(c) > eps; n++)
    {
        c  *= -x*x / (2 * n - 1) * (2 * n);
        sum += c;
    }
    return sum;
}
