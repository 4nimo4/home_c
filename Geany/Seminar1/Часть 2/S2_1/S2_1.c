/*
 * S1_1.c

 * 
 */


#include <stdio.h>

int main(void)
{
	int x;
	scanf("%d", &x);
	
	printf("%s", (x == 0) ? "NO\n" : "YES\n");
	printf("%s", (!x ) ? "NO\n" : "YES\n"); //другая запись условия х=0
	
	x == 0 ? printf("NO\n") : printf("YES\n");
	!x ? printf("NO\n") : printf("YES\n");//другая запись условия х=0
	
	x != 0 ? printf("YES\n") : printf("NO\n"); // Условие x !=0 часто опускают	
	x ? printf("YES\n") : printf("NO\n");//Тогда следующая строка 
	return 0;
}

