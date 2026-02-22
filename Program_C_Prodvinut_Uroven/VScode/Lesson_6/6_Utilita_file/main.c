/*
//------------------------------------------------------
//  Исследуем исполняемый файл - prog
//------------------------------------------------------ 
//      Утилита file
//------------------------------------------------------ 
После того как исходный файл был скомпилирован в объектный файл или
исполняемый файл, параметры, используемые для его компиляции,
больше не очевидны.
Команда file просматривает содержимое объектного файла или
исполняемого файла и определяет некоторые из его характеристик,
например, был ли он скомпилирован с динамической или статической
компоновкой.
вводим команды:

make clean
make
file prog

вывод:
prog: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, 
interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=9e333223d348c32196463a7b25f36562172b5e66, 
for GNU/Linux 3.2.0, not stripped

not stripped - 

*/
#include <stdio.h>   
#include <inttypes.h> 

double pown (double d, unsigned n)
{
    double x = 1.0;
    for (size_t j = 1; j <= n; j++)
    {
        x *= d;
    }
    return x;
}

int main(void)
{
    double sum = 0.0;

    for (size_t i = 1; i <= 0xfffffff; i++)
    {
        sum += pown (i, i % 5);
    }
    printf ("sum = %g\n", sum);

    return 0;
}