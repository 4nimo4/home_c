/*
//------------------------------------------------------
//   C2-Польская запись
//------------------------------------------------------ 
Необходимо вычислить выражение написанное в обратной польской записи. 
На вход подается строка состоящая из целых, неотрицательных чисел и 
арифметических символов. В ответ единственное целое число - результат. 

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_STACK_SIZE 255

typedef int datatype;

datatype st[MAX_STACK_SIZE];
int pst = 0; // вершина стека (количество элементов)

// Поместить значение в стек
void push(datatype v)
{
    if (pst >= MAX_STACK_SIZE) 
    {
        // переполнение стека — выражение слишком сложное
        exit(EXIT_FAILURE);
    }
    st[pst++] = v;
}

// Снять и вернуть верхнее значение стека
datatype pop(void)
{
    if (pst <= 0) 
    {
        // попытка чтения из пустого стека — некорректное выражение
        exit(EXIT_FAILURE);
    }
    return st[--pst];
}

// Проверка: цифра ли символ
_Bool isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

// Проверка: оператор ли символ
_Bool isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Выполнить операцию: взять два верхних аргумента (A B) и выполнить A op B.
// Порядок: из стека сперва достаём B, потом A, считаем A op B.
void operate(char c)
{
    datatype b = pop(); // второй операнд (верх стека)
    datatype a = pop(); // первый операнд

    switch (c)
    {
        case '+': 
            push(a + b); 
            break;
        case '-': 
            push(a - b); 
            break;
        case '*': 
            push(a * b); 
            break;
        case '/':
            if (b == 0) 
            {
                // деление на ноль — по условию обычно не бывает
                exit(EXIT_FAILURE);
            }
            push(a / b);
            break;
        default:
            // неизвестный оператор
            exit(EXIT_FAILURE);
    }
}

int main(void)
{
    char c;
    char str[1000];
    int len = 0;

    // читаем строку целиком (включая пробелы) до '\n' или EOF
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (len >= (int)sizeof(str) - 1)
            break;
        str[len++] = c;
    }
    str[len] = '\0';

    // Парсим строку: пропускаем пробелы, читаем числа и операторы
    for (int i = 0; i < len; ) {
        if (str[i] == ' ' || str[i] == '\t') 
        {
            i++;
            continue;
        }
        if (isDigit(str[i])) 
        {
            int number = 0;
            // читаем целое неотрицательное число
            while (i < len && isDigit(str[i])) 
            {
                number = number * 10 + (str[i] - '0');
                i++;
            }
            push(number);
        } 
        else if (isOperator(str[i])) 
        {
            operate(str[i]);
            i++;
        } else 
        {
            // игнорируем неожиданные символы (если в условии их нет — просто не будет)
            i++;
        }
    }

    // В стеке должен остаться ровно один элемент — результат
    if (pst == 1) 
    {
        printf("%d\n", pop());
    }
    // если pst != 1 — выражение некорректно; по условию обычно это не проверяют

    return 0;
}