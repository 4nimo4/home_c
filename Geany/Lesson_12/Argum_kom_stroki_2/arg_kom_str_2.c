/*
Программа должна принимать два аргумента:
-h - вызов подсказки
-s - с числовым параметром
//-----------------------------------------------
В курсовом будет:
-m - номер месяца
-f - имя файла (строка)
-h - help -выдача подсказки
//-------------------------------------------

посмотри функцию sscanf

вводим:
    gcc arg_kom_str_2.c
    ./a.out -h
получаем:
    argc = 0, argv = ./a.out
    argc = 1, argv = -h
    HELP
вводим:
    ./a.out -s 10 
получаем:
    argc = 1, argv = -s
    10 10 2
    argc = 2, argv = 10
*/

#include <stdio.h>
int main(int argc, char *argv[])
{
    int size=0;
    for(int i=0; i<argc; i++)
        {
            printf("argc = %d, argv = %s\n", i, argv[i]);
            char* str = argv[i];
            if(str[0]=='-')//если первый символ в строке '-'
                {
                    switch (str[1])//если второй символ в строке 'h'или 's'
                        {
                            case 'h'/* constant-expression */:
                                 /* code */
                                printf("HELP\n");
                            break;
                            case 's':
                                sscanf(argv[i+1],"%d",&size); 
                                //sscanf вытаскивает число из строки
                                printf("%d %s %d\n",size,argv[i+1],i+1);
                            break;
                            default:
                            break;
                        }
                }
        }
    return 0;
}