
/*
    Директивы #line и #error

#line позволяет изменить номер строки компилятора и (при необходимости) имя файла,
выводимого для ошибок и предупреждений.
*/

#include <stdio.h>
#line 100 "help.c"
#ifndef __APPLE__
#error This code work only in MacOs
#endif

int main(void)
{
    int a=5, b=7, c;
    c = MAX(a,++b) ;
    printf("c = %d\n", c);// типа с = ((а) >= (++b)?(a):(++b))
    return 0;
}