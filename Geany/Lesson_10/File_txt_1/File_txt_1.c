/*
 * File_txt_1.c
 * 
 * Примеры работы с текстовыми файлами
 
 * Дан текстовый файл in.txt, содержащий целые числа.
	Посчитать сумму чисел  в в текстовом файле
 * 
 */


#include <stdio.h>

int main()
{
	FILE *f;
	int sum = 0, n;
	f = fopen("/home/mikhail/home_c/Geany/Lesson_10/File_1/in.txt", "r");
	while (fscanf (f, "%d", &n) == 1)
		{
			sum += n;
		}
	fclose (f);
	printf ("%d\n", sum);
	return 0;
}

