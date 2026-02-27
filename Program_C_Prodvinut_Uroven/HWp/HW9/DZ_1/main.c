/*
//------------------------------------------------------ 
//              Подведем итоги
//------------------------------------------------------
● Сформировали базовые навыки работы с протоколами
TCP/IP и обработкой данных на примере Raspberry Pi,
которые могут быть полезны при создании других сетевых
приложений и IoT-решений на примере приложения
"Локальный чат"
● Изучили работу TCP-сокетов на практическом примере
● Выполнили оптимизацию кода приложения чата
● Поняли принцип работы веб-сервера, HTTP-протокола,
обработку запросов, генерацию и отправку HTTP-ответов
на базе TCP-сокетов
● Получили начальные навыки разработки web-интерфейсов

//------------------------------------------------------
//              Домашнее задание
//------------------------------------------------------
1. Используя практическое задание №2 необходимо «разобрать» кнопку, т.е.
отправлять в консоль данные ее нажатия.

3.* Написать приложение «Локальный чат» на архитектуре «клиент-сервер»
с использованием байт-стаффинга (byte stuffing).
    -------------------------------------------------------------
    |    МНП          |   Сегмент данных    |      МКП          |
    -------------------------------------------------------------
    |    1 байт       |   Сообщения чата    |      1 байт       |
    -------------------------------------------------------------
    |    SOP = 0x7E   |                     |      EOP = 0x7E   |
    -------------------------------------------------------------
Где:
МНП (SOP) – маркер начала пакета;
МКП (EOP)– маркер конца пакета;
Сегмент данных не содержит кодов МНП и МКП 
(достигается применением вставки байт). Если в передаваемых данных 
встречается код 0x7E, то он заменяется последовательностью 0x7D, 0x5E.
Если в передаваемых данных встречается код 0x7D, 
то он заменяется последовательностью 0x7D, 0x5D.
То есть:
0x7E = вставить 0x7D, заменить 0x7E на ( 0x7E xor 0x20) = 0x7D, 0x5E
0x7D = вставить 0x7D, заменить 0x7D на ( 0x7D xor 0x20) = 0x7D, 0x5D
При приеме данных происходит обратная замена
0x7D, 0x5E = отбросить 0x7D, заменить 0x5E на ( 0x5E xor 0x20) = 0x7E.
0x7D, 0x5D = отбросить 0x7D, заменить 0x5D на ( 0x5D xor 0x20) = 0x7D.

*/

#include <stdio.h>      // printf, perror
#include <string.h>     // memset, strlen, strncmp, strstr
#include <sys/types.h>  // базовые типы (ssize_t и др.)
#include <sys/socket.h> // socket, setsockopt, bind, listen, accept, recv, send
#include <netinet/in.h> // struct sockaddr_in, htons, htonl
#include <arpa/inet.h>  // inet_ntoa
#include <stdlib.h>     // exit, malloc, free (если нужно)
#include <unistd.h>     // close
#include <stdbool.h>    // тип bool (true/false)

#define PORT 8002       // TCP-порт, на котором будет работать HTTP-сервер
#define BUF_SIZE 4096   // размер буфера для приёма и отправки (запрос+ответ)

/*
 * Простейший HTTP-сервер:
 *  - принимает HTTP-запросы от браузера;
 *  - по URL /on1, /off1, /on2, /off2 меняет состояние "виртуальных светодиодов";
 *  - возвращает HTML-страницу с кнопками и отображением текущего состояния.
 * Завершение работы сервера осуществляется по Ctrl+C в консоли.
 */

int main(void)
{
    int server;                 // дескриптор серверного (слушающего) сокета
    int client = -1;            // дескриптор сокета подключившегося клиента
    char buffer[BUF_SIZE];      // буфер для приёма HTTP-запроса
    int bufsize = BUF_SIZE;     // размер буфера, используем как отдельную переменную

    // Переменные для хранения состояния "светодиодов":
    // 0 = выключен (OFF), 1 = включен (ON)
    int led1_state = 0;
    int led2_state = 0;

    struct sockaddr_in server_addr; // структура с адресом сервера (IP+порт)
    struct sockaddr_in client_addr; // структура с адресом клиента
    socklen_t addrlen = sizeof(client_addr);

    // 1. Создаём TCP-сокет: IPv4 (AF_INET), потоковый (SOCK_STREAM), протокол по умолчанию (0 → TCP)
    server = socket(AF_INET, SOCK_STREAM, 0);
    printf("HTTP SERVER\n");
    if (server < 0) 
    {
        perror("socket");       // выводим описание системной ошибки
        return 1;
    }
    printf("=> Socket server has been created...\n");

    // Разрешаем переиспользовать порт (чтобы не ждать TIME_WAIT при перезапуске сервера)
    int opt = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        perror("setsockopt");
        close(server);
        return 1;
    }

    // 2. Заполняем структуру адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));         // обнуляем структуру
    server_addr.sin_family      = AF_INET;                // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);      // слушать на всех интерфейсах (0.0.0.0)
    server_addr.sin_port        = htons(PORT);            // порт в сетевом порядке байт

    // 3. Привязываем сокет к адресу/порту
    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("bind");
        close(server);
        return 1;
    }

    printf("=> Listening on port %d...\n", PORT);

    // 4. Переводим сокет в режим прослушивания (серверный режим)
    if (listen(server, 5) < 0) 
    {
        perror("listen");
        close(server);
        return 1;
    }

    printf("=> Open http://127.0.0.1:%d/ in browser.\n", PORT);
    printf("=> Stop server with Ctrl+C in this console.\n");

    // Основной бесконечный цикл: принимаем клиентов и обрабатываем их по одному
    while (1) 
    {
        printf("\n=> Waiting for client...\n");

        // 5. Ожидаем входящее подключение (блокирующий вызов)
        client = accept(server, (struct sockaddr *)&client_addr, &addrlen);
        if (client < 0) 
        {
            perror("accept");
            break;  // выходим из цикла при ошибке accept
        }

        printf("=> Connected with client %s\n", inet_ntoa(client_addr.sin_addr));

        // 6. Принимаем HTTP-запрос от клиента
        // recv блокируется, пока не придут данные
        int result = recv(client, buffer, bufsize - 1, 0);
        if (result <= 0) 
        {
            // 0 — клиент корректно закрыл соединение
            // <0 — ошибка
            printf("=> recv error or client disconnected early\n");
            close(client);
            continue; // переходим к ожиданию следующего клиента
        }

        // Делаем из полученных байт C-строку
        buffer[result] = '\0';
        //char *first_line_end = strstr(buffer, "\r\n");
        //if (first_line_end) 
        //{
        //    *first_line_end = '\0';
        //}
        printf("=== HTTP request from client ===\n%s\n", buffer);

        // ---- ОБРАБОТКА КНОПОК / ИЗМЕНЕНИЕ СОСТОЯНИЯ ----
        // Запрос HTTP имеет первую строку вида: "GET /on1 HTTP/1.1"
        // Мы проверяем, какой именно путь был запрошен.
        if (strncmp(buffer, "GET ", 4) == 0) 
        {
            if (strstr(buffer, "GET /on1 ") != NULL) 
            {
                led1_state = 1;
                printf(">>> BUTTON: LED1 ON\n");
            } else if (strstr(buffer, "GET /off1 ") != NULL) 
            {
                led1_state = 0;
                printf(">>> BUTTON: LED1 OFF\n");
            } else if (strstr(buffer, "GET /on2 ") != NULL) 
            {
                led2_state = 1;
                printf(">>> BUTTON: LED2 ON\n");
            } else if (strstr(buffer, "GET /off2 ") != NULL) 
            {
                led2_state = 0;
                printf(">>> BUTTON: LED2 OFF\n");
            } else 
            {
                // Например, запрос корня "/" или favicon.ico
                printf(">>> Unknown path (main page or favicon)\n");
            }
        }

        // Подготовим отображение текущего состояния
        const char *led1_text  = led1_state ? "ON"  : "OFF";
        const char *led1_color = led1_state ? "green" : "red";

        const char *led2_text  = led2_state ? "ON"  : "OFF";
        const char *led2_color = led2_state ? "green" : "red";

        // ---- СФОРМИРОВАТЬ HTTP-ОТВЕТ С HTML-СТРАНИЦЕЙ ----
        char response[BUF_SIZE];

        // snprintf собирает HTTP-заголовки и HTML-страницу в одну строку
        int n = snprintf(response, sizeof(response),
            // Стартовая строка и заголовки HTTP-ответа
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Connection: close\r\n"
            "\r\n"
            // Тело ответа (HTML-документ)
            "<!DOCTYPE HTML>"
            "<html>"
            " <head>"
            "   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
            "   <title>ESP32 - Web Server</title>"
            "   <style>"
            "     body { font-family: Arial; }"
            "     .state { font-weight: bold; }"
            "   </style>"
            " </head>"
            " <body>"
            " <h1>ESP32 - Web Server</h1>"

            " <p>LED #1 "
            "  <span class=\"state\" style=\"color:%s;\">[%s]</span><br>"
            "  <a href=\"/on1\"><button>ON</button></a>&nbsp;"
            "  <a href=\"/off1\"><button>OFF</button></a>"
            " </p>"

            " <p>LED #2 "
            "  <span class=\"state\" style=\"color:%s;\">[%s]</span><br>"
            "  <a href=\"/on2\"><button>ON</button></a>&nbsp;"
            "  <a href=\"/off2\"><button>OFF</button></a>"
            " </p>"

            " </body>"
            "</html>",
            led1_color, led1_text,   // подставляем цвет и текст для LED1
            led2_color, led2_text    // подставляем цвет и текст для LED2
        );

        // Проверка на обрезку строки (на случай, если BUF_SIZE мало)
        if (n < 0 || n >= (int)sizeof(response)) 
        {
            fprintf(stderr, "Response truncated or error in snprintf\n");
        }

        // 7. Отправляем HTTP-ответ клиенту
        send(client, response, strlen(response), 0);

        // 8. Закрываем клиентский сокет (соединение на один запрос)
        close(client);
        printf("=> Client connection closed.\n");
    }

    // 9. Закрываем серверный сокет перед выходом
    close(server);
    printf("Goodbye...\n");
    return 0;
}