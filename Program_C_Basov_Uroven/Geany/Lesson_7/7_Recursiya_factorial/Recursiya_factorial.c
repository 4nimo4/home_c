/*
 * Recursiya_factorial.c
 * 
	Вычисление факториала
	
	Факториал можно описать итерационно:
 
unsigned int factorial(unsigned int n) 
{
	unsigned int i, fact=1;
	for(i=2;i<=n;i++)
	fact*=i;
	return fact;
}
*/
//----------------------А здесь рекурсивно!!!---------------------------
#include <stdio.h>

unsigned int factorial(unsigned int);

int main() 
{
	factorial(3);
	return 0;
}
/*
 factorial(3) 
 {
	if(3<=1)
	return 1;
	return 3*factorial(2);
}

factorial(2) 
 {
	if(2<=1)
	return 1;
	return 2*factorial(1);
}

factorial(1) 
 {
	if(1<=1)
	return 1;
}
 */
unsigned int factorial(unsigned int n) 
{
	printf("%d\n",n);
	if(n<=1) // Условие остановки
	return 1;
	int _f = n * factorial(n-1);
	printf("%d*factorial(%d)=%d\n",n,n-1,_f);
	return _f; // Шаг
}
