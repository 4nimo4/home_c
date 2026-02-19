/*
//------------------------------------------------------
//    Обратная польская форма
//------------------------------------------------------ 
Формат записи
Форма записи математических и логических выражений, в
которой операнды расположены перед знаками операций.
Рассмотрим пример такой записи:
1 2 + 4 * 3 +
В обычной инфиксной записи выражение выглядит так:
(1+2) * 4+3

A + B       A B +
A - B       A B -
A / B       A B /
A * B       A B *
Алгоритм

x = ab + cd + 1 -/
1. взять очередной элемент;
2. если это не знак операции, добавить его в стек;
3. если это знак операции, то;
    взять из стека два операнда;
    выполнить операцию и записать результат в стек;
4. перейти к шагу 1.

Пример вычисления
Вычислить выражение 1 2 + 4 × 3 +
Шаг             Оставшаяся цепочка          Стек
1               1 2 + 4 * 3 +               1
2               2 + 4 * 3 +                 1 2
3               + 4 * 3 +                   3
4               4 * 3 +                     3 4
5               * 3 +                       12
6               3 +                         12 3
7               +                           15

Обратная польская форма
Требуется вычислить его значение за O(n), где n — длина строки. Для
вычисления выражений в обратной польской нотации удобно
использовать стек. Порядок действий такой:
1. Обработка входного символа
2. Если на вход подано число, оно помещается на вершину стека.
3. Если на вход подан знак операции, то со стека снимаются два
числа и над ними выполняется соответствующая операция.
Результат выполненной операции кладётся обратно на вершину
стека.
4. Если входной набор символов обработан не полностью, перейти
к шагу 1.
5. После полной обработки входного набора символов, результат
вычисления выражения лежит на вершине стека в качестве
единственного числа.


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
    if (pst >= MAX_STACK_SIZE) {
        fprintf(stderr, "Error. Stack overflow\n");
        exit(EXIT_FAILURE);
    }
    st[pst++] = v;
}

// Снять и вернуть верхнее значение стека
datatype pop(void)
{
    if (pst <= 0) {
        fprintf(stderr, "Error. Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    return st[--pst];
}

_Bool isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

_Bool isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Выполнить операцию: взять два верхних аргумента (A B) и выполнить A op B.
// Заметьте: pop() сначала вернёт верхний элемент (B), затем следующий (A),
// поэтому порядок аргументов должен быть arg2 <op> arg1.
void operate(char c)
{
    datatype b = pop(); // верхний элемент (второй операнд)
    datatype a = pop(); // следующий (первый операнд)

    switch (c)
    {
        case '+': 
            push(a + b); 
            break;
        case '-': 
            push(a - b); 
            break; // A - B
        case '*': 
            push(a * b); 
            break;
        case '/':
            if (b == 0) {
                fprintf(stderr, "Error: division by zero\n");
                exit(EXIT_FAILURE);
            }
            push(a / b); break; // A / B
        default:
            fprintf(stderr, "Unknown operator: %c\n", c);
            exit(EXIT_FAILURE);
    }
}

int main(void)
{
    char c;
    char str[1000];
    int len = 0;

    printf("Input inverse string: ");
    // читаем строку целиком (включая пробелы) до '\n'
    while ((c = getchar()) != '\n' && c != EOF)//1 2 + 4 * 3 + (=15) //3 5 + 10 * 17 2 * - (=46)
    {
        str[len++] = c;
        if (len >= (int)sizeof(str) - 1) break;
    }
    str[len] = '\0';

    // Парсим строку: пропускаем пробелы, читаем числа и операторы
    for (int i = 0; i < len; ) {
        if (str[i] == ' ' || str[i] == '\t') {
            i++;
            continue;
        }
        if (isDigit(str[i])) {
            int number = 0;
            while (i < len && isDigit(str[i])) {
                number = number * 10 + (str[i] - '0');
                i++;
            }
            push(number);
        } else if (isOperator(str[i])) {
            operate(str[i]);
            i++;
        } else {
            // Некорректный символ — пробуем пропустить или сигнализируем
            fprintf(stderr, "Warning: ignoring unknown character '%c'\n", str[i]);
            i++;
        }
    }

    if (pst != 1) {
        fprintf(stderr, "Error: stack has %d elements after evaluation (expected 1)\n", pst);
        // Можно при желании вывести содержимое стека
    } else {
        printf("answer = %d\n", pop());
    }

    return 0;
}
