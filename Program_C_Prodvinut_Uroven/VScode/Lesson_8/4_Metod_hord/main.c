/*
//------------------------------------------------------ 
// Метод хорд
//------------------------------------------------------
➢ Выберем две начальные точки C1(x1,y1) и C2(x2,y2) и проведём через них прямую. 
Она пересечёт ось абсцисс в точке (x3,0).
➢ Теперь найдём значение функции y3 = f(x3). Временно будем считать его корнем 
на отрезке [x1,x2].
➢ Вместо точек С1 и С2 мы возьмём точку С3(x3,y3) и точку С2(x2,y2).
➢ Теперь с этими двумя точками проделаем ту же самую операцию и продолжим алгоритм, 
то есть будем получать две точки Cn+1(xn+1,yn+1) и Cn(xn,yn) и повторять операцию с ними.
➢ Отрезок, соединяющий последние две точки, пересекает ось абсцисс в точке, 
значение абсциссы которой можно приближённо считать корнем.
➢ Эти действия нужно повторять до тех пор, пока не получим значение корня 
с нужной погрешностью ε.

Находим следующую точку по формуле

Xi+1 = Xi - (f(Xi) * (Xi - X0)) / (f(Xi) - f(X0))

Выбираем отрезок с учетом знака функции на концах

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
        printf("Find Chord Search root = %f\n", rootFindChord(points[i][0],points[i][1],0.001,f));
        //Find root for 5  steps Find Div2 Search root1 = -1.382683
    }

    return 0;
}