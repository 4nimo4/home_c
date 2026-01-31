/*
 * File_txt_3.c
 * 
 * Примеры работы с текстовыми файлами
 
 * Дан текстовый файл in.txt. Необходимо посчитать количество цифр в файле и
	записать это число в конец данного файла
 * 
 */


#include <stdio.h>

int main()
{
	FILE *f_my; 
	FILE *f_my2;// указательна переменную может быть и f_my например!
	int sum = 0;
	signed char c;// обязательно signed! иначе зациклится
	f_my = fopen("in.txt", "r+"); // режим чтение и дозапись
	f_my2 = fopen("in2.txt", "w"); // режим чтение и дозапись
	while ( (c=fgetc(f_my))!=EOF ) 
		{
			if(c>='0' && c<='9') //Если это цифра 
				{
					//printf ("%d ", c);
					sum += c-'0';// Добавим цифру к сумме
				}
		}
	fprintf (f_my, " %d", sum);
	printf (" %d\n", sum);
	fclose (f_my);
	fclose (f_my2);
	return 0;
}

