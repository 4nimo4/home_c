#ifndef TEMP_API_H
#define TEMP_API_H

#include <stdlib.h>

#define MAX_RECORDS 600000  // достаточно для большого файла

typedef struct sensor {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int temp;    // -99..99
    int line;    // номер строки в файле
    int valid;   // 1 - корректная запись, 0 - ошибка
} sensor_t;

int load_temperature_csv(const char *filename,
                         sensor_t data[],
                         int max_size);

void print_help(const char *progname);

#endif