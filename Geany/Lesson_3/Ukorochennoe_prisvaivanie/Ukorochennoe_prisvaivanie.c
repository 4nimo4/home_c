/*
 * Ukorochennoe_prisvaivanie.c

 */


#include <stdio.h>

int main()
{
	int a = 50, b = 7;
	a = a + b; // a=57
	a += b; // a=57 , - тоже самое
	
	// Инкремент ++
	// Декрементт --
	
	// постфиксная форма 
	int c, d=7;
	c = d++; // c=7 d=8
	
	// префиксная форма
	int e, f =7;
	e = ++f; // e=8 f=8
	
	return 0;
}

