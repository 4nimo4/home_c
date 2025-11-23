/*
 * funkc_modul_chisla.c
 * 
	Составьте функцию, модуль числа и приведите пример её
	использования
 */
#include <stdio.h>

//--------------------------------------------
// 1 Вариант
//--------------------------------------------
/*
int abs(int num)
{
	return (num<0)?-num:num;
}

int main()
{
	int num;
	scanf("%d",&num);
	printf("%d",abs(num));
	return 0;
}
*/
//--------------------------------------------
// 2 Вариант
//--------------------------------------------
int abs(int num)
{
	if (num<0)
		{
			return -num;
		}
	else
		{
			return num;
		}
}

int main()
{
	int num;
	scanf("%d",&num);
	printf("%d",abs(num));
	return 0;
}
