/*
 * dat_temper4.c
 * 
 * Продолжение
 * 
	Сериализация и десериализация данных	
 */
/*
 * 
	Подготовка к курсовому проекту
 * 
 * Описать структурный тип для представления сбора информации с
	датчика температуры, необходимые поля: дата (день, месяц, год) и
	температура. Используя этот тип, описать функцию, принимающую на
	вход массив таких данных и упорядочивающую его по возрастанию
	температуры, по дате
 * 
 * 
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "temp_api.h"

int main(void)
{
	data_t d;
	union sensor_serdes* ds = (union sensor_serdes*) &d;
	//sensor_t info[SIZE];
	d.number=AddInfo(d.info);
	print(d.info,d.number);	// Выводим массив данных
	
	//save_bin(d.info, d.number); // Записали данные в бинарный файл 
	//save_bin_d(&d); // Записали данные в бинарный файл 
	save_bin_ser(ds); // Записали данные в бинарный файл 
	
	printf("\nSort by t\n"); //Неотсортированный массив по температуре
	SortByT(d.info,d.number);	//Сортируем массив по температуре
	print(d.info,d.number);	//Выводим отсортированный массив по температкре
	
	printf("\nLoad\n");
	//load_bin(d.info, d.number); // Считали данные из бинарного файла
	//load_bin_d(&d); // Считали данные из бинарного файла
	load_bin_des(ds); // Считали данные из бинарного файла
	print(d.info,d.number);	//
	
	printf("\nSort by date\n");	//Неотсортированный массив по дате
	// Заменим функцию SortByDate функцией qsort
	//SortByDate(info,number); //Сортируем массив по дате
	//----------------------------------------------------------------------
	//qsort(info, number, sizeof (sensor_t), (int(*) (const void*, const void *)) Compare);
	//-----------------------------------------------------------------------
	qsort(d.info, d.number, sizeof (sensor_t), Compare);//Избавились от (int(*) (const void*, const void *))
	//-----------------------------------------------------------------------
	print(d.info,d.number); //Выводим отсортированный массив по дате
	return 0;
}

