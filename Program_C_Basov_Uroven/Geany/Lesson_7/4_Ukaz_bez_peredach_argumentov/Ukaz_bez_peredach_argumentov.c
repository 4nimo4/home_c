/*
 * Ukaz_bez_peredach_argumentov.c
 * 
 * 	n = 7
	m = 5
	Параметры функции swap передаются по значению и, следовательно, поменялись
	местами только копии параметров n и m внутри функции, при этом никакого
	влияния на сами фактические параметры n и m не произошло
 */


#include <stdio.h>
void swap(int a, int b) 
{
	int tmp;
	tmp = a;
	a = b;
	b = tmp; 
}
int main(void)
{
	int n=7, m=5;
	swap(n, m); // Передаются значения
	printf("n = %d m = %d\n",n,m);
}
