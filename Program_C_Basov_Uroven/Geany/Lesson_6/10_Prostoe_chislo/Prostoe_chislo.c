/*
 * Prostoe_chislo.c
 * 
	Опишите функцию, проверяющую, является ли
	целое неотрицательное число n простым. В случае
	положительного ответа функция возвращает
	значение 1, в противном случае — 0
 */
#include <stdio.h>

int prime(int n)
{
//	int i=2;
//	while (i*i<=n)
	for(int i=2;i*i<=n; i++)
		{
			if (n%i==0)
			return 0;
		//	i++;
		}
 return 1;
}

int main()
{
	int num;
	scanf("%d",&num);
	prime(num) ? printf("Prime") : printf("Not Prime");
	return 0;
}

