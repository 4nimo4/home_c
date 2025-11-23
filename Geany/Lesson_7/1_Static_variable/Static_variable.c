#include <stdio.h>
void func(void) 
{
	static int a=5; //статическая память
	a++;
	printf("a1 = %d\n",a);
	//printf("a2 = %d\n",a++);
	//printf("a3 = %d\n",++a);
}
int main(void)
{
	func();
	func();
	func();
	return 0;
}

