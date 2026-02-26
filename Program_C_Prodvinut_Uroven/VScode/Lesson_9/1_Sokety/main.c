/*
//------------------------------------------------------ 
// Сокеты
//------------------------------------------------------
// Что такое сокет? Какими они бывают?
//------------------------------------------------------

*/

#include <stdio.h>    // ввод-вывод (printf и др.)
#include <math.h>     // fabs и т.п.
#include <stdlib.h>   // rand, srand, RAND_MAX

#define ACCURACY 1000 //Точность с которой будем делать вычисления
#define ACCURACY_2 100000 //Точность с которой будем делать вычисления

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

//------------------------------------------------------ 
// Вычисление интеграла с помощью метода Монте-Карло
//------------------------------------------------------
float calcIntegralMonteCarlo(float xl, float xr, float fmax, size_t n, function f) 
{
    size_t in_d = 0;
    float width = fabs(xr-xl), height = fmax;
    for(size_t i=0; i<n; i++) 
    {
        float x = ((float)rand()/(float)RAND_MAX) * width - fabs(xl);
        float y = (float)rand()/(float)RAND_MAX * height;
        if(y<f(x))
            in_d++;
    }
    return fabs( (float)in_d / n * width * height );
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
    printf("calcIntegralMonteCarlo = %f\n", calcIntegralMonteCarlo(-1.382683,-0.617316,1,ACCURACY_2,f));
        //calcIntegralMonteCarlo = 0.494190
        //- точность гораздо хуже хотя ACCURACY_2 = 100000, а не 1000 как в других выриантах
    
    return 0;
}