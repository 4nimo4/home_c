/*
 * File_txt_2.c
 * 
 * Примеры работы с текстовыми файлами
 
 * Ввести имя файла и напечатать его размер
 * 
 */


#include <stdio.h>

int main()
{
	FILE *f;
	char filename[100]={0};
	size_t size;
	printf("Input file name: ");
	scanf("%s",filename); //Считали строку
	f = fopen (filename, "r"); // Открыли файл на чтение
	if (f != NULL) 
		{
	//Установим метку в конец файла:	
			fseek (f, 0, SEEK_END); //Если открылся, то идем в конец файла
	//ftell - посчитает семещение в байтах относительно начала файла до ранее //установленной метки:
			//Функция ftell возвращает значение
			//указателя текущего положения потока	
			size = ftell (f); //считали размер в байтах в переменную size
			fclose (f); // закрываем файл
			printf ("File size of '%s' - %lu bytes.\n", filename, size);
		} 
	else 
		{
			printf ("Can't open file %s\n", filename); // Если файл неоткрылся, то выходим
		}
	return 0;
}


