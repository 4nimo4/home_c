/*
 * Ukazatel_na_ukazatel.c
 * 
 * Указатель на указатель

 */


//Можно объявлять указатели на указатели:
#include <stdio.h>
int main() 
{
	int a = 77;
	int *ptrA = &a;
	int** ppA = &ptrA;
	printf("%d\n",a);
	*ptrA = 88;
	printf("%d\n",a);// << std::endl; // 88
	**ppA = 99;
	printf("%d\n",a);// << std::endl; // 99
	return 0;
}

