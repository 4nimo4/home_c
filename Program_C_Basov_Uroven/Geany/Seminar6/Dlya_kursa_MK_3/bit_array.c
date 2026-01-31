/*
//--------------------------------------------------------------------
                Процесс сборки
                Побитовые операции
//------------------------------------------------------------------------
        Задача
//------------------------------------------------------------------------
Реализовать работу с битовым массивом, то есть функции:

int setbit(int index, int bit, uint32_t array[])    
int getbit(int index,uint32_t array[])

такие, чтобы устанавливали (забирали) index-й бит по
счёту в массиве array.

Пример, bit_array.c
//-----------------------------------------------------------
Пояснения:

В функции
setbit
и
getbit
используется деление на 32, чтобы найти нужное 32-битное слово, 
и остаток от деления на 32 — чтобы найти позицию бита в этом слове.
Вводится номер бита и значение, устанавливается бит, затем выводится 
содержимое слова и значение бита.
Проверка границ массива защищает от выхода за пределы.


/*
#include <stdio.h>
#include <stdint.h>

int setbit(int index, int bit, uint32_t array[])   //index=0..8*sizeof(array) - это набор из
            //uint32_t (набор int-ов и мы хотим либо поднимать, либо опускакть какой либо бит 
            //во всем этом наборе)
{
    bit = (bit != 0);
    int byte_number = index/8/sizeof(uint32_t); //находим байт и в рамках этого байта, 
                                                //поднимаем или опускаем бит
    array [byte_number] &= ~(1 << (index%32));
    array [byte_number] |= (bit << (index%32));
    return bit;
}

int getbit(int index,uint32_t array[])
{
    int byte_number = index/8/sizeof(uint32_t);
    uint32_t bit = array[byte_number] & (1 << (index%32));
    return bit != 0;
}

int main()
{
    uint32_t array[100] = {0};
    int index=0,b=0;
    scanf ("%d%d",&index, &b);
    if (index<0 || index > 8*sizeof(array))
        return 1;
    setbit (index,b,array);
    printf("%u\n", array[index/32]);
    printf("%d\n",
    getbit(index,array));
    return 0;
}
*/
#include <stdio.h>      // Для функций ввода-вывода
#include <stdint.h>     // Для uint32_t

// Устанавливает значение (0 или 1) бита с номером index в битовом массиве array[]
int setbit(int index, int bit, uint32_t array[])
{
    bit = (bit != 0);   // Любое ненулевое значение превращаем в 1 (логическое)
    // Определяем номер 32-битного слова, в котором находится нужный бит
    int word_number = index / 32;
    // Сбрасываем (обнуляем) нужный бит
    array[word_number] &= ~(1U << (index % 32));
    // Устанавливаем бит, если bit == 1
    array[word_number] |= (bit << (index % 32));
    return bit;         // Возвращаем установленное значение
}

// Получает значение (0 или 1) бита с номером index в битовом массиве array[]
int getbit(int index, uint32_t array[])
{
    int word_number = index / 32; // Определяем номер 32-битного слова
    // Получаем значение нужного бита (0 или 1)
    uint32_t bit = array[word_number] & (1U << (index % 32));
    return bit != 0;    // Преобразуем к 0 или 1
}

int main()
{
    uint32_t array[100] = {0};    // Битовый массив (3200 бит)
    int index = 0, b = 0;         // index — номер бита, b — значение (0 или 1)

    // Вводим номер бита и значение
    scanf("%d%d", &index, &b);

    // Проверяем, что индекс в допустимых пределах
    if (index < 0 || index >= 8 * sizeof(array))
        return 1;

    // Устанавливаем бит
    setbit(index, b, array);

    // Выводим 32-битное слово, в котором находится этот бит (для наглядности)
    printf("%u\n", array[index / 32]);

    // Проверяем и выводим значение этого бита
    printf("%d\n", getbit(index, array));

    return 0;
}