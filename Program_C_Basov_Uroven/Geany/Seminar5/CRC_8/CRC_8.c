/*
    Задача вычисления CRC-8
//----------------------------------------------------    
Схематичное представление вычисления CRC на примере деления 
на многочлен x 8 + x2 + x1 + x0

//---------------------------------------------------------------------
*/

#include <stdio.h>

/* 
Name : CRC-8
Poly : 0x31 x^8 + x^5 + x^4 + 1
Init : 0xFF
Revert: false
XorOut: 0x00
Check : 0xF7 ("123456789")
MaxLen: 15 байт(127 бит) - обнаружение одинарных, двойных, тройных
и всех нечетных ошибок 
*/
unsigned char Crc8(unsigned char*pcBlock, unsigned int len)
{
    unsigned char crc=0xFF;
    unsigned int i;
    while (len--)
    {
        crc^=*pcBlock++;
        for (i =0; i <8; i++)
        {
            crc=crc&0x80? (crc<<1) ^0x31:crc<<1;
        }
    }
    return crc;
}

int main(int argc, char **argv)
{
    unsigned char arr[] = {'1','2','3','4','5','6','7','8','9',0};
                                            //в 0 и кладется подсчитанное CRC
    arr[9] = Crc8(arr,9);
    printf("%x\n",Crc8(arr,9));// CRC = f7
    arr[3] = 7; //Внесли помеху в сообщение - изменили один байт
    printf("%x\n",Crc8(arr,9));// CRC = b0 - не совпало с f7
    if(Crc8(arr,9) != arr[9])
    {
        printf("CRC ERROR\n"); //Если CRC не совпало, то выводим сообщение об ощибке CRC
    }
    return 0;
}