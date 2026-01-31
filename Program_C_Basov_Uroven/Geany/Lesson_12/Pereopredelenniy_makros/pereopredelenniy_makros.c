/*
    Предопределенные макросы

Удобная штука, выводит информацию о программе, когда был скомпилирован файл

вот что выведет:

/*
File: pereopredelenniy_makros.c
Date:Q Nov 24 2025
Time: 09:58:46
Version: 202311
Line: 15
Function: main
Function: why_me
Line: 24
*/



#include <stdio.h>
void why_me();

int main(void)
{
    printf("File: %s\n", __FILE__);
    printf("Date:Q %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("Version: %ld\n",__STDC_VERSION__);
    printf("Line: %d\n", __LINE__);
    printf("Function: %s\n", __func__);
    why_me();
    return 0;
}

void why_me() 
{
    printf("Function: %s\n", __func__);
    printf("Line: %d\n", __LINE__);
}
