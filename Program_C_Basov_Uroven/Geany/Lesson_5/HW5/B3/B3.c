/*
 * B3.c
	Сумма квадратов маленьких чисел
	
	Ввести два целых числа a и b (a ≤ b) и вывести сумму квадратов всех чисел от a до b.

	Input format	
	Два целых числа по модулю не больше 100
	
	Output format
	Сумма квадратов от первого введенного числа до второго


 */


#include <stdio.h>
#include <locale.h>


int main()
{
	int a, b, sum=0;
	setlocale(LC_ALL, "Rus");
	//printf("Введите два целых числа a и b (a ≤ b) от 1 до 100 \n");
	scanf("%d%d", &a, &b);
		if ((a<=0) || (a>100) || (b<=0) || (b>100) || (a>b))	
		{
		//	printf("Вы ввели недопустимое число! - Прощайте!\n");
			return 0;
		}
	for (int i=b-a+1; i!=0; i--)
		{
			sum += a*a;
			a++;
		}
	printf("%d ", sum);	
	//printf("\nПрограмма завершена\n");
	return 0;
}

