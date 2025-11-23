/*
 * Cikl_for.c
 * 
 * 	
 * 
 * Цикл с известным числом шагов for
	Цикл for является самым мощным видом цикла в Си.
	
	● Перед началом выполнения цикла вычисляется выражение expression1
	● Тело цикла – оператор statement – выполняется до тех пор, пока
	значение выражения expression2 оказывается не ложным
	● После окончания итерации цикла вычисляется выражение expression3
	
	for (expression1; expression2; expression3) statement;
	
	Внимание! Любое из выражений expression , а также оператор statement ,
	может быть пустым.
	
	
 * 
 * 
 */


#include <stdio.h>

int main(int argc, char **argv)
{
	/*
	int i, a=1;
	for(i=1; i<4; i++) 
		a++;
	printf("%d\n", a);
	
	int s, b=1;
	for(s=1; s<4; s++) 
		b = b+s;
	printf("%d\n", b);
	
	int e, k=1, l=2;
	for(e=3; e>=1; e--)
		k+=l; 
	printf("%d\n", k);
	*/
	
	// можно использовать область видимости переменных и одни и теже переменные
	// Но это не очень красивый код
	{
	int i, a=1;
	for(i=1; i<4; i++) 
		a++;
	printf("%d\n", a);
	}
	
	{
	int i, a=1;
	for(i=1; i<4; i++) 
		a += i;
	printf("%d\n", a);
	}
	
	{
	int i, a=1, b=2;
	for(i=3; i>=1; i--)
		a+=b; 
	printf("%d\n", a);
	}
	return 0;
}

