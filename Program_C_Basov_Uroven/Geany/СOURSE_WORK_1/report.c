#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "temp_function.h"

int main(int argc, char *argv[])
{
    int opt;
    char *filename = NULL;
    int month = 0;
    int month_specified = 0;

    if (argc == 1) {
        print_help(argv[0]);
        return 0;
    }

    while ((opt = getopt(argc, argv, "hf:m:")) != -1) {
        switch (opt) {
        case 'h':
            print_help(argv[0]);
            return 0;
        case 'f':
            filename = optarg;
            break;
        case 'm':
            month = atoi(optarg);
            month_specified = 1;
            break;
        default:
            print_help(argv[0]);
            return 1;
        }
    }

    if (!filename) {
        printf("Ошибка: не указан входной файл (-f file.csv)\n");
        print_help(argv[0]);
        return 1;
    }

    if (month_specified && (month < 1 || month > 12)) {
        printf("Ошибка: некорректный месяц %d (должен быть 1..12)\n", month);
        return 1;
    }

    sensor_t data[MAX_RECORDS];

    int n = load_temperature_csv(filename, data, MAX_RECORDS);
    if (n == 0) {
        printf("Не удалось считать данные из файла %s\n", filename);
        return 1;
    }

    // ---------- расчёт статистики по месяцам и году ----------

    long month_sum[13] = {0};
    int  month_cnt[13] = {0};
    int  month_min[13];
    int  month_max[13];
    int  month_first_line[13] = {0};
    int  month_err[13] = {0};

    for (int i = 1; i <= 12; ++i) {
        month_min[i] =  1000;
        month_max[i] = -1000;
    }

    int year = 0;
    long year_sum = 0;
    int year_cnt = 0;
    int year_min =  1000;
    int year_max = -1000;

    int last_month = 0;

    for (int i = 0; i < n; ++i) {
        if (data[i].valid) {
            int m = data[i].month;
            int t = data[i].temp;

            if (year == 0)
                year = data[i].year;

            if (m >= 1 && m <= 12) {
                if (month_cnt[m] == 0)
                    month_first_line[m] = data[i].line;

                month_sum[m] += t;
                month_cnt[m]++;
                if (t < month_min[m]) month_min[m] = t;
                if (t > month_max[m]) month_max[m] = t;

                last_month = m;
            }

            year_sum += t;
            year_cnt++;
            if (t < year_min) year_min = t;
            if (t > year_max) year_max = t;
        } else {
            // строка с ошибкой: отнесём к последнему корректному месяцу
            if (last_month >= 1 && last_month <= 12)
                month_err[last_month]++;
        }
    }

    double year_avg = (year_cnt > 0) ? (double)year_sum / year_cnt : 0.0;

    // ---------- вывод ----------

    if (month_specified) {
        int m = month;
        printf("============================================================================\n");
        printf(" Line           YEAR    MONTH    ERROR_DATA          MinTEMP       MaxTEMP         AvgTEMP\n");
        printf("============================================================================\n");

        if (month_cnt[m] == 0) {
            printf("Нет данных за месяц %d\n", m);
            return 0;
        }

        double avg = (double)month_sum[m] / month_cnt[m];

        printf("%-13d%6d%8d%12d%16d%13d%14.2f\n",
               month_first_line[m],
               year,
               m,
               month_err[m],
               month_min[m],
               month_max[m],
               avg);

        if (month_err[m] > 0) {
            printf("       ------Wrong format in line------ xx\n");
            printf("Строки с ошибками: ");

            int printed = 0;
            int current_month = 0;
            for (int i = 0; i < n; ++i) {
                if (data[i].valid) {
                    current_month = data[i].month;
                } else {
                    if (current_month == m) {
                        if (printed > 0) printf(", ");
                        printf("%d", data[i].line);
                        printed++;
                    }
                }
            }
            printf("\n");
        }
    } else {
        printf("============================================================================\n");
        printf(" Line           YEAR    MONTH    ERROR_DATA          MinTEMP       MaxTEMP         AvgTEMP\n");
        printf("============================================================================\n");

        for (int m = 1; m <= 12; ++m) {
            if (month_cnt[m] == 0)
                continue;

            double avg = (double)month_sum[m] / month_cnt[m];

            printf("%-13d%6d%8d%12d%16d%13d%14.2f\n",
                   month_first_line[m],
                   year,
                   m,
                   month_err[m],
                   month_min[m],
                   month_max[m],
                   avg);
        }

        printf("\nСтатистика за год %d:\n", year);
        printf("  Среднегодовая температура = %.2f\n", year_avg);
        printf("  Минимальная температура   = %d\n", year_min);
        printf("  Максимальная температура   = %d\n", year_max);
    }

    return 0;
}