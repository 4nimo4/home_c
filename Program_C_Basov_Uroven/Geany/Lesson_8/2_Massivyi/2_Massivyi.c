/*
 * 2_Massivyi.c
 * 

 * 
 */


#include <stdio.h>
#define N 15
int main() 
{
	//int n=100;
	//scanf ("%d", &n);// ввод количества элементов массива
	int arr[N]={0};
	int i=8; 
	arr[i++]=12; // 8 ой элемент
	arr[++i]=15;	// тут уже I=9 = 12 и мы увеличиваем ++ на 1, значит 
					// это уже будет 10 й элемент массива =15
	for(int j=0; j<N; j++)
		{
			printf("%d ", arr[j]);//  
		}

	return 0;
}

