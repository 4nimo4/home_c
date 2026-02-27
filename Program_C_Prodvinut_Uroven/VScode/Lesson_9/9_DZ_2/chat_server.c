/*
//------------------------------------------------------
//              Домашнее задание
//-----------------------------------------------------
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"

#define PORT 9000
#define BUF_SIZE 1024

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    uint8_t buf[BUF_SIZE];

    // 1. Создаём TCP-сокет
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Адрес
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Byte-stuffing local chat server listening on port %d\n", PORT);

    // 3. Ждём одного клиента
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
    printf("Type messages in client; server will print and echo them.\n");
    printf("Stop server with Ctrl+C.\n");

    // 4. Основной цикл: принимаем кадры, декодируем, печатаем и эхо-отправляем
    while (1) {
        ssize_t n = recv(client_fd, buf, BUF_SIZE, 0);
        if (n <= 0) {
            printf("Client disconnected or recv error\n");
            break;
        }

        // В простом варианте считаем, что в одном recv() приходит ровно один кадр.
        // Для реального протокола нужно буферизовать и искать SOP/EOP.
        int start = -1, end = -1;
        for (ssize_t i = 0; i < n; ++i) {
            if (buf[i] == SOP) { start = i; break; }
        }
        if (start >= 0) {
            for (ssize_t j = start + 1; j < n; ++j) {
                if (buf[j] == EOP) { end = j; break; }
            }
        }

        if (start < 0 || end < 0) {
            printf("No complete frame in this recv\n");
            continue;
        }

        uint8_t decoded[BUF_SIZE];
        size_t dlen = destuff_bytes(&buf[start], end - start + 1, decoded);
        if (dlen == 0) {
            printf("Destuff error\n");
            continue;
        }

        decoded[dlen] = '\0';
        printf("Client: %s\n", decoded);

        // Эхо: отправим обратно клиенту то же сообщение (снова закодировав)
        uint8_t encoded[BUF_SIZE];
        size_t elen = stuff_bytes(decoded, dlen, encoded);
        send(client_fd, encoded, elen, 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}