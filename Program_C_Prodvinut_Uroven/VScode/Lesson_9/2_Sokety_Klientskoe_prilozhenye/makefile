# Компилятор и флаги
CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11
LDFLAGS = -lm        # линковка с libm (для pow и других math-функций)

TARGET = prog

# По умолчанию собираем цель
all: $(TARGET)

# Линковка: объектные файлы -> исполняемый файл
$(TARGET): client.o
	$(CC) $(CFLAGS) -o $(TARGET) client.o $(LDFLAGS)

# Компиляция main.c в main.o
main.o: client.c
	$(CC) $(CFLAGS) -c client.c -o client.o

# Очистка
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean

# собрать программу  make 
# запустить			./prog
# удалить prog и *.o 	make clean