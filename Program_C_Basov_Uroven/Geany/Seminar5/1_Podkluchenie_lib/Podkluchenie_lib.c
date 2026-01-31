/*
    Пример работы с файловой системой
//--------------------------------------------------------------------------
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//--------------------------------------------------------------------------
Данная программа показывает содержимое каталогов(папок- что в них находится)
имена папок в пути желательно чтобы были на английском, иначе выдаст ошибку!!!!
//---------------------------------------------------------------------------
Как запускать программу? - ./a.out -f /путь/к/папке
или просто - ./a.out (по умолчанию список текущего каталога).
//---------------------------------------------------------------------------
Подключение библиотек
    
getcwd - Получает имя текущего рабочего каталога
char *getcwd(char *buf, size_t size);
Функция getcwd() копирует абсолютный путь к текущему
рабочему каталогу в массиве, на который указывает buf,
имеющий длину size.
📙 Возвращает строку (с null в конце), содержащую путь к
каталогу.
📙 Если текущий рабочий каталог — корень, строка
заканчивается обратной косой чертой.
📙 Если текущий рабочий каталог отличается от корневого,
строка заканчивается именем каталога, а не обратной
косой чертой.
📙 Библиотека языка C GNU glibc: Интерфейсы Файловой
системы

*/
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <locale.h>
#include <bits/getopt_core.h>

enum {PATH_LENGTH=256};

#define STR255 "%255s"
#define LINUX 0

//  getcwd - Получает имя текущего рабочего каталога
void convert_path_to_full(char *full_path,const char *dir) 
{
    if(dir[0]=='/') 
    {
        strcpy(full_path, dir);
    } 
    else if (dir[0]=='.') 
    {
        getcwd(full_path,PATH_LENGTH);
    }
    else 
    {
        getcwd(full_path,PATH_LENGTH);
        strcat(full_path, "/");
        strcat(full_path, dir);
    }
    if(full_path[strlen(full_path)-1] !='/')
    {
        strcat(full_path,"/");// добавляем /в конце
    }      
}
//  Примечание: для Windows не работает
#if defined __linux__

#ifndef DT_LNK
#define DT_LNK 10
#endif
#ifndef DT_FIFO
#define DT_FIFO 1
#endif
#ifndef DT_CHR
#define DT_CHR 2
#endif
#ifndef DT_DIR
#define DT_DIR 4
#endif
#ifndef DT_BLK
#define DT_BLK 6
#endif
#ifndef DT_SOCK
#define DT_SOCK 12
#endif


void print_filetype(int type) 
{
    switch (type) 
    {
        case DT_BLK: printf("b "); 
            break;
        case DT_CHR: printf("c "); 
            break;
        case DT_DIR: printf("d "); //directory
            break; 
        case DT_FIFO: printf("p "); //fifo
            break; 
        case DT_LNK: printf("l "); //Sym link
            break; 
        case DT_SOCK: printf("s "); //Filetype isn't identified
            break; 
        default:
        printf(" "); 
            break;
    }
}
#endif

/*
 *  Дополнить пробелами и табами
Расширить строку пробелами.
@print_lenth длина до которой надо расширить
*/
void print_space(int print_lenth, int str_lenth)
{
    while( (print_lenth - str_lenth)>0 )
    {
        putchar(' ');
        str_lenth++;
    }
}
void print_tab(int tab_number)
{
    for(int t=1; t<tab_number; t++)
    putchar('\t');
}

//  Печатаем размер файла
void print_file_size(long long int byte_number) 
{
    if(byte_number/(1024*1024))
    {
        printf("%lld Mb", byte_number/(1024*1024));
    }    
    else if(byte_number/1024)
    {
        printf("%lld Kb", byte_number/1024);
    }    
    else
    {
        printf("%lld b", byte_number);
    } 
}
//  Функции opendir и readdir
/*
opendir - открывает поток каталога, соответствующий каталогу name, и возвращает
указатель на этот поток. Поток устанавливается на первой записи в каталоге.
DIR *opendir(const char *name);
readdir - представляет собой поток каталогов, представляющий собой упорядоченную
последовательность всех записей каталога в определенном каталоге.
struct dirent *readdir(DIR *dirp);
Функция readdir () возвращает указатель на структуру, представляющую запись
каталога в текущей позиции в потоке каталогов, заданном аргументом dirp , и
устанавливает поток каталогов на следующую запись. При достижении конца потока
каталогов она возвращает нулевой указатель
*/
void ls(const char *dir) 
{
    static int tab_count = 0; //уровень вложенности рекурсии
    tab_count++;
    struct stat file_stats;
    DIR *folder;
    struct dirent *entry;
    int files_number = 0;
    char full_path[PATH_LENGTH]={0};
    convert_path_to_full(full_path, dir);
    folder = opendir(full_path);
    if(folder == NULL)
    {
        perror("Unable to read directory");
        printf("%s\n",full_path);
        return;
    }
    while( (entry=readdir(folder)) ) 
    {
        if( entry->d_name[0]=='.' )//пропускаем поддиректории
        {
            continue;
        }
            
        char full_filename[PATH_LENGTH]={0};
        files_number++;
        print_tab(tab_count);//отступы
        printf("%4d : ",files_number); 
#if defined __linux__
        //не работает для Windows
        print_filetype(entry->d_type);
#endif
        strcpy(full_filename, full_path);
        strcat(full_filename, entry->d_name);
        printf("%s", entry->d_name);
        print_space(20, strlen(entry->d_name));
        if (!stat(full_filename, &file_stats))
        {
            print_file_size(file_stats.st_size);
            printf("\n");
        }
        else 
        {
            printf("stat failed for this file\n");
            perror(0);
        }
    }
    closedir(folder);
    tab_count--;
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, ".UTF-8");
    char dir[PATH_LENGTH] = {0};
    char buf[PATH_LENGTH];

    int rez;
    while ((rez = getopt(argc, argv, "hf:")) != -1) {
        switch (rez) {
            case 'h':
                printf("This is example of list directory\n");
                printf("Usage: %s -f <folder>\n", argv[0]);
                printf("Options:\n");
                printf("  -h        Show help\n");
                printf("  -f <dir>  Specify folder to list\n");
                return 0;
            case 'f':
                if (optarg != NULL) {
                    strcpy(dir, optarg);
                } else {
                    printf("Ошибка: опция -f требует аргумент.\n");
                    return 1;
                }
                break;
            case '?':
                printf("Unknown argument: %s. Try -h for help.\n", argv[optind - 1]);
                return 1;
        }
    }

    // Если пользователь не передал -f, используем текущий каталог
    if (strlen(dir) == 0) {
        strcpy(dir, ".");
    }

    convert_path_to_full(buf, dir);
    printf("is for folder %s\n", buf);
    ls(buf);
    return 0;
}