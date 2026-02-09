#include <stdio.h>
#include "temp_function.h"

int load_temperature_csv(const char *filename,
                         sensor_t data[],
                         int max_size)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Ошибка открытия файла %s\n", filename);
        return 0;
    }

    int count = 0;
    int line_num = 0;
    char line_buf[256];

    while (fgets(line_buf, sizeof(line_buf), f)) {
        line_num++;

        if (count >= max_size) {
            break;
        }

        data[count].line  = line_num;
        data[count].valid = 0;

        int Y, M, D, H, Min, T;
        int n_scan = sscanf(line_buf, "%d;%d;%d;%d;%d;%d",
                            &Y, &M, &D, &H, &Min, &T);

        if (n_scan != 6) {
            // ошибка формата
            count++;
            continue;
        }

        if (T < -99 || T > 99) {
            // неверная температура
            count++;
            continue;
        }

        // корректная запись
        data[count].year   = Y;
        data[count].month  = M;
        data[count].day    = D;
        data[count].hour   = H;
        data[count].minute = Min;
        data[count].temp   = T;
        data[count].valid  = 1;

        count++;
    }

    fclose(f);
    return count;
}

void print_help(const char *progname)
{
    printf("Использование: %s [ключи]\n", progname);
    printf("Ключи:\n");
    printf("  -h                 показать это сообщение\n");
    printf("  -f file.csv        указать входной CSV-файл\n");
    printf("  -m mm              вывести статистику только за указанный месяц (1-12)\n");
}