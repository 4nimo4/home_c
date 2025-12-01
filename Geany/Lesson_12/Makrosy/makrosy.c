/*
    Макросы

Макросы — это идентификаторы, определяемые с помощью
директивы #define, которые предполагают замену на
определенную последовательность символов.
*/
//Debug print - вывод номера строки в котрой ошибка
#include <stdio.h>
#define DEBUGPRINT fprintf (stderr,"debug in %d line\n", __LINE__)

int main(void)
{
   DEBUGPRINT;

   DEBUGPRINT;

   DEBUGPRINT;
    return 0; 
}