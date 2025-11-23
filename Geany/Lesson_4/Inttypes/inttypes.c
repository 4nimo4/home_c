/*
 * inttypes.c
 * Типы фиксированной длинны
 * 
 */


#include <stdio.h>
#include <inttypes.h>


int main()
{
	int64_t x;
	uint64_t ux;
	int32_t a; // = int a 
	int8_t b; // = char b
	uint8_t c; // = unsigned char c
	scanf("%" SCNd64, &x); // SCNu32
	printf("a = %" PRId64 " hex a = %" PRIx64"\n", x, x); // PRIu64
	scanf("%" SCNu64, &ux); // SCNu32
	printf("a = %" PRIu64 " hex a = %" PRIx64"\n", ux, ux); // PRIu64
	
	return 0;
}

