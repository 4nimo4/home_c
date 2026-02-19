/*
//------------------------------------------------------
//    Алгоритм «сортировочная станция» улучшенная версия
//------------------------------------------------------ 
В данном алгоритме под токеном подразумевается число, скобка или одна из
операций (+ - * /).
Вычислить выражение (3+5)*12-17*3

Шаг                 Цепочка             Стек           Вывод     
1                   (3+5)*12-17*3       (         
2                   3+5)*12-17*3        (               3
3                   +5)*12-17*3         ( +             3
4                   5)*12-17*3          ( +             3 5     
5                   )*12-17*3                           3 5 +
6                   *12-17*3            *               3 5 +
7                   12-17*3             *               3 5 + 12 *
8                   -17*3               -               3 5 + 12 *
9                   17*3                -               3 5 + 12 *17
10                  *3                  * -             3 5 + 12 *17
11                  3                   * -             3 5 + 12 *17 3
12                                                      3 5 + 12 *17 3 * -
//------------------------------------------------------
улучшеная версия программы так, чтобы она стала более безопасной, переносимой и удобной:
Ниже — улучшённая, аккуратная и комментированная версия на целых (int). 
Она возвращает код ошибки на некорректный ввод и проверяет границы буферов. 

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define INPUT_MAX   1024   // макс длина входной строки
#define OUT_MAX     2048   // макс длина выходной (постфиксной) строки
#define STACK_MAX   512    // макс размер стека операторов

/* Стек для операторов */
static char opstack[STACK_MAX];
static int op_top = 0;

/* Безопасный push */
static bool push_op(char c) {
    if (op_top >= STACK_MAX) return false;
    opstack[op_top++] = c;
    return true;
}

/* Безопасный pop: возвращает '\0' при underflow */
static char pop_op(void) {
    if (op_top <= 0) return '\0';
    return opstack[--op_top];
}

/* Взгляд на вершину без извлечения; '\0' если пустой */
static char peek_op(void) {
    if (op_top <= 0) return '\0';
    return opstack[op_top - 1];
}

/* Проверка оператора */
static bool is_op(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/* Приоритет оператора: больше — выше приоритет */
static int prec(char c) {
    if (c == '+' || c == '-') return 1;
    if (c == '*' || c == '/') return 2;
    return 0;
}

/* Добавление токена (строка) в выходную очередь с проверкой границы */
static bool append_out(char *out, size_t *pos, const char *token) {
    size_t need = strlen(token);
    if (*pos + need >= OUT_MAX - 1) return false;
    memcpy(out + *pos, token, need);
    *pos += (int)need;
    out[*pos] = '\0';
    return true;
}

/* append single char + space */
static bool append_out_char(char *out, size_t *pos, char c) {
    if (*pos + 2 >= OUT_MAX - 1) return false;
    out[(*pos)++] = c;
    out[(*pos)++] = ' ';
    out[*pos] = '\0';
    return true;
}

/* Основная функция: преобразование infix -> postfix (RPN).
   Возвращает 0 при успехе, ненулевой код при ошибке. */
int infix_to_postfix(const char *in, char *out, size_t out_max) {
    size_t len = strlen(in);
    size_t out_pos = 0;
    op_top = 0;

    for (size_t i = 0; i < len; ) {
        /* Пропуск пробелов */
        if (isspace((unsigned char)in[i])) { i++; continue; }

        /* Число (поддержка знакового целого числа) */
        if (isdigit((unsigned char)in[i]) || 
            (in[i] == '-' && i + 1 < len && isdigit((unsigned char)in[i+1]) &&
             (i == 0 || in[i-1] == '(' || isspace((unsigned char)in[i-1])))) {
            // считаем число (включая унарный минус)
            int sign = 1;
            if (in[i] == '-') { sign = -1; i++; }
            long val = 0;
            size_t digits = 0;
            while (i < len && isdigit((unsigned char)in[i])) {
                val = val * 10 + (in[i] - '0');
                i++; digits++;
                if (digits > 20) { /* защита от слишком длинного числа */ break; }
            }
            char buf[64];
            int n = snprintf(buf, sizeof(buf), "%ld ", sign * val);
            if (n < 0) return 2;
            if (!append_out(out, &out_pos, buf)) return 3; // output overflow
            continue;
        }

        char c = in[i];

        /* Открывающая скобка */
        if (c == '(') {
            if (!push_op(c)) return 4; // stack overflow
            i++;
            continue;
        }

        /* Закрывающая скобка — вытащить операторы до '(' */
        if (c == ')') {
            bool found = false;
            while (op_top > 0) {
                char t = pop_op();
                if (t == '(') { found = true; break; }
                if (!append_out_char(out, &out_pos, t)) return 3;
            }
            if (!found) return 5; // mismatched parentheses
            i++;
            continue;
        }

        /* Оператор */
        if (is_op(c)) {
            // пока есть оператор на вершине со >= приоритетом, выписываем его
            while (op_top > 0) {
                char t = peek_op();
                if (t == '(') break;
                if (prec(t) >= prec(c)) {
                    pop_op();
                    if (!append_out_char(out, &out_pos, t)) return 3;
                } else break;
            }
            if (!push_op(c)) return 4;
            i++;
            continue;
        }

        /* Неизвестный символ */
        return 6; // unknown token
    }

    /* По окончании входа — вытащить все оставшиеся операторы */
    while (op_top > 0) {
        char t = pop_op();
        if (t == '(' || t == ')') return 5; // mismatched parentheses
        if (!append_out_char(out, &out_pos, t)) return 3;
    }

    // успешный результат уже в out
    (void)out_max;
    return 0;
}

/* Простой демонстрационный main */
int main(void) {
    char input[INPUT_MAX];
    char output[OUT_MAX];

    printf("Input infix expression: ");
    if (!fgets(input, sizeof(input), stdin)) return 0;
    // убираем конечный перевод строки
    size_t L = strlen(input);
    if (L && input[L-1] == '\n') input[L-1] = '\0';

    int rc = infix_to_postfix(input, output, OUT_MAX);
    if (rc == 0) {
        printf("Postfix: %s\n", output);
    } else {
        fprintf(stderr, "Error: conversion failed (code %d)\n", rc);
    }
    return rc;
}
