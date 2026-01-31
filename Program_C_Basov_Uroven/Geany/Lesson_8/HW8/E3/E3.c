/*
 * E3.c
 * 
 *	Максимум и минимум
 * 
 * Считать массив из 10 элементов и найти в нем максимальный 
 * и минимальный элементы и их номера.
 * 
 * Формат входных данных
	10 целых чисел через пробел

	
	Формат результата
	4 целых числа через пробел: номер максимума, максимум, номер минимума, минимум.

 * 
 */


#include <stdio.h>
#define SIZE 10
//Объявляем заголовки функций
int Input(int arr[], int);	//Функция заполнения массива данными с клавиатуры
void Print(int arr[], int);	//Функция вывода на индикацию массива данных
int Min(int arr[], int);	//Функция поиска минимума
int Max(int arr[], int);	//Функция поиска максимума
int PosMin(int arr[], int);	//Функция поиска позиции минимального элемента в массиве
int PosMax(int arr[], int);	//Функция поиска позиции максимального элемента в массив

int arr[SIZE]={0};		//Создали массив и заполнили его нулями
								
int main() 
{
	int max, min, posmax, posmin;
	Input(arr, SIZE);	// В функцию пердаем имя массива и количество элемонтов
//	Print(arr, SIZE);
	min = Min(arr,SIZE);
	max = Max(arr,SIZE);
	posmin = PosMin(arr,SIZE);
	posmax = PosMax(arr,SIZE);
	printf("%d %d %d %d\n", posmax, max, posmin, min);
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
//Функция поиска минимума
int Min(int arr[], int n)
{
	int min=arr[0];
	for(int i=0; i<n; i++)
		{
			if(min > arr[i])
				{
					min = arr[i];
				}
		//printf("i=%d min=%d \n", i, min);			
		}
	return min;	
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
//--------------------------------------------------------------
//Функция поиска позиции минимального элемента в массиве
int PosMin(int arr[], int n) 
{
	int min=arr[0], posmin=1;
	for(int i=0; i<n; i++)
		{
			if(min > arr[i])
				{
					min = arr[i];
					posmin = i;
					posmin++;
				}
//			printf("i=%d arr[i]=%d min=%d posmin=%d \n", i, arr[i], min, posmin);			
		}
	printf("\n");	
	return posmin;	
}
//--------------------------------------------------------------
//Функция поиска позиции максимального элемента в массиве
int PosMax(int arr[], int n)
{
	int max=arr[0], posmax=1;
	for(int i=1; i<n; i++)
		{
			if(max < arr[i])
				{
					max = arr[i];
					posmax = i;
					posmax++;
				}
//		printf("i=%d arr[i]=%d max=%d posmax=%d \n", i, arr[i], max, posmax);		
		}
	return posmax;	
}
//--------------------------------------------------------------

