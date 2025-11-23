/*
 * S3_12.c
 * 
 //----------------------------------------------------------------
	Указатели и символьные строки
//-----------------------------------------------------------------

	char str[10] = "0123456";
	char *p;						// указатель на символ
	p = str;						// или & str[0]
	*p = 'A';						// "A12345"
	p++;							// перейти к str[1]
	*p = 'B';						// "AB2345"
	p++;							// перейти к str[2]
	strcpy(p, "CD");				// "ABCD"
	strcat(p, "qqq");				// "ABCDqqq"
	puts(p);
 */


#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
	char str[10] = "0123456";
	char *p;						// указатель на символ

	p = str;						// или & str[0]
	puts(p);
	printf("\n");
	
	*p = 'A';						// "A12345"
	puts(p);
	printf("\n");
	
	p++;							// перейти к str[1]
	*p = 'B';						// "AB2345"
	puts(p);
	printf("\n");
	
	p++;							// перейти к str[2]
	strcpy(p, "CD");				// "ABCD"
	puts(p);
	printf("\n");
	
	strcat(p, "qqq");				// "ABCDqqq"
	puts(p);
	printf("\n");
	
	return 0;
}
