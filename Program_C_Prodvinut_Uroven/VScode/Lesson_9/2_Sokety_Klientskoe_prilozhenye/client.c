/*
//------------------------------------------------------ 
//          Клиентское приложение
//------------------------------------------------------
Для создания приложения «Локальный чат» на архитектуре
«клиент-сервер» на этапе соединения на клиенте нам нужно
выполнить следующие шаги:
● Инициализация переменных клиента
● Установка сокет соединения
● Инициализация структуры server_addr номером порта и IP
адреса сервера
● Установка соединения

*/

#include <stdio.h>      // printf, perror, getchar
#include <stdbool.h>    // тип bool
#include <string.h>     // memset, strlen
#include <sys/types.h>  // стандартные типы (ssize_t и др.)
#include <sys/socket.h> // socket, connect, send, recv
#include <netinet/in.h> // struct sockaddr_in, htons
#include <arpa/inet.h>  // inet_pton, inet_ntoa
#include <stdlib.h>     // exit, стандартные функции
#include <unistd.h>     // close

#define PORT 1500       // номер порта сервера
#define BUF_SIZE 1024   // размер буфера для обмена данными

int main(void)
{
    int client_fd;          // дескриптор клиентского сокета
    bool isExit = false;    // флаг завершения основного цикла
    char buffer[BUF_SIZE];  // буфер для отправки/приёма сообщений
    const char *ip = "127.0.0.1";   // IP-адрес сервера (loopback — локальная машина)

    struct sockaddr_in server_addr; // структура с адресом сервера

    // 1. Создаём сокет
    // AF_INET      — IPv4
    // SOCK_STREAM  — потоковый сокет (TCP)
    // 0            — протокол по умолчанию (TCP)
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("CLIENT\n");
    if (client_fd < 0) {
        perror("socket");
        return 1;
    }
    printf("=> Socket client has been created...\n");

    // 2. Заполняем структуру адреса сервера
    memset(&server_addr, 0, sizeof(server_addr)); // обнуляем структуру
    server_addr.sin_family = AF_INET;             // IPv4
    server_addr.sin_port   = htons(PORT);         // порт сервера в сетевом порядке байт

    // Переводим строковый IP-адрес в двоичный вид и записываем в sin_addr
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(client_fd);
        return 1;
    }

    // 3. Устанавливаем соединение с сервером
    // connect связывает наш сокет с указанным адресом сервера.
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_fd);
        return 1;
    }

    printf("=> Connected to server %s with port %d\n",
           inet_ntoa(server_addr.sin_addr), PORT);
    printf("=> Type '#' to end the connection.\n\n");

    // 4. Основной цикл общения с сервером
    // Клиент:
    //  1) читает строку с клавиатуры;
    //  2) отправляет её серверу;
    //  3) ждёт ответ и печатает его.
    // Выход — если сообщение клиента или сервера начинается с '#'.
    while (!isExit) {
        ssize_t nbytes;

        // -------- СООБЩЕНИЕ КЛИЕНТА (ввод через getchar) --------
        printf("Client: ");
        fflush(stdout);  // сразу выводим подсказку

        int cntr = 0;
        int ch;

        // Читаем посимвольно до '\n' или EOF, но не более BUF_SIZE - 1 символов
        while ((ch = getchar()) != '\n' && ch != EOF && cntr < BUF_SIZE - 1) {
            buffer[cntr++] = (char)ch;
        }
        buffer[cntr] = '\0';  // завершаем строку

        if (cntr == 0) {
            // Пустая строка (просто Enter) — пропускаем и начинаем следующую итерацию
            continue;
        }

        // Отправляем введённую строку серверу
        send(client_fd, buffer, strlen(buffer), 0);

        // Если первое введённое пользователем значение — '#',
        // считаем это командой завершения.
        if (buffer[0] == '#') {
            isExit = true;
            break;
        }

        // -------- ЧТЕНИЕ ОТВЕТА ОТ СЕРВЕРА --------
        printf("Server: ");
        fflush(stdout);

        // recv блокируется, пока не придёт ответ от сервера
        nbytes = recv(client_fd, buffer, BUF_SIZE - 1, 0);
        if (nbytes <= 0) {
            // nbytes == 0 — сервер корректно закрыл соединение
            // nbytes < 0  — ошибка при чтении
            if (nbytes < 0) perror("recv");
            printf("\n=> Server disconnected.\n");
            break;
        }

        // Преобразуем принятые байты в C-строку
        buffer[nbytes] = '\0';
        printf("%s\n", buffer);

        // Если сервер прислал строку, начинающуюся с '#',
        // клиент тоже завершает работу.
        if (buffer[0] == '#') {
            isExit = true;
            break;
        }
    }

    printf("\n=> Connection terminated.\nGoodbye...\n");
    close(client_fd);   // закрываем сокет клиента
    return 0;
}