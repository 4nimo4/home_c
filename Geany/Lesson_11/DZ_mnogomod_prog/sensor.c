
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "sensor.h"



//--------------------------------------------------------------------	
void AddRecord(sensor_t info[], int number, uint16_t year,uint8_t month, uint8_t day,int8_t t)
{
	info[number].year = year;
	info[number].month = month;
	info[number].day = day;
	info[number].t = t;
}
//-------------------------------------------------------------------
int AddInfo(sensor_t info[])
{
	int counter=0;
	AddRecord(info,counter++,2021,9,16,9);
	AddRecord(info,counter++,2022,9,2,-9);
	AddRecord(info,counter++,2021,1,7,8);
	AddRecord(info,counter++,2021,9,5,1);
 return counter;
}
//----------------------------------------------------------------
void print(sensor_t info[],int number)
{
	printf("===================================\n");
	for(int i=0;i<number;i++)
		{
			printf("%04d-%02d-%02d t=%3d\n", info[i].year, 
			info[i].month, info[i].day, info[i].t);
		}
}
//------------------------------------------------------------------
void cgangeIJ(sensor_t info[],int i, int j) //Функция смены позиций двух элементов массива
{
	sensor_t temp;
	temp=info[i];
	info[i]=info[j];
	info[j]=temp;
}
//--------------------------------------------------------------
/*упорядочивающую его по неубыванию температуры
 * Здесь все довольно просто
*/
void SortByT(sensor_t info[],int n)
{
	for(int i=0; i<n; ++i)
		{
			for(int j=i; j<n; ++j)
				{
					if(info[i].t >= info[j].t)
						{
							cgangeIJ(info,i,j);
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

//другой подход
// 2. можно написать специальную функцию Compare
//---------------------------------
//Избавились от (int(*) (const void*, const void *)) в функции qsort
//int Compare(sensor_t* a,sensor_t* b)
int Compare(const void* va,const void* vb)//такие замены
{
	//Делаем явное приведение типов
	sensor_t* a = (sensor_t*)va;	//такие замены
	sensor_t* b = (sensor_t*)vb;	//такие замены
//------------------------------------
	if(a->year != b->year)
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
//1. можно написать так 
//упорядочивающую его по дате
void SortByDate(sensor_t info[],int n)
{
	for(int i=0; i<n; ++i)
		{
			for(int j=i; j<n; ++j)
				{
					//if(DateToInt(info+i)>=DateToInt(info+j)) //Для варианта 1
					if(Compare(info+i,&info[j])>0) //Для варианта 2
						{
							cgangeIJ(info,i,j);
						} 	
				}
		}
}
//-----------------------------------------------------------------
//Запишем данные в бинарный файл
//---------------------------------------------------------------	
void save_bin(sensor_t info[],int number) //Открыли файл на запись
	{
		FILE* f = fopen("sensor.bin","wb");
		fwrite(info,number*sizeof(struct sensor),1,f);
		fclose(f);
	}
//-----------------------------------------------------------------
//Считаем данные из бинарного файла
//---------------------------------------------------------------	
void load_bin(sensor_t info[],int number)	//Открыли файл на чтение
	{
		//Проверка на открытие файла
		FILE* f = fopen("sensor.bin","rb");
		//Проверка на считанные данные
		fread(info,number*sizeof(struct sensor),1,f);
		fclose(f);
	}	
//-----------------------------------------------------------------
//Добавим количество элементов с которыми мы работаем. Запишем их в начало файла 
//Считаем данные из бинарного файла
//---------------------------------------------------------------
void load_bin_d(data_t* d)
{
//Проверка на открытие файла
 FILE* f = fopen("sensor.bin","rb");
//Проверка на считанные данные
 fread(&d->number,sizeof(d->number),1,f);
 fread(d->info,d->number*sizeof(struct sensor),1,f);
 fclose(f);
}
//-----------------------------------------------------------------
//Добавим количество элементов с которыми мы работаем. Запишем их в начало файла 
//Запишем данные в бинарный файл
//---------------------------------------------------------------	
void save_bin_d(data_t* d)
{
 FILE* f = fopen("sensor.bin","wb");
 fwrite(&d->number,sizeof(d->number),1,f);
 fwrite(d->info,d->number*sizeof(struct sensor),1,f);
 fclose(f);
}
//---------------------------------------------------------------
void load_bin_des(union sensor_serdes * d)
	{
		FILE* f = fopen("sensor.bin","rb");
		fread(d->bytes,sizeof(d->_data.number),1,f);
		fread(d->bytes+sizeof(d->_data.number),d->_data.number*sizeof(struct sensor),1,f);
		fclose(f);
	}
void save_bin_ser(union sensor_serdes* d)
	{
		FILE* f = fopen("sensor.bin","wb");
		fwrite(d->bytes,sizeof(d->_data.number),1,f);
		fwrite(d->bytes+sizeof(d->_data.number),d->_data.number*sizeof(struct sensor),1,f);
		fclose(f);
	}
//---------------------------------------------------------------	
