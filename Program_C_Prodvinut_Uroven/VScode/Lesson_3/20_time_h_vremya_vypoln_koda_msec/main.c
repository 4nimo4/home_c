// Эта директива говорит стандартной библиотеке (glibc), что мы хотим
// использовать POSIX-расширения версии не ниже 1993 года.
// В частности, это включает прототип функции clock_gettime и структуру timespec.
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>

// "Долгая" работа — имитация нагрузки для замера времени
void do_work(void)
{
    // volatile запрещает компилятору оптимизировать переменную sum и выкидывать цикл.
    // Без volatile компилятор может решить, что sum нигде не используется,
    // и избавиться от всего цикла, тогда время будет почти 0.
    volatile long long sum = 0;

    // Большой цикл, занимающий некоторое время
    for (long long i = 0; i < 500000000; ++i) 
    {
        sum += i;
    }
}

int main(void)
{
    // struct timespec — структура для представления времени высокой точности:
    //   tv_sec  — целое число секунд,
    //   tv_nsec — наносекунды (0..999999999).
    struct timespec start, end;

    // Берём время "до" выполнения работы.
    // CLOCK_MONOTONIC — монотонные часы:
    //   - не зависят от изменения системного времени (ручной перевод часов, NTP),
    //   - гарантированно не идут назад.
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Выполняем код, время работы которого хотим измерить.
    do_work();

    // Берём время "после" выполнения работы.
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Теперь нужно вычесть два момента времени: end - start.
    // Раздельно считаем разницу по секундам и наносекундам.
    long long sec_diff  = (long long)end.tv_sec  - (long long)start.tv_sec;
    long long nsec_diff = (long long)end.tv_nsec - (long long)start.tv_nsec;

    // Если разность наносекунд получилась отрицательной,
    // "занимаем" одну секунду и добавляем к наносекундам 1_000_000_000.
    // Это нормализация: приводим (сек, нс) к виду, где нс >= 0.
    if (nsec_diff < 0) {
        sec_diff  -= 1;
        nsec_diff += 1000000000LL;
    }

    // Общее время в наносекундах:
    //   total_nsec = sec_diff * 1_000_000_000 + nsec_diff
    long long total_nsec = sec_diff * 1000000000LL + nsec_diff;

    // Выводим время в чистых наносекундах
    printf("Elapsed time: %lld ns\n", total_nsec);

    // Дополнительно выводим то же самое:
    //  - в миллисекундах (делим наносекунды на 1e6 = 1000000.0),
    //  - в секундах (делим на 1e9 = 1000000000.0).

    printf("Elapsed time: %.3f ms\n", total_nsec / 1e6);
    printf("Elapsed time: %.6f s\n",  total_nsec / 1e9);

    return 0;
}