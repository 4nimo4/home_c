/*
 * S2_2.c
 * Статические переменные и указатели

 * 
 * 
 */


#include <stdio.h>
void func(void) 
{
	//int a=5; //локальная переменная , всегда будет 6, 6, 6
	static int a=5; //локальная переменная ,  будет 6, 7, 8
	a++;
	printf("a = %d\n",a);
}
	int main(void)
{
	func();
	func();
	func();
 return 0;
}


