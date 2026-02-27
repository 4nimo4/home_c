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
    int sockfd;
    struct sockaddr_in serv_addr;
    uint8_t buf[BUF_SIZE];

    // 1. Создаём сокет
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); return 1; }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_port        = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    printf("Connected to local chat server on 127.0.0.1:%d\n", PORT);
    printf("Type messages, 'exit' to quit.\n");

    char line[BUF_SIZE];

    while (1) {
        printf("You: ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }

        // Уберём '\n' в конце
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }

        if (strcmp(line, "exit") == 0) {
            break;
        }

        // 2. Кодируем (stuffing) и отправляем
        uint8_t encoded[BUF_SIZE];
        size_t elen = stuff_bytes((uint8_t*)line, len, encoded);
        send(sockfd, encoded, elen, 0);

        // 3. Ждём ответ (эхо) от сервера
        ssize_t n = recv(sockfd, buf, BUF_SIZE, 0);
        if (n <= 0) {
            printf("Server closed connection or recv error\n");
            break;
        }

        // В учебном варианте также считаем, что прилетает ровно один кадр.
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
            printf("No complete frame in server response\n");
            continue;
        }

        uint8_t decoded[BUF_SIZE];
        size_t dlen = destuff_bytes(&buf[start], end - start + 1, decoded);
        if (dlen == 0) {
            printf("Destuff error in server response\n");
            continue;
        }

        decoded[dlen] = '\0';
        printf("Server: %s\n", decoded);
    }

    close(sockfd);
    return 0;
}