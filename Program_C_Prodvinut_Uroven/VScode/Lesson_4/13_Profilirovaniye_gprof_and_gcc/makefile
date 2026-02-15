# Компилятор и флаги
CC     = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -pg

# Имя итоговой программы
TARGET = prog

# Цель по умолчанию — собрать программу
all: $(TARGET)

# Сборка исполняемого файла из объектных файлов
$(TARGET): main.o function.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o function.o

# Компиляция main.c в main.o
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

# Компиляция function.c в function.o
function.o: function.c
	$(CC) $(CFLAGS) -c function.c -o function.o

# Очистка: удалить объектные файлы и исполняемый файл
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean

# собрать программу  make 
# запустить			./prog
# удалить prog и *.o 	make clean