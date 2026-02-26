/*
//------------------------------------------------------ 
// Метод касательных (Ньютона)
//------------------------------------------------------
Нельзя попадать в точку перегиба!!!!
Основная идея метода заключается в следующем: задаётся начальное
приближение вблизи предположительного корня.
После чего строится касательная к графику исследуемой функции в точке
приближения, для которой находится пересечение с осью абсцисс.
Эта точка берётся в качестве следующего приближения.
И так далее, пока не будет достигнута необходимая точность.
На рисунке видно, что последующее приближение x n+1 лучше предыдущего xn.

Находим следующую точку по формуле

Xi+1 = Xi - f(Xi) / (f'(Xi)

Для реализации данного метода необходимо вычислить производную от функции в
примере:

f(x) = 8x^4 + 32x^3 + 40x^2 + 16x + 1

Производная будет:
fʼ(x) = 32x^3 + 96x^2 + 80x + 16

*/

#include <stdio.h>    // ввод-вывод (printf и др.)
#include <math.h>

#define ACCURACY 0.001 //Точность с которой будем делать вычисления

typedef float(*function)(float);

//(-2, -1.5) (-1.5, -1) (-1, -0.5) (-0.5, 0)
//------------------------------------------------------
//Для Линейного поиска
//------------------------------------------------------
float f(float x) 
{
    return 8*x*x*x*x + 32*x*x*x + 40*x*x + 16*x + 1;
}

//------------------------------------------------------
//Для функции поиска корня методом деления отрезка пополам
//и модифицированной функции поиска корня
//------------------------------------------------------
int signF(float x,function f)
{
    return f(x)==0 ? 0 : (f(x) < 0 ? -1:+1);
}

//------------------------------------------------------
//Для метода касательных Ньютона
//------------------------------------------------------
float df(float x)
{
    return 32*x*x*x + 96*x*x + 80*x + 16;
}
//------------------------------------------------------



//------------------------------------------------------ 
// Линейный поиск
//------------------------------------------------------
float rootFindLineSearch(float xl, float xr, float eps,function f) 
{
    float x, minx = xl, nextstep;
    nextstep = fabs(xr-xl)/(1/eps); //разбиваем на отрезки интервал
    int stepcount=0;
    for(x=xl; x<xr; x += nextstep, stepcount++) 
    {
        if( fabs(f(x)) < fabs(f(minx)) )
            minx = x;
    }
    printf("\nFind root for %d steps\n",stepcount); //статистика
    return minx;
}

//------------------------------------------------------ 
// Функция поиска корня методом деления отрезка пополам
//------------------------------------------------------
float rootFindDiv(float xl, float xr, float eps, function f) 
{
    int stepcount=0; //число шагов
    float xm;
    while(fabs(xr-xl)>eps) //вещественный модуль разницы или floatabs
    { 
        stepcount++;
        xm=(xl+xr)/2; // середина отрезка
        if(signF(xl,f) != signF(xm,f)) //если знак отличается
            xr=xm;
        else
            xl=xm;
    }
    printf("Find root for %d steps\n",stepcount); //статистика
    return (xl+xr)/2;
} 

//------------------------------------------------------ 
// Модифицированная функция поиска корня
//------------------------------------------------------
float rootFindDiv2(float xl, float xr, float eps, function f)
{
    int stepcount=0; //число шагов
    float xm;
    while(fabs(xr-xl)>eps) //вещественный модуль разницы
    {
        stepcount++;
        xm=(xl+xr)/2; // середина отрезка
        if(f(xr)==0)  // нашли решение на правой границе
        {
            printf("Find root for %d steps\n",stepcount);
            return xr;
        }
        if(f(xl)==0) // нашли решение на левой границе
        {
            printf("Find root for %d steps\n",stepcount);
            return xl;
        }
        if(signF(xl,f) != signF(xm,f)) //если знак отличается
            xr=xm;
        else
            xl=xm;
    }
    printf("Find root for %d  steps\n",stepcount); //статистика
    return (xl+xr)/2;
}

//------------------------------------------------------ 
// Метод хорд
//------------------------------------------------------
float rootFindChord(float xl, float xr, float eps, function f)
{
    int stepcount=0;
    while(fabs(xr - xl) > eps) 
    {
        xl = xr - (xr - xl) * f(xr) / (f(xr) - f(xl));
        xr = xl - (xl - xr) * f(xl) / (f(xl) - f(xr));
        stepcount++;
    }
    printf("Find root for %d steps\n",stepcount);
    return xr;
}

//------------------------------------------------------ 
// Метод касательных (Ньютона)
//------------------------------------------------------
float rootFindTangent(float xn, float eps, function f, function df)
{
    float x1 = xn - f(xn)/df(xn);
    float x0 = xn;
    int stepcount=0;
    while(fabs(x0-x1) > eps)
    {
        x0 = x1;
        x1 = x1 - f(x1)/df(x1);
        stepcount++;
    }
    printf("Find root for %d steps\n",stepcount);
    return x1;
}

int main(void) 
{

    float points[4][2] = {{-2, -1.5},{-1.5, -1},{-1, 0.5},{-0.5, 0}};
    for(int i=0;i<4;i++)
    {
        printf("------------------Root%d----------------------\n",i);

        printf("Line Search root1 = %f\n", rootFindLineSearch(points[i][0],points[i][1],ACCURACY,f));
        //Find root for 10011 steps  Line Search root1 = -1.923878
        printf("Find Div Search root1 = %f\n", rootFindDiv(points[i][0],points[i][1],ACCURACY,f));
        //Find Div Search root for 13 steps Find Div Search root1 = -1.923859
        printf("Find Div2 Search root1 = %f\n", rootFindDiv2(points[i][0],points[i][1],ACCURACY,f));
        //Find root for 13  steps Find Div2 Search root1 = -1.923859
        printf("Find Chord Search root = %f\n", rootFindChord(points[i][0],points[i][1],ACCURACY,f));
        //Find root for 3  steps Find Chord Search root = -1.382683
        printf("Find Find Tangent root = %f\n", rootFindTangent(points[i][0],ACCURACY,f,df));
        //Find root for 3  steps Find Find Tangent root = -1.923879
    }

    return 0;
}