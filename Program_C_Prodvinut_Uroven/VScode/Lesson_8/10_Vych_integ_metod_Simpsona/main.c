/*
//------------------------------------------------------ 
// Вычисление интеграла по методу Симпсона
//------------------------------------------------------
Суть метода заключается в приближении подъинтегральной функции 
на отрезке [a,b] параболой. Аналогично двум предыдущим
методам отрезок разбивается на n равных частей, 
для каждой из частей применяется метод Симпсона.

Формулой Симпсона называется:

  xr       xr      
  ∫ f(x) ≈ ∫ p(x) ≈ ((xr - xl)/6) * (f(xl) + 4f((a + b)/2) + f(xr))
 xl       xl      

где p(x) — парабола.
*/

#include <stdio.h>    // printf
#include <math.h>     // pow
#include <stdlib.h>   // size_t

#define ACCURACY   1000   // число разбиений интервала

typedef double (*function)(double);
// Тип function — указатель на функцию вида double f(double)

//------------------------------------------------------
// Интегрируемая функция f(x)
// f(x) = 8x^4 + 32x^3 + 40x^2 + 16x + 1
//------------------------------------------------------
double f(double x)
{
    return 8.0*pow(x, 4.0) + 32.0*pow(x, 3.0) + 40.0*pow(x, 2.0) + 16.0*x + 1.0;
    // Можно и без pow:
    // return 8.0*x*x*x*x + 32.0*x*x*x + 40.0*x*x + 16.0*x + 1.0;
}

//------------------------------------------------------
// Метод прямоугольников (левых)
//------------------------------------------------------
double calcIntegralSquare(double xl, double xr, size_t n, function f)
{
    double sum = 0.0;
    double h = (xr - xl) / (double)n;

    double x = xl;
    for (size_t i = 0; i < n; ++i) {
        sum += f(x);  // высота в левой точке
        x += h;
    }

    return sum * h;
}

//------------------------------------------------------
// Метод трапеций (классическая формула)
// I ≈ h * [ (f(x0)+f(xn))/2 + f(x1)+...+f(x_{n-1}) ]
//------------------------------------------------------
double calcIntegralTrap(double xl, double xr, size_t n, function f)
{
    double h = (xr - xl) / (double)n;
    double sum = 0.5 * (f(xl) + f(xr));

    for (size_t i = 1; i < n; ++i) {
        double x = xl + i * h;
        sum += f(x);
    }

    return sum * h;
}

//------------------------------------------------------
// Метод Симпсона
// I ≈ h/3 * [ f(x0) + f(xn) + 4*(сумма по нечётным) + 2*(по чётным) ]
// Требует чётного n.
//------------------------------------------------------
double calcIntegralSimpson(double xl, double xr, size_t n, function f)
{
    if (n % 2 == 1) {
        ++n; // делаем n чётным
    }

    double h = (xr - xl) / (double)n;
    double sum = f(xl) + f(xr);

    for (size_t i = 1; i < n; ++i) {
        double x = xl + i * h;
        if (i % 2 == 0) {
            sum += 2.0 * f(x);  // чётные
        } else {
            sum += 4.0 * f(x);  // нечётные
        }
    }

    return sum * h / 3.0;
}

int main(void)
{
    size_t n = ACCURACY;
    double a = -1.382683;
    double b = -0.617316;

    printf("Число разбиений интервала для всех методов = %zu\n", n);

    double I_square  = calcIntegralSquare(a, b, n, f);
    double I_trap    = calcIntegralTrap(a, b, n, f);
    double I_simpson = calcIntegralSimpson(a, b, n, f);

    printf("calcIntegralSquare  = %.10f\n", I_square);
    printf("calcIntegralTrap    = %.10f\n", I_trap);
    printf("calcIntegralSimpson = %.10f\n", I_simpson);
    /*
    вычислили какой метод точнее производит вычисления:

    Число разбиений интервала для всех методов = 100
    calcIntegralSquare  = 0.4926934965
    calcIntegralTrap    = 0.4926934799
    calcIntegralSimpson = 0.4927357535

    Число разбиений интервала для всех методов = 1000
    calcIntegralSquare  = 0.4927353296
    calcIntegralTrap    = 0.4927353280
    calcIntegralSimpson = 0.4927357507

    Число разбиений интервала для всех методов = 10000
    calcIntegralSquare  = 0.4927357466
    calcIntegralTrap    = 0.4927357464
    calcIntegralSimpson = 0.4927357507

    Число разбиений интервала для всех методов = 100000
    calcIntegralSquare  = 0.4927357506
    calcIntegralTrap    = 0.4927357506
    calcIntegralSimpson = 0.4927357507

    Число разбиений интервала для всех методов = 1000000
    calcIntegralSquare  = 0.4927357507
    calcIntegralTrap    = 0.4927357507
    calcIntegralSimpson = 0.4927357507

    Вывод: Метод Симпсона обладает самой высокой точностью вычислений
    Уже на 1000 разбиений интервала он оказывается самым точным
    */

    return 0;
}