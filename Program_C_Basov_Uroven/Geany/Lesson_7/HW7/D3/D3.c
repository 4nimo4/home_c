/*
 * D3.c
 * 
	В обратном порядке
	
	Дано целое не отрицательное число N. Выведите все его цифры по одной, 
	в обратном порядке, разделяя их пробелами или новыми строками.

	Формат входных данных
	Одно не отрицательное целое число
	
	Формат результата
	Последовательность цифр введенного числа в обратном порядке через пробел
 * 
 * 
 */


#include <stdio.h>
 
void vivod_cifr_probel(int);
 
int main()
{
	int a;
    scanf("%d", &a);
    if (a == 0)
		{	
			printf("%d", a);
			return 0;		
		}
	vivod_cifr_probel(a); 
	return 0;
}

    
  

void vivod_cifr_probel(int b) 
{	
	int d=0;	
    if (b > 0)
		{	
			d = b%10;
			b /= 10;
			printf("%d ", d);
			vivod_cifr_probel(b);		
		}	
 }      

