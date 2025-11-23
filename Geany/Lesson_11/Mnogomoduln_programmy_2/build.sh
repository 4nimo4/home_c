#Решеткой обозначаются комментарии в данном типе файлов

#!/bin/bash
#chmod +x build.sh
# Компиляция с предупреждениями
gcc -Wall -c -o main.o main.c
gcc -Wall -c -o func.o func.c

# Получение ассемблерного кода 
#gcc -S -o main.s main.c
#gcc -S -o func.s func.c

# Связывание
gcc -Wall -o prog main.o func.o
