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

#include <stdio.h>    // стандартный ввод-вывод (printf и др.)
#include <math.h>     // математические функции: fabs, и т.п.

#define ACCURACY 0.001 // Точность для большинства методов (ε по x)
#define EPS 0.0001     // Отдельная точность для комбинированного метода
#define MAX_ITER 1000000
#define NYUT_ETALON 1e-8f;  // высокая точность "Эталонног" метода Ньютона

typedef float(*function)(float); 
// Тип "function" — указатель на функцию вида float f(float)


//------------------------------------------------------
// Целевая функция f(x), корни которой ищем
//------------------------------------------------------
float f(float x) 
{
    return 8*x*x*x*x + 32*x*x*x + 40*x*x + 16*x + 1;
}

//------------------------------------------------------
// Функция знака для f(x):
//  0  — если f(x) == 0
// -1  — если f(x) < 0
// +1  — если f(x) > 0
//------------------------------------------------------
int signF(float x, function f)
{
    return f(x) == 0 ? 0 : (f(x) < 0 ? -1 : +1);
}

//------------------------------------------------------
// Первая производная f'(x)
// f(x)  = 8x^4 + 32x^3 + 40x^2 + 16x + 1
// f'(x) = 32x^3 + 96x^2 + 80x + 16
//------------------------------------------------------
float df(float x)
{
    return 32*x*x*x + 96*x*x + 80*x + 16;
}

//------------------------------------------------------
// Вторая производная f''(x)
// f'(x)  = 32x^3 + 96x^2 + 80x + 16
// f''(x) = 96x^2 + 192x + 80
//------------------------------------------------------
float ddf(float x) 
{
    return 96*x*x + 192*x + 80;
}

//------------------------------------------------------ 
// Линейный поиск (грубый перебор точек на отрезке)
//------------------------------------------------------
float rootFindLineSearch(float xl, float xr, float eps, function f) 
{
    float x;
    float minx = xl;       // текущая лучшая точка (минимальный |f|)
    float nextstep;

    // Длину интервала делим примерно на (1/eps) шагов.
    nextstep = fabs(xr - xl) / (1 / eps);

    int stepcount = 0;     // счётчик шагов

    for (x = xl; x < xr; x += nextstep, stepcount++) 
    {
        if (fabs(f(x)) < fabs(f(minx)))
            minx = x;
    }

    printf("Find root for %d steps\n", stepcount); // статистика
    return minx;  // точка, где |f(x)| минимально на интервале
}

//------------------------------------------------------ 
// Метод деления отрезка пополам (бисекция)
//------------------------------------------------------
float rootFindDiv(float xl, float xr, float eps, function f) 
{
    int stepcount = 0; 
    float xm;          

    while (fabs(xr - xl) > eps) 
    { 
        stepcount++;
        xm = (xl + xr) / 2.0f;

        if (signF(xl, f) != signF(xm, f))
            xr = xm;
        else
            xl = xm;
    }

    printf("Find root for %d steps\n", stepcount);
    return (xl + xr) / 2.0f;
} 

//------------------------------------------------------ 
// Модифицированная бисекция
//------------------------------------------------------
float rootFindDiv2(float xl, float xr, float eps, function f)
{
    int stepcount = 0; 
    float xm;

    while (fabs(xr - xl) > eps)
    {
        stepcount++;
        xm = (xl + xr) / 2.0f;

        if (f(xr) == 0)
        {
            printf("Find root for %d  steps\n", stepcount);
            return xr;
        }
        if (f(xl) == 0)
        {
            printf("Find root for %d  steps\n", stepcount);
            return xl;
        }

        if (signF(xl, f) != signF(xm, f))
            xr = xm;
        else
            xl = xm;
    }

    printf("Find root for %d  steps\n", stepcount);
    return (xl + xr) / 2.0f;
}

//------------------------------------------------------ 
// Метод хорд (секущих)
//------------------------------------------------------
float rootFindChord(float xl, float xr, float eps, function f)
{
    int stepcount = 0;

    while (fabs(xr - xl) > eps) 
    {
        xl = xr - (xr - xl) * f(xr) / (f(xr) - f(xl));
        xr = xl - (xl - xr) * f(xl) / (f(xl) - f(xr));
        stepcount++;
    }

    printf("Find root for %d steps\n", stepcount);
    return xr;
}

//------------------------------------------------------ 
// Метод касательных (Ньютона)
// xn — начальное приближение
//------------------------------------------------------
float rootFindTangent(float xn, float eps, function f, function df)
{
    float x1 = xn - f(xn)/df(xn);
    float x0 = xn;
    int stepcount = 0;

    while (fabs(x0 - x1) > eps)
    {
        x0 = x1;
        x1 = x1 - f(x1)/df(x1);
        stepcount++;
    }

    printf("Find root for %d steps\n", stepcount);
    return x1;
}

//------------------------------------------------------ 
// Комбинированный метод (хорды + Ньютон)
//------------------------------------------------------
float rootFindCombine(float xl, float xr, float eps,
                      function f, function df, function ddf) 
{
    int stepcount = 0;

    while (fabs(xl - xr) > 2*eps) 
    {
        if (f(xl) * ddf(xl) < 0)
            xl = xl - (f(xl) * (xl - xr)) / (f(xl) - f(xr));
        else
            xl = xl - f(xl)/df(xl);

        if (f(xr) * ddf(xr) < 0)
            xr = xr - (f(xr) * (xr - xl)) / (f(xr) - f(xl));
        else
            xr = xr - f(xr)/df(xr);

        stepcount++;
    }

    printf("Find root for %d steps\n", stepcount);
    return (xl + xr) / 2.0f;
}

//------------------------------------------------------ 
// "Эталонный" метод Ньютона с очень маленьким eps
// Используется только для получения более точного корня
// и сравнения ошибок остальных методов
//------------------------------------------------------
float newtonHighPrecision(float x0, function f, function df)
{
    const float eps = NYUT_ETALON;  // высокая точность
    float x1 = x0 - f(x0)/df(x0);
    float x_prev = x0;
    int iter = 0;

    while (fabs(x1 - x_prev) > eps && iter < MAX_ITER)
    {
        x_prev = x1;
        x1 = x1 - f(x1)/df(x1);
        iter++;
    }

    if (iter == MAX_ITER) {
        printf("WARNING: newtonHighPrecision did not converge from x0 = %f\n", x0);
    }

    return x1;
}

int main(void) 
{
    // Точные отрезки, на которых сидят корни
    const float points[4][2] = {
        { -2.0f,  -1.6f   },
        { -1.5f,  -1.0f   },
        { -0.7f,  -0.5f   },
        { -0.16f, -0.001f }
    };

    const float eps = EPS;  // точность для комбинированного метода
    float x_true[4];

    // Сначала находим "эталонные" корни с высокой точностью
    for (int i = 0; i < 4; i++)
    {
        float mid = (points[i][0] + points[i][1]) / 2.0f;
        x_true[i] = newtonHighPrecision(mid, f, df);
    }

    // Теперь запускаем все методы и сравниваем с эталоном
    for (int i = 0; i < 4; i++)
    {
        printf("Точность для большинства методов (ε по x) = %f\n", ACCURACY);
        printf("Точность для комбинированного метода (ε по x) = %f\n\n", EPS);
        
        printf("------------------Root%d----------------------\n", i);
        printf("Эталонный корень (Ньютон, eps=1e-8) ≈ %.10f\n\n", x_true[i]);

        // 1. Линейный поиск
        float xr = rootFindLineSearch(points[i][0], points[i][1], ACCURACY, f);
        printf("Line Search root  = %.6f, |error| = %.10f\n",
               xr, fabs(xr - x_true[i]));

        // 2. Бисекция
        xr = rootFindDiv(points[i][0], points[i][1], ACCURACY, f);
        printf("Bisection root    = %.6f, |error| = %.10f\n",
               xr, fabs(xr - x_true[i]));

        // 3. Модифицированная бисекция
        xr = rootFindDiv2(points[i][0], points[i][1], ACCURACY, f);
        printf("Bisection2 root   = %.6f, |error| = %.10f\n",
               xr, fabs(xr - x_true[i]));

        // 4. Метод хорд
        xr = rootFindChord(points[i][0], points[i][1], ACCURACY, f);
        printf("Secant root       = %.6f, |error| = %.10f\n",
               xr, fabs(xr - x_true[i]));

        // 5. Метод Ньютона (касательных)
        xr = rootFindTangent(points[i][0], ACCURACY, f, df);
        printf("Newton root       = %.6f, |error| = %.10f\n",
               xr, fabs(xr - x_true[i]));

        // 6. Комбинированный метод
        xr = rootFindCombine(points[i][0], points[i][1], eps, f, df, ddf);
        printf("Combined root     = %.6f, |error| = %.10f\n\n",
               xr, fabs(xr - x_true[i]));
    }

    return 0;
}