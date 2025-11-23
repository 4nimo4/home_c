/*
 * Uslovnyi_operator.c
	Условный оператор имеет вид:
	● if (expr) stmt; else stmt;,
	причем нужно отметить, что:
	ветка else всегда относится к ближайшему if
	● if (expr) stmt;
	● else if (expr) stmt;
	● else if (expr) stmt;
	● else stmt;
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{
	int a = 5, b = 7;
	printf("%d\n", (a += 5) * (b -=3));
	printf("a=%d,b=%d\n", a, b);
	printf("%d\n", --b / (a++ - 3));
	printf("a=%d,b=%d\n", a, b);
	printf("%d\n", (a -= 2) || (b - 7));
	printf("a=%d,b=%d\n", a, b);
	printf("%d\n", (a *= b) + (b *= a)); // a=9, b=3
	printf("a=%d,b=%d\n", a, b); // неопределенность
	return 0;
}

