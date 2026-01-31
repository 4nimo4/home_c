/*
 * razmery_structur.c
 * 
	Размеры структур 
	
	Выравнивание в структурах и объединениях
	
 */
 
 #include <stdio.h>
 #include <inttypes.h>

//------------------------------------------------------------ 
//Sizeof s1 = 16
//Sizeof s2 = 12
//------------------------------------------------------------
/* 
struct str1 
	{
		uint32_t u;
		uint8_t c1;
		int32_t i;
		uint8_t c2;
	} s1;
struct str2 
	{
		uint32_t u;
		int32_t i;
		uint8_t c1;
		uint8_t c2;
	} s2;
*/
//------------------------------------------------------------	
//------------------------------------------------------------ 
//Sizeof s1 = 10
//Sizeof s2 = 10
//------------------------------------------------------------ 
#pragma pack(push, 1)
struct str1 
	{
		uint32_t u;
		uint8_t c1;
		int32_t i;
		uint8_t c2;
	} s1;
#pragma pack(pop)


#pragma pack(push, 1)	
struct str2 
	{
		uint32_t u;
		int32_t i;
		uint8_t c1;
		uint8_t c2;
	} s2;
#pragma pack(pop)


//------------------------------------------------------------	
int main(void)
{
	printf("Sizeof s1 = %lu\n", sizeof(s1));
	printf("Sizeof s2 = %lu\n", sizeof(s2));
	return 0;
}

