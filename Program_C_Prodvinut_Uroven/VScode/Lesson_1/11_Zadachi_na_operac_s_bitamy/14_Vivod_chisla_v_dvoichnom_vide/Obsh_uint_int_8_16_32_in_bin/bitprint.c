#include <stdio.h>
#include <stdint.h>
#include "bitprint.h"

int main(void)
{
    uint8_t  u8  = 0x3F;
    int8_t   s8  = -5;
    uint16_t u16 = 0x06CC;
    int16_t  s16 = -123;
    uint32_t u32 = 0x3F;
    int32_t  s32 = -5;

    printf("u8  = %u, bin = ", u8);
    print_binary_uint8(u8);
    putchar('\n');

    printf("s8  = %d, bin = ", s8);
    print_binary_int8(s8);
    putchar('\n');

    printf("u16 = %u, bin = ", u16);
    print_binary_uint16(u16);
    putchar('\n');

    printf("s16 = %d, bin = ", s16);
    print_binary_int16(s16);
    putchar('\n');

    printf("u32 = %u, bin = ", u32);
    print_binary_uint32(u32);
    putchar('\n');

    printf("s32 = %d, bin = ", s32);
    print_binary_int32(s32);
    putchar('\n');

    return 0;
}