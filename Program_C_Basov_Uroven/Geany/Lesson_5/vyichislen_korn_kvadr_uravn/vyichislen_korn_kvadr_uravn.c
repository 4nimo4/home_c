/*
 * vyichislen_korn_kvadr_uravn.c
 * 
	Вычисление корней квадратного уравнения
	(продолжение) - добавляем систему меню
 * 
 */


#include <stdio.h>
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
	NO_PRINT:
			scanf("%c",&Choice);
			printf("%x\n",Choice);
			switch(Choice)
				{
					case '1':
						printf("SquarEquation()\n");
					break;
					case '0':
					case 'q':
					case 'Q':
						return 0;
					break;
					case 0xa:
						goto NO_PRINT;
					break;
					default:
			printf("Непонятный выбор %x\n",Choice);
					break;
				}
		}
	return 0;
}

