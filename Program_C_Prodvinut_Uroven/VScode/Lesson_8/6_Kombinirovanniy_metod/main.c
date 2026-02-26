/*
//------------------------------------------------------ 
// Комбинированный метод
//------------------------------------------------------
Идея
Методы хорд и касательных дают приближения корня с разных сторон.
Комбинированный метод сочетает в себе принципы метода хорд и метода
касательных, и позволяет решать нелинейные уравнения y(x)=0 с заданной
точностью ε.
Приближение к искомому корню происходит одновременно с двух сторон
отрезка, на котором отделен корень уравнения.
В данном методе один из концов отрезка не является закрепленным.
Для построения хорды используются значения приближений, полученные с
помощью метода касательных на предыдущей итерации.

Если f(xl)*f’’(xl) < 0, то проводим хорду:

Xl = Xl - (f(Xl) * (Xl - Xr)) / (f(Xl) - f(Xr))

иначе касательную

Xl = Xl - f(Xl) / (f'(Xl)
Xr = Xr - f(Xr) / (f'(Xr)

Суть комбинированного метода заключается в разбиении отрезка [a; b] (при
условии f(a)f(b) < 0) на три отрезка с помощью хорды и касательной и выборе
нового отрезка от точки пересечения хорды с осью абсцисс до точки
пересечения касательной с осью абсцисс, на котором функция меняет знак и
содержит решение.
● Построение хорд и касательных продолжается до достижения
необходимой точности решения ε.
● Условие начальной точки для метода хорд: f(x)fʼʼ(x) < 0.
● Условие начальной точки для метода касательных: f(x)fʼʼ(x) > 0.

Сначала находим такой отрезок [a; b], что функция f(x) дважды непрерывно
дифференцируема и меняет знак на отрезке, то есть f(a)f(b) < 0.
Для реализации этого метода потребуется вычислить вторую производную
для функции. В нашем примере:
f(x) = 8x^4 + 32x^3 + 40x^2 + 16x + 1
fʼ(x) = 32x^3 + 96x^2 + 80x+16
fʼʼ(x) = 96x^2 + 192x+80

*/

#include <stdio.h>    // ввод-вывод (printf и др.)
#include <math.h>

#define ACCURACY 0.001 //Точность с которой будем делать вычисления
#define EPS 0.0001

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
//Для Комбинированного метода
//------------------------------------------------------
float ddf(float x) 
{
    return 96*x*x + 192*x + 80;
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

//------------------------------------------------------ 
// Комбинированный метод
//------------------------------------------------------
float rootFindCombine(float xl, float xr, float eps, function f, function df, function ddf ) 
{
    int stepcount=0;
    while(fabs(xl-xr) > 2*eps) 
    {
        if( f(xl)*ddf(xl)<0 )
            xl = xl - (f(xl)*(xl - xr))/(f(xl) - f(xr));
        else
            xl = xl - f(xl)/df(xl);
        if( f(xr)*ddf(xr)<0 )
            xr = xr - (f(xr)*(xr - xl))/(f(xr) - f(xl));
        else
            xr = xr - f(xr)/df(xr);
        stepcount++;
    }
    printf("Find root for %d steps\n",stepcount);
    return(xl+xr)/2;
}


int main(void) 
{

    const float points[4][2] = {{-2, -1.6},{-1.5, -1},{-0.7, -0.5},{-0.16, -0.001}};
    const float eps = EPS;
    //float points[4][2] = {{-2, -1.5},{-1.5, -1},{-1, 0.5},{-0.5, 0}};
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
        printf("Find Combine Search root = %f\n", rootFindCombine(points[i][0],points[i][1],eps,f,df,ddf));
        //Find root for 3  steps Find Combine Search root = -1.923879
    }

    return 0;
}