# Компилятор и флаги
CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11
LDFLAGS = -lm        # линковка с libm (для pow и других math-функций)

TARGETS = chat_server chat_client

# По умолчанию собираем цель
all: $(TARGETS)

# Линковка: объектные файлы -> исполняемый файл
chat_server: chat_server.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Линковка: объектные файлы -> исполняемый файл
chat_client: chat_client.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Компиляция main.c в main.o
chat_server.o: chat_server.c common.h
	$(CC) $(CFLAGS) -c chat_server.c -o chat_server.o

# Компиляция main.c в main.o
chat_client.o: chat_client.c common.h
	$(CC) $(CFLAGS) -c chat_client.c -o chat_client.o

# Очистка
clean:
	rm -f *.o $(TARGETS)

.PHONY: all clean

# make				соберёт chat_server и chat_client  		
# ./chat_server		в одном терминале							
# ./chat_client		в другом									
# make clean		удалить .o и бинарники					