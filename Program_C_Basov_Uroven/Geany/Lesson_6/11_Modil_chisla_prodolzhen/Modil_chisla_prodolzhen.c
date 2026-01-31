/*
 * Modil_chisla_prodolzhen.c
 * 
	Вычисление корней квадратного уравнения
	(продолжение). Дорабатываем систему меню
 * 
 * 
 */


#include <stdio.h>
#include <conio.h>
#include <locale.h>

int main(int argc, char **argv)
{
	char Choice;
	setlocale(LC_ALL, "Rus");
	while(1)
		{
			printf("1. Вычисление корней квадратного уравнения\n");
			printf("0. Выход\n");
			printf("Для выход нажмите Q\n");
			Choice = getch();
			switch(Choice)
				{
					case '1':
					SquarEquation();
					break;
					case '0':
					case 'q':
					case 'Q':
					return 0;
					break;
					default:
					printf("Непонятный выбор %x\n",Choice);
					break;
				}
		}
		return 0;
}
