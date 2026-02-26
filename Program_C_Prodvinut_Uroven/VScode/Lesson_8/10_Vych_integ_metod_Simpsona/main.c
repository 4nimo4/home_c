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

#include <stdio.h>    // ввод-вывод (printf и др.)
#include <math.h>

#define ACCURACY 1000 //Точность с которой будем делать вычисления

typedef float(*function)(float);

//(-2, -1.5) (-1.5, -1) (-1, -0.5) (-0.5, 0)
float f(float x) 
{
    return 8*x*x*x*x + 32*x*x*x + 40*x*x + 16*x + 1;
}

//------------------------------------------------------ 
// Для тестового прямоугольника
//------------------------------------------------------
float testf(float x)
{
    return 2;
}

//------------------------------------------------------ 
// Вычисление интеграла методом прямоугольника
//------------------------------------------------------
float calcIntegralSquare(float xl, float xr, size_t n, function f) 
{
    float sum = 0;
    float h = (xr-xl)/n;
    for(size_t i=0; i<n; i++) 
    {
        sum += f(xl);
        xl += h;
    }
    return sum*h;
}

//------------------------------------------------------ 
// Вычисление интеграла методом трапеций
//------------------------------------------------------
float calcIntegralTrap(float xl, float xr, size_t n, function f) 
{
    float sum = 0;
    float h = (xr-xl)/n;
    for(float x=xl+h; x<xr-h; x+=h) 
    {
        sum += 0.5*h*(f(x)+f(x+h));
    }
    return sum;
}

//------------------------------------------------------ 
// Вычисление интеграла по методу Симпсона
//------------------------------------------------------
float calcIntegralSimpson(float xl, float xr, size_t n, function f) 
{
    float sum = 0;
    float h = (xr-xl)/n;
    for(float x=xl+h; x<xr-h; x+=h) 
    {
        sum += h/6.0*(f(x) + 4.0*f(0.5*(x+x+h)) + f(x+h));
    }
    return sum;
}

int main(void) 
{

    printf("calcIntegralSquare integral %f\n", calcIntegralSquare(0,2,ACCURACY,testf));
        //Тестовая функция для проверки точности вычисления площади квадрата
        //со сторонами 2 х 2 = 4
        //calcIntegralSquare integral 4.000000
        /*
        float testf(float x) вот эта часть кода
        {
            return 2;
        }
        */
    printf("calcIntegralSquare = %f\n", calcIntegralSquare(-1.382683,-0.617316,ACCURACY,f));
        //calcIntegralSquare = 0.492744 - площадь половины лепестка синусоиды
        //выше оси абцисс
    printf("calcIntegralTrap = %f\n", calcIntegralTrap(-1.382683,-0.617316,ACCURACY,f));
        //calcIntegralTrap = 0.492743
    printf("calcIntegralSimpson = %f\n", calcIntegralSimpson(-1.382683,-0.617316,ACCURACY,f));
        //calcIntegralSimpson = 0.492743
    return 0;
}