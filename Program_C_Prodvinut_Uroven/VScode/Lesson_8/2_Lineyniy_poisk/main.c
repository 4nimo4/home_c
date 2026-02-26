/*
//------------------------------------------------------ 
// Линейный поиск
//------------------------------------------------------
Алгоритм является простейшим алгоритмом поиска.
В отличие от, например, двоичного поиска, не накладывает никаких ограничений
на функцию и имеет простейшую реализацию.
Поиск значения функции осуществляется по следующему алгоритму:
● Простым сравнением очередного рассматриваемого значения с нулем
● Поиск происходит слева направо, то есть от меньших значений аргумента к
большим
● Если значения совпадают (с той или иной точностью) с нулем, то поиск
считается завершённым.

*/

#include <stdio.h>    // ввод-вывод (printf и др.)
#include <math.h>

typedef float(*function)(float);

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

//(-2, -1.5) (-1.5, -1) (-1, -0.5) (-0.5, 0)
float f(float x) 
{
    return 8*x*x*x*x + 32*x*x*x + 40*x*x + 16*x + 1;
}

int main(void) 
{
    printf("Line Search root1 = %f\n",rootFindLineSearch(-2,-1.5,0.001,f)); //Line Search root1 = -1.924006
    printf("Line Search root2 = %f\n",rootFindLineSearch(-1.5,-1,0.001,f)); //Line Search root2 = -1.382509
    printf("Line Search root3 = %f\n",rootFindLineSearch(-1,-0.5,0.001,f)); //Line Search root3 = -0.617482
    printf("Line Search root4 = %f\n",rootFindLineSearch(-0.5,0, 0.001,f)); //Line Search root4 = -0.076005

    return 0;
}