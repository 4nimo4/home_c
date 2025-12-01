
#include <stdio.h>
//#include <stdint.h>
//#include <string.h>
//#include <inttypes.h>
//#include <stdlib.h>

#include "temp_api.h"

//--------------------------------------------------------------------
//Добавим запись переменных
//--------------------------------------------------------------------
void AddRecord(sensor_t info[], int number, uint16_t year, uint8_t month, uint8_t day, uint8_t watch, uint8_t minutes, int8_t t)
{
	info[number].year = year;
	info[number].month = month;
	info[number].day = day;
	info[number].watch = watch;		
	info[number].minutes = minutes; 
	info[number].t = t;
}
//-------------------------------------------------------------------
//Инициализируем массив данными
//--------------------------------------------------------------------
int AddInfo(sensor_t info[])
{
	int counter = 0;
	AddRecord(info, counter++, 2021, 9, 16, 15, 30, 9); //Год,месяц, день, час, минута, температура
	AddRecord(info, counter++, 2022, 9, 2, 8, 47, -9);
	AddRecord(info, counter++, 2021, 1, 7, 12, 56, 8);
	AddRecord(info, counter++, 2021, 9, 5, 21, 39, 1);	
	AddRecord(info, counter++, 2024, 12, 17, 16, 44, 41);
	AddRecord(info, counter++, 2024, 2, 23, 11, 22, -25);
	AddRecord(info, counter++, 2023, 9, 16, 15, 30, 67);
	AddRecord(info, counter++, 2023, 9, 2, 8, 47, -9);
	AddRecord(info, counter++, 2024, 1, 7, 12, 56, 48);
	AddRecord(info, counter++, 2023, 9, 5, 21, 39, 1);
	AddRecord(info, counter++, 2021, 12, 17, 16, 44, 34);
	AddRecord(info, counter++, 2025, 2, 23, 11, 22, -5);
	AddRecord(info, counter++, 2021, 9, 16, 15, 30, 16);
	AddRecord(info, counter++, 2022, 9, 2, 8, 47, -19);
	AddRecord(info, counter++, 2025, 1, 7, 12, 56, 8);
	AddRecord(info, counter++, 2021, 9, 5, 21, 39, 11);
	AddRecord(info, counter++, 2025, 12, 17, 16, 44, 30);
	AddRecord(info, counter++, 2025, 2, 23, 11, 22, -2);
	return counter;
}
//----------------------------------------------------------------
// Функция вывода на индикацию элементов массива
//------------------------------------------------------------------
void print(sensor_t info[], int number)
{
	printf("===================================\n");
	for (int i = 0; i < number; i++)
	{
		printf("%04d-%02d-%02d watch=%02d min=%02d t=%3d\n", info[i].year,
			   info[i].month, info[i].day, info[i].watch, info[i].minutes, info[i].t);
	}
}
//------------------------------------------------------------------
// Функция смены позиций двух элементов массива
//------------------------------------------------------------------
void cgangeIJ(sensor_t info[], int i, int j)
{
	sensor_t temp;
	temp = info[i];
	info[i] = info[j];
	info[j] = temp;
}
//--------------------------------------------------------------
/*упорядочивающую возрастанию температуры
 * Здесь все довольно просто
 */
void SortByT(sensor_t info[], int n)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = i; j < n; ++j)
		{
			if (info[i].t >= info[j].t)
			{
				cgangeIJ(info, i, j);
			}
		}
	}
}
//--------------------------------------------------------------
/* По дате ситуация несколько сложнее, потому что дата имеет несколько полей,
 * год, месяц и день. есть два подхода.
 */
//--------------------------------------------------------------
//
//-------------------------------------------------------------
unsigned int DateToInt(sensor_t info[])
{
	return info->year << 16 | info->month << 8 | info->day;
}

// другой подход
//  2. можно написать специальную функцию Compare
//---------------------------------
// Избавились от (int(*) (const void*, const void *)) в функции qsort
// int Compare(sensor_t* a,sensor_t* b)
int Compare(const void *va, const void *vb) // такие замены
{
	// Делаем явное приведение типов
	sensor_t *a = (sensor_t *)va; // такие замены
	sensor_t *b = (sensor_t *)vb; // такие замены
	//------------------------------------
	if (a->year != b->year)
	{
		return a->year - b->year;
	}
	else if (a->month != b->month)
	{
		return a->month - b->month;
	}
	else
	{
		return a->day - b->day;
	}
}
//--------------------------------------------------------------
// 1. можно написать так
// упорядочивающую его по дате
void SortByDate(sensor_t info[], int n)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = i; j < n; ++j)
		{
			// if(DateToInt(info+i)>=DateToInt(info+j)) //Для варианта 1
			if (Compare(&info[i], &info[j]) > 0) // Для варианта 2
			{
				cgangeIJ(info, i, j);
			}
		}
	}
}
//-----------------------------------------------------------------
// Запишем данные в бинарный файл
//---------------------------------------------------------------
void save_bin(sensor_t info[], int number) // Открыли файл на запись
{
	FILE *f = fopen("sensor.bin", "wb");
	fwrite(info, sizeof(sensor_t), number, f);
	fclose(f);
}
//-----------------------------------------------------------------
// Считаем данные из бинарного файла
//---------------------------------------------------------------
void load_bin(sensor_t info[], int number) // Открыли файл на чтение
{
	// Проверка на открытие файла
	FILE *f = fopen("sensor.bin", "rb");
	// Проверка на считанные данные
	fread(info, sizeof(sensor_t), number, f);
	fclose(f);
}
//-----------------------------------------------------------------
// Добавим количество элементов с которыми мы работаем. Запишем их в начало файла
// Считаем данные из бинарного файла
//---------------------------------------------------------------
void load_bin_d(data_t *d)
{
	// Проверка на открытие файла
	FILE *f = fopen("sensor.bin", "rb");
	// Проверка на считанные данные
	fread(&d->number, sizeof(d->number), 1, f);
	fread(d->info, sizeof(sensor_t), d->number, f);
	fclose(f);
}
//-----------------------------------------------------------------
// Добавим количество элементов с которыми мы работаем. Запишем их в начало файла
// Запишем данные в бинарный файл
//---------------------------------------------------------------
void save_bin_d(data_t *d)
{
	FILE *f = fopen("sensor.bin", "wb");
	fwrite(&d->number, sizeof(d->number), 1, f);
	fwrite(d->info, sizeof(sensor_t), d->number, f);
	fclose(f);
}
//-----------------------------------------------------------------
// Запишем данные из бинарного файла в union
//---------------------------------------------------------------
void load_bin_des(union sensor_serdes *d)
{
	FILE *f = fopen("sensor.bin", "rb");
	fread(d->bytes, sizeof(d->bytes), 1, f);
	//fread(d->bytes + sizeof(d->_data.number), d->_data.number * sizeof(struct sensor), 1, f);
	fclose(f);
}
//-----------------------------------------------------------------
// Сохраним данные из union в бинарный файл
//---------------------------------------------------------------
void save_bin_ser(union sensor_serdes *d)
{
	FILE *f = fopen("sensor.bin", "wb");
	fwrite(d->bytes, sizeof(d->bytes), 1, f);
	//fwrite(d->bytes + sizeof(d->_data.number), d->_data.number * sizeof(struct sensor), 1, f);
	fclose(f);
}
//----------------------------------------------------------------
//Статистика за месяц
//---------------------------------------------------------------
//Среднемесячная температура
//---------------------------------------------------------------
void SortBy_Аverage_monthly__T(sensor_t info[], int n, int year, int month) 
{
    int sum = 0, count = 0;
    for (int i = 0; i < n; i++) 
	{
        if (info[i].year == year && info[i].month == month) 
		{
            sum += info[i].t;
            count++;
        }
    }
    if (count > 0)
        printf("Среднемесячная температура %d-%02d = %.2f\n", year, month, (float)sum / count);
    else
        printf("Нет данных за %d-%02d\n", year, month);
}
//---------------------------------------------------------------
//Минимальная температура в текущем месяце
//---------------------------------------------------------------
void SortBy_min_T_in_the_current_month(sensor_t info[], int n, int year, int month) 
{
    int min_t = 127; // максимально возможное значение для int8_t
    int found = 0;
    for (int i = 0; i < n; i++) 
	{
        if (info[i].year == year && info[i].month == month) 
		{
            if (!found || info[i].t < min_t) 
			{
                min_t = info[i].t;
                found = 1;
            }
        }
    }
    if (found)
        printf("Минимальная температура %d-%02d = %d\n", year, month, min_t);
    else
        printf("Нет данных за %d-%02d\n", year, month);
}
//---------------------------------------------------------------
//Максимальная температура в текущем месяце
//---------------------------------------------------------------
void SortBy_max_T_in_the_current_month(sensor_t info[], int n, int year, int month) 
{
    int max_t = -128; // минимально возможное значение для int8_t
    int found = 0;
    for (int i = 0; i < n; i++) 
	{
        if (info[i].year == year && info[i].month == month) 
		{
            if (!found || info[i].t > max_t) 
			{
                max_t = info[i].t;
                found = 1;
            }
        }
    }
    if (found)
        printf("Максимальная температура %d-%02d = %d\n", year, month, max_t);
    else
        printf("Нет данных за %d-%02d\n", year, month);
}


//----------------------------------------------------------
//Функция ввода года с проверкой
//---------------------------------------------------------------
int input_year(const char *prompt) 
{
    int y;
    printf("%s", prompt);
    while (scanf("%d", &y) != 1) 
	{
        printf("Некорректный ввод. Попробуйте еще раз: ");
        while (getchar() != '\n'); // очистка буфера
    }
    return y;
}
//----------------------------------------------------------------
//Статистика за год
//---------------------------------------------------------------
//Среднегодовая температура
//---------------------------------------------------------------
void SortBy_Аverage_year__T(sensor_t info[], int n, int year) 
{
    int sum = 0, count = 0;
    for (int i = 0; i < n; i++) 
	{
        if (info[i].year == year) 
		{
            sum += info[i].t;
            count++;
        }
    }
    if (count > 0)
        printf("Среднегодовая температура %d = %.2f\n", year, (float)sum / count);
    else
        printf("Нет данных за %d\n", year);
}
//---------------------------------------------------------------
//Минимальная температура за год
//---------------------------------------------------------------
void SortBy_min_T_for_the_year(sensor_t info[], int n, int year) 
{
    int min_t = 127;
    int found = 0;
    for (int i = 0; i < n; i++) 
	{
        if (info[i].year == year) 
		{
            if (!found || info[i].t < min_t) 
			{
                min_t = info[i].t;
                found = 1;
            }
        }
    }
    if (found)
        printf("Минимальная температура за %d = %d\n", year, min_t);
    else
        printf("Нет данных за %d\n", year);
}
//---------------------------------------------------------------
//Максимальная температура за год
//---------------------------------------------------------------
void SortBy_max_T_for_the_year(sensor_t info[], int n, int year) 
{
    int max_t = -128;
    int found = 0;
    for (int i = 0; i < n; i++) 
	{
        if (info[i].year == year) 
		{
            if (!found || info[i].t > max_t) 
			{
                max_t = info[i].t;
                found = 1;
            }
        }
    }
    if (found)
        printf("Максимальная температура за %d = %d\n", year, max_t);
    else
        printf("Нет данных за %d\n", year);
}