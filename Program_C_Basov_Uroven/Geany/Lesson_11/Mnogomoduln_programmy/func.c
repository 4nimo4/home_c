#include <stdio.h>
#include "func.h"

 extern int m; //Глобальная переменная
int max(int a, int b)
{
    m = 5;
    printf("m=%d\n", m);
    m = 33;
    return a>b? a : b;
}