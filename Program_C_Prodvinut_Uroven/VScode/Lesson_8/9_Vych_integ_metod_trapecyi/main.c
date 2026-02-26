/*
//------------------------------------------------------ 
// Вычисление интеграла методом трапеций
//------------------------------------------------------
В отличие от метода прямоугольника, интеграл в данном методе
разбивается на трапеции. Т.к замена происходит на элементарную
линейную функцию, площадь вычисляется точнее.

Площадь трапеции вычисляется по формуле:

SQUARE = (a + b)/2 * h , где a и b — основания, а h — высота трапеции.

Составная формула для вычисления интеграла будет выглядеть:

   xr       xr
   ∫  f(x) ≈ ∑   (f(xi) + f(xi+1))/2 * (xi+1 - xi)
  xl        i=xl

Если сетка достаточно велика, то высотой можно пренебречь.
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
    return 0;
}