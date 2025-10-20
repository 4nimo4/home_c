/*
 * C9.c
 * 
	Факториал
	
	Составить функцию вычисления N!. Использовать ее при вычислении факториала int factorial(int n)

	Формат входных данных
	Целое положительное число не больше 20

	Формат результата
	Целое положительное число

 * 
 * 
 */
int factorial(int);

#include<stdio.h>
int main() 
{
    int f, n;
    scanf("%d",&n);
    f = factorial(n);
    	
    printf("%d\n", f);
    return 0;
}

int factorial(int a)
{
	int factorial = 1; 
    if(a>=0 || a<=20)
		{
			for(int i=1; i<=a; i++) 
				{
					factorial *= i;
				}
		}
	return factorial;
}
