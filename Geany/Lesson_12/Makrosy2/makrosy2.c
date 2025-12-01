/*
    Makrosy2
    
Если в ходе макроподстановки получившаяся
строка снова содержит известные препроцессору
имена макросов, то к этим именам рекурсивно
применяется макроподстановка до получения
строки, не содержащей имена макросов.
*/
/*
#include <stdio.h>
#define ONE printf ("DEBUG\n")
#define TWO ONE; ONE
#define FOUR TWO; TWO

int main(void)
{
    FOUR;
    return 0;
}
*/
#include <stdio.h>
#define A B + 1
#define B 2

int main(void)
{
    int a;
    a = A + 2;
    printf("a = %d\n", a);
    return 0;
}