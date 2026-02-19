/*
//------------------------------------------------------
//    XOR шифрование
//------------------------------------------------------ 
В криптографии простой шифр XOR — это алгоритм шифрования, который
работает в соответствии с принципами операции:

A ^ 0 = A                                           x       y       x^y
A ^ A = 0 — самообратимость                         0       0       0
A ^ B = B ^ A — коммутативность                     0       1       1
(A ^ B) ^ C = A ^ (B ^ C) — ассоциативность         1       0       1
(A ^ B) ^ B = A — реверсивность                     1       1       0

Пример XOR шифрования

*/
//O(log2 n) сложность

/*
#include <stdio.h>
#include <string.h>


void encryptDecrypt(char inpString[], char key[]) 
{
    size_t len = strlen(inpString);
    size_t key_len = strlen(key);
    for (size_t i = 0; i < len; i++) 
    {
        inpString[i] = inpString[i] ^ key[i%key_len];
    }
}

int main(void)
{
    char sampleString[] = "Hello world";
    char key[] = "PASSWORD";//HASSWORD

    //Что плохо в этом примере? Если символ в слове key совпадает с
    //символом в кодируемом слове, то в строку будет занесено число 0 —
    //признак конца строки.
    //Если вместо использовать, то все сломается 
    //char key[] = "HASSWORD";//HASSWORD - вот здесь заменили PASSWORD на HASSWORD

    printf("Encrypted String: ");
    encryptDecrypt(sampleString, key);//HASSWORD
    printf("%s\n", sampleString);
    
    printf("Decrypted String: ");
    encryptDecrypt(sampleString, key);//HASSWORD
    printf("%s\n", sampleString);

    return 0;
}
*/


//Для решения этой проблемы мы напишем структуру в которой будет длянна отдельно 
//и сама строка отдельно

#include <stdio.h>
#include <string.h>
#include <stddef.h>   // для size_t

// Простая структура-обёртка для строки: длина + указатель на буфер
typedef struct String {
    size_t len;   // длина строки (количество символов, без учёта '\0')
    char *str;    // указатель на модифицируемую строку
} str_t;

// Функция "шифрования/дешифрования" XOR-ключом.
// На вход подаём два объекта str_t: входная строка и ключ.
// Описание: каждый байт inpString->str xor'ится с соответствующим
// байтом ключа (ключ циклически повторяется).
void encryptDecrypt(str_t *inpString, str_t *key)
{
    if (!inpString || !key || !inpString->str || !key->str)
        return; // защита от NULL

    if (inpString->len == 0 || key->len == 0)
        return; // нечего делать

    // Применяем XOR с повторяющимся ключом
    for (size_t i = 0; i < inpString->len; i++)
    {
        inpString->str[i] = inpString->str[i] ^ key->str[i % key->len];
    }
}

int main(void)
{
    // Исходная строка (модифицируемая) и ключ
    char sampleString[] = "Hello world";
    char key[] = "PASSWORD";//Если изменить PASSWORD на , HASSWORD то все будет нормально работать

    // Оборачиваем их в структуру str_t:
    // strlen возвращает количество символов до '\0' (без нулевого байта)
    str_t s_sampleString = { strlen(sampleString), sampleString };
    str_t s_key = { strlen(key), key };

    // Покажем исходную строку
    printf("Original: %s\n", s_sampleString.str);

    // Зашифруем
    printf("Encrypted: ");
    encryptDecrypt(&s_sampleString, &s_key);
    printf("%s\n", s_sampleString.str);

    // Расшифруем (XOR тот же ключ — операция обратима)
    printf("Decrypted: ");
    encryptDecrypt(&s_sampleString, &s_key);
    printf("%s\n", s_sampleString.str);

    return 0;
}