/*
//------------------------------------------------------ 
//                      LwIP
//------------------------------------------------------
//                 Что такое LwIP?
//------------------------------------------------------
● lwIP (англ. lightweight IP) широко используемый TCP/IP-стек с
открытым исходным кодом, предназначенный для встраиваемых
систем.
● Центром внимания реализации LwIP TCP/IP является сокращение
использования ресурсов и при этом сохранение
полномасштабной TCP. Это делает LwIP возможным для
использования во встраиваемых системах с десятками килобайт
свободной оперативной памяти и с местом для около 40 килобайт
кода ROM.
● LwIP используется многими производителями встраиваемых
систем

//------------------------------------------------------
//                 Особенности lwIP
//------------------------------------------------------
lwIP (англ. lightweight IP) широко используемый TCP/IP-стек с открытым исходным кодом, 
предназначенный для встраиваемых систем. LwIP был первоначально разработан Адамом Дункельсом[англ.] 
в Шведском институте компьютерных наук и в настоящее время разрабатывается и поддерживается 
командой разработчиков.
Центром внимания реализации LwIP TCP/IP является сокращение использования ресурсов и при этом 
сохранение полномасштабной TCP. Это делает LwIP возможным для использования во встраиваемых системах 
с десятками килобайт свободной оперативной памяти и с местом для около 40 килобайт кода ROM.
LwIP используется многими производителями встраиваемых систем, например, Altera 
(в операционных системах для архитектуры Nios II)[2], Analog Devices (процессоры Blackfin)[3][4], 
Xilinx[5] и Honeywell (системы авионики). 
Также на основе lwIP создан TCP/IP драйвер операционной системы ReactOS[6].
Вместе с lwIP Адам Дункелс создал ещё один TCP/IP стек — uIP. 
Этот стек оптимизировали для минимизации используемой памяти, в нём не используется динамическое 
выделение памяти (куча).
LwIP предлагает три различных API-интерфейса, предназначенных для различных целей:
Raw API является основной API LwIP. Этот API направлен на обеспечение наилучших характеристик 
при использовании минимального размера кода. 
Один из недостатков этого API является то, что он обрабатывает асинхронные события с использованием 
обратных вызовов, которые усложняют разработку приложений.
Netconn API представляет собой последовательный API интерфейс построенный на базе Raw API. 
Это позволяет выполнять многопоточную обработку данных, следовательно, 
требуется наличие операционной системы. Это проще в использовании, чем Raw API 
за счет более низкой реализации исполнения и увеличения объема памяти.
BSD API сокеты, сокеты Беркли, как сокет реализации (Posix / BSD), построенный на базе API Netconn. 
Данный факт повышает переносимость кода. Он имеет те же недостатки, что и API Netconn.


эти библиотеки 

#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/timeouts.h"

VScode не видит сейчас, потому что их должен сформировать 
CubeIDE после создания проекта для STM32F4Discovery 
только после этого можно будет собрать проект
*/


#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/timeouts.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define HTTP_PORT 8002
#define BUF_SIZE  4096

static struct tcp_pcb *http_pcb = NULL;

// Состояния "виртуальных" светодиодов
static int led1_state = 0;
static int led2_state = 0;

// Прототипы внутренних функций
static err_t http_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

/* Инициализация HTTP-сервера (аналог socket+bind+listen) */
void http_server_init(void)
{
    err_t err;

    http_pcb = tcp_new();
    if (http_pcb == NULL) {
        printf("http_server_init: tcp_new failed\n");
        return;
    }

    err = tcp_bind(http_pcb, IP_ADDR_ANY, HTTP_PORT);
    if (err != ERR_OK) {
        printf("http_server_init: tcp_bind failed: %d\n", err);
        tcp_close(http_pcb);
        http_pcb = NULL;
        return;
    }

    http_pcb = tcp_listen(http_pcb);
    tcp_accept(http_pcb, http_accept);

    printf("HTTP server (LwIP) listening on port %d\n", HTTP_PORT);
}

/* Колбэк при новом входящем соединении (аналог accept) */
static err_t http_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    printf("=> New client connected\n");

    // Можно задать пользовательский контекст, если нужно
    tcp_arg(newpcb, NULL);

    // Регистрируем обработчик приёма данных
    tcp_recv(newpcb, http_recv);

    return ERR_OK;
}

/* Колбэк приёма данных (аналог recv + твоя HTTP-логика) */
static err_t http_recv(void *arg, struct tcp_pcb *tpcb,
                       struct pbuf *p, err_t err)
{
    LWIP_UNUSED_ARG(arg);

    // Если p == NULL, клиент закрыл соединение
    if (p == NULL) {
        printf("=> Client closed connection\n");
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    if (err != ERR_OK) {
        pbuf_free(p);
        return err;
    }

    char buffer[BUF_SIZE];

    if (p->tot_len >= BUF_SIZE) {
        printf("Request too big, dropping\n");
        pbuf_free(p);
        return ERR_BUF;
    }

    // Копируем из pbuf в обычный буфер
    pbuf_copy_partial(p, buffer, p->tot_len, 0);
    buffer[p->tot_len] = '\0';

    printf("=== HTTP request from client ===\n%s\n", buffer);

    // Сообщаем стеку, что мы приняли эти данные
    tcp_recved(tpcb, p->tot_len);

    // ---- ЛОГИКА КНОПОК / СОСТОЯНИЙ (как в твоем Linux-сервере) ----
    if (strncmp(buffer, "GET ", 4) == 0) {
        if (strstr(buffer, "GET /on1 ") != NULL) {
            led1_state = 1;
            printf(">>> BUTTON: LED1 ON\n");
        } else if (strstr(buffer, "GET /off1 ") != NULL) {
            led1_state = 0;
            printf(">>> BUTTON: LED1 OFF\n");
        } else if (strstr(buffer, "GET /on2 ") != NULL) {
            led2_state = 1;
            printf(">>> BUTTON: LED2 ON\n");
        } else if (strstr(buffer, "GET /off2 ") != NULL) {
            led2_state = 0;
            printf(">>> BUTTON: LED2 OFF\n");
        } else {
            printf(">>> Unknown path (main page or favicon)\n");
        }
    }

    const char *led1_text  = led1_state ? "ON"  : "OFF";
    const char *led1_color = led1_state ? "green" : "red";
    const char *led2_text  = led2_state ? "ON"  : "OFF";
    const char *led2_color = led2_state ? "green" : "red";

    // Формируем HTTP-ответ (как в Linux-версии)
    char response[BUF_SIZE];
    int n = snprintf(
        response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<!DOCTYPE HTML>"
        "<html>"
        " <head>"
        "   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
        "   <title>LwIP Web Server</title>"
        "   <style>"
        "     body { font-family: Arial; }"
        "     .state { font-weight: bold; }"
        "   </style>"
        " </head>"
        " <body>"
        " <h1>LwIP Web Server (training)</h1>"

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
        led1_color, led1_text,
        led2_color, led2_text
    );

    if (n < 0 || n >= (int)sizeof(response)) {
        printf("Response truncated or error in snprintf\n");
    }

    // Отправка HTTP-ответа
    err_t wr_err = tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
    if (wr_err == ERR_OK) {
        tcp_output(tpcb);
    } else {
        printf("tcp_write error: %d\n", wr_err);
    }

    // Освобождаем pbuf
    pbuf_free(p);

    // Закрываем соединение (один запрос — один ответ)
    tcp_close(tpcb);

    return ERR_OK;
}