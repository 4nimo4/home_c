/*
//------------------------------------------------------ 
// Метод деления отрезка пополам
//------------------------------------------------------
Численный метод для решения нелинейных уравнений вида f(x)=0. Предполагается только
непрерывность функции f(x).
Поиск основывается на теореме о промежуточных значениях.
Алгоритм основан на следующем утверждении: если на отрезке [a,b] функция принимает
нулевое значение, то на концах отрезка функция должна быть противоположных знаков.
● Изначально необходимо найти приближение — задать a и b для исходного отрезка.
● Далее разделить отрезок пополам и взять ту из половинок, на концах которой функция
по-прежнему принимает значения противоположных знаков.
● Если значение функции в серединной точке оказывается искомым нулём, то процесс
завершается.

*/

#include <stdio.h>    // ввод-вывод (printf и др.)
#include <math.h>

#define ACCURACY 0.0001 //Точность с которой будем делать вычисления

typedef float(*function)(float);

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

//(-2, -1.5) (-1.5, -1) (-1, -0.5) (-0.5, 0)
float f(float x) 
{
    return 8*x*x*x*x + 32*x*x*x + 40*x*x + 16*x + 1;
}

int signF(float x,function f)
{
    return f(x)==0 ? 0 : (f(x) < 0 ? -1:+1);
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
    printf("Find Div Search root for %d steps\n",stepcount); //статистика
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
    }

    return 0;
}