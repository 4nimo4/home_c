/*
 * union2.c
 * 
	Пример упаковки float
	
	Выравнивание в структурах и объединениях
	
 */
 
 #include <stdio.h>
 #include <inttypes.h>

//------------------------------------------------------------ 
//Для одного числа
//------------------------------------------------------------ 

union intbytes 	
	{
		uint32_t number;
		float real;
		uint8_t bytes[4];
	} d;

//------------------------------------------------------------ 
//если два или более чисел, используем массив
//------------------------------------------------------------
/*
union intbytes 	
	{
		uint32_t number[2];
		float real[2];
		uint8_t bytes[8];
	} d;
*/
//------------------------------------------------------------	
int main(void)
{
	d.real = 3.14;
	printf ("Real %f\n",d.real);
	printf ("Number %x\n",d.number);
	printf(" in memory is: %x %x %x %x\n", d.bytes[0],d.bytes[1], d.bytes[2], d.bytes[3]); 
	return 0;
}
