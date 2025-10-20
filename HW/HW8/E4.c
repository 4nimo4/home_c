/*
 * E4.c
 * 
 *	Два максимума
 * 
 * Считать массив из 10 элементов и найти в нем два максимальных элемента
 *  и напечатать их сумму.
 * 
 * Формат входных данных
	10 целых чисел через пробел

	
	Формат результата
	Сумма двух максимальных элементов.


 * 
 */


#include <stdio.h>
#define SIZE 10
//Объявляем заголовки функций
int Input(int arr[], int);	//Функция заполнения массива данными с клавиатуры
void Print(int arr[], int);	//Функция вывода на индикацию массива данных
int Max_mensh(int arr[], int, int);	//Функция поиска следующего по убыванию элемента после max
int Max(int arr[], int);	//Функция поиска максимума


int arr[SIZE]={0};		//Создали массив и заполнили его нулями
								
int main() 
{
	int max, max_mensh, sum;
	Input(arr, SIZE);	// В функцию пердаем имя массива и количество элемонтов
//	Print(arr, SIZE);
	max = Max(arr,SIZE);
	max_mensh = Max_mensh(arr,SIZE,max);
	sum = max + max_mensh;
	printf("%d\n", sum);
	return 0;
}
//----------------------------------------------------------
//Функция заполнения массива данными с клавиатуры
int Input(int arr[], int n)
{
	int i;
	for(i=0; i<n; i++)
		{
			scanf("%d", &arr[i]);		
		}
	return i;
}
//----------------------------------------------------------
//Функция вывода на индикацию массива данных
void Print(int arr[], int n) 
{
	for(int i=0; i<n; i++)
		{
			printf("%d ", arr[i]);	
		}
	printf("\n");	
}
//--------------------------------------------------------------
//Функция поиска следующего по убыванию элемента после max
int Max_mensh(int arr[], int n, int max)
{
	int min_max, j=0;
	for(int i=0 ; i<n; i++)	//Проверим сколько цифр в массиве равны max
			// Если больше одной, то min_max = max; return min_max;	
		{
			if(arr[i] == max)
				{
					j++;
				}
		}
	if(j > 1)
		{
			
			min_max = max;
			return min_max;	
		}
	j=0;
	if(arr[j] == max)
		{
			j++;
		}
	min_max = arr[j];
//	printf("j=%d max=%d min_max=%d \n", j, max, min_max);
	printf("\n");
	for(; j<n; j++)
		{
			
			if(min_max < arr[j] && arr[j] < max)
				{
					min_max = arr[j];
				}
//		printf("j=%d max=%d min_max=%d \n", j, max, min_max);			
		}
	return min_max;	
}
//--------------------------------------------------------------
//Функция поиска максимума
int Max(int arr[], int n)
{
	int max=arr[0];
	for(int i=0; i<n; i++)
		{
			if(max < arr[i])
				{
					max = arr[i];
				}
		//printf("i=%d max=%d \n", i, max);			
		}
	return max;	
}


