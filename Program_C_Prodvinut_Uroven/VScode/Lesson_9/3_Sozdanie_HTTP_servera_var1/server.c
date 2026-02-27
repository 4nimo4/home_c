/*
//------------------------------------------------------ 
//              Создание HTTP-сервера вариант 1
//------------------------------------------------------
//          Постановка задачи HTTP-сервер
//------------------------------------------------------
Создадим HTTP-сервер, который обрабатывает запросы браузера и возвращает ответ в
виде HTML-страницы.
🕸 HTTP – текстовый протокол для обмена данными между браузером и веб-
сервером.
🕸 Сервер будет принимать запросы клиентов, писать заголовки и тело запроса и
возвращать текстовую HTML-страницу.
🕸 Для выполнения задания воспользуемся кодом сервера, написанного в ходе
практического задания №1.
🕸 Шаги создания сокета, привязки его к адресу, прослушивания соединения и
подключения клиентов аналогичны. Т.е. функции socket(), bind(), listen(), accept()
оставим без изменений.

После установки соединения с сервером, браузер отправляет HTTP-запрос,
который мы получаем через функцию recv(client, buffer, bufsize, 0).

*/

#include <stdio.h>      // printf, perror
#include <string.h>     // memset, strlen
#include <sys/types.h>  // стандартные типы
#include <sys/socket.h> // socket, setsockopt, bind, listen, accept, recv, send
#include <netinet/in.h> // struct sockaddr_in, htons, htonl
#include <arpa/inet.h>  // inet_ntoa
#include <stdlib.h>     // exit
#include <unistd.h>     // close

#define PORT 8001       // TCP-порт сервера
#define BUF_SIZE 1024   // размер буфера

int main(void)
{
    int server;               // дескриптор серверного ("слушающего") сокета
    int client = -1;          // дескриптор сокета подключённого клиента
    char buffer[BUF_SIZE];    // буфер для приёма HTTP-запроса
    int bufsize = BUF_SIZE;   // чтобы использовать имя как в задании

    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);

    // 1. Создаём сокет
    server = socket(AF_INET, SOCK_STREAM, 0);
    printf("HTTP SERVER\n");
    if (server < 0) {
        perror("socket");
        return 1;
    }
    printf("=> Socket server has been created...\n");

    // Разрешаем переиспользовать порт
    int opt = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server);
        return 1;
    }

    // 2. Заполняем структуру адреса
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(PORT);

    // 3. Привязываем сокет к адресу
    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server);
        return 1;
    }

    printf("=> Looking for clients on port %d...\n", PORT);

    // 4. Переводим сокет в режим прослушивания
    if (listen(server, 1) < 0) {
        perror("listen");
        close(server);
        return 1;
    }

    // 5. Принимаем одно подключение
    client = accept(server, (struct sockaddr *)&server_addr, &addrlen);
    if (client < 0) {
        perror("accept");
        close(server);
        return 1;
    }

    printf("=> Connected with client %s\n", inet_ntoa(server_addr.sin_addr));

    // 6. Получаем HTTP-запрос от браузера
    int result = recv(client, buffer, bufsize - 1, 0);
    if (result < 0) {
        // ошибка получения данных
        printf("\n\n=> Connection terminated error %d with IP %s\n",
               result, inet_ntoa(server_addr.sin_addr));
        close(client);
        close(server);
        exit(1);
    }

    if (result == 0) {
        // клиент закрыл соединение, не прислав данных
        printf("\n=> Client disconnected before sending request.\n");
    } else {
        // делаем полученные байты строкой и печатаем запрос (заголовки и т.д.)
        buffer[result] = '\0';
        printf("=== HTTP request from client ===\n%s\n", buffer);

        // 7. Формируем простой HTTP-ответ с HTML-страницей
        char response[BUF_SIZE] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!doctype html>\n"
            "<html><head><title>Test C HTTP Server</title></head>\n"
            "<body>\n"
            "<h1>Test page - УРА!!</h1>\n"
            "<p>This is body of the test page...</p>\n"
            "<h2>Request headers</h2>\n"
            "<em><small>Test C Http Server</small></em>\n"
            "</body></html>\n";

        // 8. Отправляем HTTP-ответ клиенту
        send(client, response, strlen(response), 0);
    }

    // 9. Завершаем работу
    printf("\n=> Connection terminated with IP %s\n", inet_ntoa(server_addr.sin_addr));
    close(client);
    close(server);
    printf("Goodbye...\n");

    return 0;
}