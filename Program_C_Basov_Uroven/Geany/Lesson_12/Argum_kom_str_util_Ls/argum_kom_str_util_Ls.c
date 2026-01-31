#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <bits/getopt_core.h>

#define PATH_LENGTH 255
/*
//-----------------------------------------------------

                        // opterr=0;  // Опция выключена

Вводим команды:
    gcc argum_kom_str_util_Ls.c
    ./a.out
    ./a.out -h
Вывод:
    This is example of list directory
    Usage: clear [options]
                            -h This help text
                            -f Specify folder.
    Example: ./a.out -f/tmp
Вводим:
    ./a.out -a -b 2 -h
Вывод: -
    found argument "a".
    found argument "b = 2".
    ./a.out: invalid option -- 'h'
    Error found !

добавим сторки:
    //Если я ничего не ввел, то по крайней мемре ,ч тобы , что нибудь вывалилось!
    if(argc == 1)
        {
            printf("Usage: clear [options]\n\
                    -h This help text\n\
                    -f Specify folder.\n");
        }
Пересоберем:
    gcc argum_kom_str_util_Ls.c
Вводим:
    ./a.out
Вывод:
    Usage: clear [options]
                    -h This help text
                    -f Specify folder.

Вводим:
    ./a.out -g
Вывод:
    ./a.out: invalid option -- 'g'
    Unknown argument: -g Try -h for help
Вводим:
    ./a.out -f
Вывод:
    ./a.out: option requires an argument -- 'f'
    Unknown argument: -f Try -h for help
Вводим:
    ./a.out -f /
Вывод:
    folder is "f = /".
*/
//-------------------------------
int main(int argc, char *argv[])
{
    char dir[PATH_LENGTH], buf[PATH_LENGTH];
    int rez=0;
    //opterr=0;
    //Если я ничего не ввел, то по крайней мемре ,ч тобы , что нибудь вывалилось!
    if(argc == 1)
        {
            printf("Usage: clear [options]\n\
                    -h This help text\n\
                    -f Specify folder.\n");
        }
    while ( (rez = getopt(argc,argv,"hf:")) != -1)
        {
            switch (rez)
            {
                case 'h':
                        printf("This is example of list directory\n");
                        printf("Usage: clear [options]\n\
                            -h This help text\n\
                            -f Specify folder.\n");
                        printf("Example: %s -f/tmp\n",argv[0]);
                        return 0;
                case 'f':
                        printf("folder is \"f = %s\".\n",optarg);
                        strcpy(dir, optarg);
                break;
                case '?':
                    printf("Unknown argument: %s Try -h for help\n", argv[optind-1]);
                return 1;
            }
        }
        // convert_path_to_full(buf, dir);
        //printf("ls for folder %s\n",buf);
    // ls(dir);
    return 0;
}
