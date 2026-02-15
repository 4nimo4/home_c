/*
//------------------------------------------------------
//     Пример оптимизации обработки массива
//------------------------------------------------------ 
Рассмотрим пример: дан массив из целых чисел. Каждое число встречается ровно два
раза, но есть одно, которое встречается только один раз. Необходимо найти его.
// В программе две реализации:
//  1) "в лоб" (O(N^2))
//  2) оптимизированная через XOR (O(N))
// Для каждой измеряем время исполнения и ресурсы.
*/
#define _POSIX_C_SOURCE 200809L

#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>


// Размер массива: 2001 элемент.
// 1000 чисел по два раза (0..999) + 1 «уникальное» число.
enum { SIZE = 2001 };

// Глобальный массив целых чисел, в который мы заполним тестовые данные.
int a[SIZE]; // {0, 1, ..., 999, 12345, 0, 1, ..., 999}

// Утилита: разница между двумя timespec в секундах (double).
double diff_timespec_sec(struct timespec start, struct timespec stop)
{
    double sec  = (double)(stop.tv_sec  - start.tv_sec);
    double nsec = (double)(stop.tv_nsec - start.tv_nsec);
    return sec + nsec / 1e9;
}

int main() 
{
    // 1. Подготовка данных
    for (int i = 0; i < 1000; i++)
        a[i] = i, a[i + 1001] = i;
    a[1000] = 12345;  // число, которое встречается только один раз

    //------------------------------------------------------
    //   ФАЗА 1: "в лоб" (O(N^2))
    //------------------------------------------------------
    struct rusage usage1_before, usage1_after;
    struct timeval u1_start, u1_end, s1_start, s1_end;
    struct timespec t1_start, t1_end;
    int same_naive = 0;

    // фиксируем "стеночное" время до
    clock_gettime(CLOCK_REALTIME, &t1_start);
    // и ресурсы до
    getrusage(RUSAGE_SELF, &usage1_before);
    u1_start = usage1_before.ru_utime;
    s1_start = usage1_before.ru_stime;

    // Алгоритм "в лоб": O(N^2)
    _Bool is_same = 0;
    for (size_t i = 0; i < SIZE; i++)
    {
        is_same = 0;
        for (size_t j = 0; j < SIZE; j++)
        {
            if (i != j && a[i] == a[j])
                is_same = 1;
        }
        if (!is_same)
            same_naive = a[i];
    }

    // ресурсы после
    getrusage(RUSAGE_SELF, &usage1_after);
    u1_end = usage1_after.ru_utime;
    s1_end = usage1_after.ru_stime;
    // стеночное время после
    clock_gettime(CLOCK_REALTIME, &t1_end);

    double wall1 = diff_timespec_sec(t1_start, t1_end);

    //------------------------------------------------------
    //   ФАЗА 2: оптимизированный XOR (O(N))
    //------------------------------------------------------
    struct rusage usage2_before, usage2_after;
    struct timeval u2_start, u2_end, s2_start, s2_end;
    struct timespec t2_start, t2_end;
    int same_xor = 0;

    // фиксируем "стеночное" время до
    clock_gettime(CLOCK_REALTIME, &t2_start);
    // и ресурсы до
    getrusage(RUSAGE_SELF, &usage2_before);
    u2_start = usage2_before.ru_utime;
    s2_start = usage2_before.ru_stime;

    // Оптимизированный алгоритм: один проход, XOR всех элементов
    for (size_t i = 0; i < SIZE; i++)
        same_xor ^= a[i];

    // ресурсы после
    getrusage(RUSAGE_SELF, &usage2_after);
    u2_end = usage2_after.ru_utime;
    s2_end = usage2_after.ru_stime;
    // стеночное время после
    clock_gettime(CLOCK_REALTIME, &t2_end);

    double wall2 = diff_timespec_sec(t2_start, t2_end);

    //------------------------------------------------------
    //   ВЫВОД РЕЗУЛЬТАТОВ
    //------------------------------------------------------

    printf("=============================================\n");
    printf("NAIVE O(N^2) VERSION\n");
    printf("Unique element (naive):      %d\n", same_naive);
    printf("User time   start:           %ld.%06ld\n", u1_start.tv_sec, u1_start.tv_usec);
    printf("User time   end:             %ld.%06ld\n", u1_end.tv_sec,   u1_end.tv_usec);
    printf("System time start:           %ld.%06ld\n", s1_start.tv_sec, s1_start.tv_usec);
    printf("System time end:             %ld.%06ld\n", s1_end.tv_sec,   s1_end.tv_usec);
    printf("Wall-clock time (naive):     %.6f s\n", wall1);
    printf("Max RSS after naive:         %ld bytes\n", usage1_after.ru_maxrss);

    printf("\n=============================================\n");
    printf("XOR O(N) VERSION\n");
    printf("Unique element (XOR):        %d\n", same_xor);
    printf("User time   start:           %ld.%06ld\n", u2_start.tv_sec, u2_start.tv_usec);
    printf("User time   end:             %ld.%06ld\n", u2_end.tv_sec,   u2_end.tv_usec);
    printf("System time start:           %ld.%06ld\n", s2_start.tv_sec, s2_start.tv_usec);
    printf("System time end:             %ld.%06ld\n", s2_end.tv_sec,   s2_end.tv_usec);
    printf("Wall-clock time (XOR):       %.6f s\n", wall2);
    printf("Max RSS after XOR:           %ld bytes\n", usage2_after.ru_maxrss);

    printf("\n=============================================\n");
    printf("Check: results equal?        %s\n",
           (same_naive == same_xor) ? "YES" : "NO");
    
    return 0;
}