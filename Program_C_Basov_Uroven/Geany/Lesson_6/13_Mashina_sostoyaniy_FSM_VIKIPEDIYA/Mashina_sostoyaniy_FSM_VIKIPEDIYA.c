/*
 * Mashina_sostoyaniy_FSM_VIKIPEDIYA.c
 * 
 * Машина состояний (FSM) и конечные автоматы
 *	ВИКИПЕДИЯ
 * 
	Пусть требуется написать на языке Си программу, читающую из потока
	стандартного ввода текст, состоящий из строк, и для каждой строки
	печатающую первое слово этой строки и перевод строки.
 * 
 * 
 */


#include <stdio.h>
//----------------------------------------------------------
/*
int main() 
{
	int c;

	do 
	{
		c = getchar();
		while (c == ' ') 
			{
				c = getchar();
			}
		while (c != ' ' && c != '\n' && c != EOF) 
			{
				putchar(c), c = getchar();
				putchar('\n');
			}
		while (c != '\n' && c != EOF) 
			{
				c = getchar();
			} 
		while (c != EOF);
		
 return 0;
}
*/
//------------------------------------------

int main() 
{
	enum states 
		{ before,inside, after } state;
	int c;
	state = before;
	while ((c = getchar()) !=EOF) 
		{
			switch (state) 
				{
					case before:
					if (c == '\n')
						{
							putchar('\n');
						}
					else if (c != ' ')
						{
							putchar(c), state = inside;
							break;
							case inside: 
						}
					switch (c) 
						{
							case ' ': state = after; 
							break;
							case '\n':
							putchar('\n'), state = before;
							break;
							default: putchar(c);
						}
					break;
					case after:
						if (c == '\n')
							{
								putchar('\n'), state = before;
							}
							
				}
		}
 return 0;
}
