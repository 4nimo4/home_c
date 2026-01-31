//--------------------------------------------------------
//          Пример структуры заголовка ТСP
//--------------------------------------------------------
/* пояснения по смыслу полей IP‑заголовка (это не TCP, 
а именно заголовок IP v4):        
//--------------------------------------------------------
*/
/*
#include <stdio.h>
#include <stdint.h>

int main(void) 
{

//--------------------------------------------------------
//                пояснения по смыслу       
//--------------------------------------------------------

//Покажу два распространённых варианта IP_Address
//и поясню, когда какой удобнее.
//-----------------------------------------------------------------
//Вариант 1: как 4 отдельных байта (наглядно)
//-----------------------------------------------------------------

    typedef struct
    {
        uint8_t b1; // первый октет, например 192
        uint8_t b2; // второй октет, например 168
        uint8_t b3; // третий октет, например 0
        uint8_t b4; // четвёртый октет, например 1
    } IP_Address;
    //Использование:
    IP_Address ip = {192, 168, 0, 1};
    printf("%u.%u.%u.%u\n", ip.b1, ip.b2, ip.b3, ip.b4);
    //Так удобнее «человечески» работать с адресом.
//---------------------------------------------------------
//  Вариант 2: как одно 32-битное число
//---------------------------------------------------------
typedef struct
{
    uint32_t addr; // адрес в сетевом порядке байт (big-endian)
} IP_Address;
//Использование (с учётом порядка байт):
IP_Address ip;
ip.addr = htonl(0xC0A80001); // 192.168.0.1, если используете сокеты/стек TCP/IP
//Так удобнее, если вы часто передаёте/сравниваете адреса как целые числа.

//---------------------------------------------------------
    // Директива препроцессора #pragma pack(push, 1)
    // говорит компилятору:
    //  - "сохрани текущие настройки выравнивания" (push)
    //  - "выравнивай поля структуры по 1 байту" (pack(1))
    //
    // То есть компилятор не будет добавлять "пустые" байты (паддинг)
    // между полями, и структура будет плотно соответствовать формату
    // сетевого заголовка. 
    #pragma pack(push, 1)

    // Описание структуры заголовка IP.
    // volatile — компилятор не будет кэшировать значения полей,
    // предполагается, что данные могут изменяться "извне"
    // (например, при доступе к памяти, связанной с оборудованием).
    typedef volatile struct 
    {
        // Первые 8 бит заголовка:
        //  - Version (4 бита) — версия протокола IP (обычно 4 для IPv4)
        //  - IHL (4 бита) — Internet Header Length, длина заголовка IP в 32-битных словах.
        //
        // Порядок битовых полей (что реально окажется старшими/младшими битами байта)
        // зависит от архитектуры и компилятора, поэтому для реальных протоколов
        // такой подход с битовыми полями может быть НЕПЕРЕНОСИМ.
        unsigned IHL     : 4;  // Длина заголовка IP (в 32-битных словах)
        unsigned Version : 4;  // Версия протокола IP (обычно 4)

        // ServType (Type of Service / DSCP/ECN) — байт с приоритетом/типом сервиса.
        uint8_t ServType;

        // Общая длина IP-пакета (заголовок + данные), в байтах.
        uint16_t PacketLength;

        // Идентификатор пакета — используется для фрагментации/сборки.
        uint16_t ID;

        // Следующие 16 бит заголовка:
        //  - флаги (3 бита: Zero, DF, MF)
        //  - смещение фрагмента (Offset, 13 бит)
        //
        // В классическом формате:
        //  bit 0 (старший): Reserved (Zero)
        //  bit 1: DF (Don't Fragment)
        //  bit 2: MF (More Fragments)
        //  bits 3–15: Fragment Offset
        //
        // Здесь они описаны битовыми полями. Опять же:
        // точное расположение в памяти зависит от реализации.
        unsigned Offset : 13;  // Смещение фрагмента (в блоках по 8 байт)
        unsigned MF     : 1;   // More Fragments — есть ли ещё фрагменты
        unsigned DF     : 1;   // Don't Fragment — запрещать фрагментацию
        unsigned Zero   : 1;   // Зарезервированный бит (обычно = 0)

        // TTL (Time To Live) — "время жизни" пакета (количество хопов).
        uint8_t TTL;

        // Protocol — номер протокола верхнего уровня (TCP = 6, UDP = 17 и т.д.).
        uint8_t Protocol;

        // CheckSum — контрольная сумма заголовка IP.
        uint16_t CheckSum;

        // IP-адрес отправителя и получателя.
        // Тип IP_Address здесь предполагается заранее объявленным,
        // например:
        //   typedef struct { uint8_t b1, b2, b3, b4; } IP_Address;
        // или uint32_t в сетевом порядке байт.
        IP_Address SrcIP;  // IP-адрес источника
        IP_Address DstIP;  // IP-адрес назначения

    } IP_Header;

    // Восстанавливаем предыдущее выравнивание структур.
    #pragma pack (pop)

    return 0;
}
*/

//-----------------------------------------------------------------
//Ниже разбор IP‑заголовка «по‑правильному» — без битовых полей и без
//#pragma pack , только через массив байтов, сдвиги 
//Будем считать, что в buf лежит минимум 20 байт IPv4‑заголовка без опций.
//-----------------------------------------------------------------
//Структура без битовых полей
//-----------------------------------------------------------------
//Такой подход:не использует struct с битовыми полями;
//не зависит от #pragma pack и выравнивания;
//явно учитывает сетевой порядок байт;
//переносим и предсказуем на разных платформах и компиляторах.

//------------------------------------------------------------------------
//Если не хочется использовать приведение к uint16_t *
//, можно собирать 16‑битные значения вручную — это ещё более переносимо:
//------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>

typedef struct
{
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
    uint8_t b4;
} IP_Address;

typedef struct
{
    uint8_t  version;       // 4 бита
    uint8_t  ihl;           // 4 бита (в 32-битных словах)
    uint8_t  serv_type;     // Type of Service / DSCP/ECN
    uint16_t total_length;  // в байтах
    uint16_t id;
    uint8_t  flags;         // 3 бита (упакованы в старших битах)
    uint16_t frag_offset;   // 13 бит (в блоках по 8 байт)
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    IP_Address src_ip;
    IP_Address dst_ip;
} IP_Header_Parsed;

// Читаем 16-битное число в big-endian (сетевой порядок) из двух байтов
static uint16_t read_u16_be(const uint8_t *p)
{
    return ((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

void parse_ip_header(const uint8_t *buf, IP_Header_Parsed *hdr)
{
    if (!buf || !hdr) return;

    // Первый байт: Version (старшие 4 бита) и IHL (младшие 4 бита)
    uint8_t ver_ihl = buf[0];
    hdr->version = ver_ihl >> 4;   // биты 7..4
    hdr->ihl     = ver_ihl & 0x0F; // биты 3..0

    hdr->serv_type   = buf[1];
    hdr->total_length = read_u16_be(buf + 2); // bytes 2–3
    hdr->id           = read_u16_be(buf + 4); // bytes 4–5

    // Байт 6–7: Flags (3 бита) + Fragment Offset (13 бит)
    uint16_t flags_offset = read_u16_be(buf + 6);

    // Старшие 3 бита — флаги, младшие 13 — смещение
    hdr->flags       = (flags_offset >> 13) & 0x07; // 3 бита
    hdr->frag_offset =  flags_offset        & 0x1FFF;

    hdr->ttl      = buf[8];
    hdr->protocol = buf[9];
    hdr->checksum = read_u16_be(buf + 10); // bytes 10–11

    hdr->src_ip.b1 = buf[12];
    hdr->src_ip.b2 = buf[13];
    hdr->src_ip.b3 = buf[14];
    hdr->src_ip.b4 = buf[15];

    hdr->dst_ip.b1 = buf[16];
    hdr->dst_ip.b2 = buf[17];
    hdr->dst_ip.b3 = buf[18];
    hdr->dst_ip.b4 = buf[19];
}

void print_ip_header_parsed(const IP_Header_Parsed *hdr)
{
    if (!hdr) return;

    printf("Version: %u\n", hdr->version);
    printf("IHL: %u (words), %u bytes\n", hdr->ihl, hdr->ihl * 4);
    printf("Service Type: %u\n", hdr->serv_type);
    printf("Total Length: %u\n", hdr->total_length);
    printf("ID: %u\n", hdr->id);

    // В стандартном IP:
    // bit 0 (старший из трёх): Reserved
    // bit 1: DF (Don't Fragment)
    // bit 2: MF (More Fragments)
    uint8_t reserved = (hdr->flags >> 2) & 0x01;
    uint8_t df       = (hdr->flags >> 1) & 0x01;
    uint8_t mf       = (hdr->flags >> 0) & 0x01;

    printf("Flags: Reserved=%u, DF=%u, MF=%u\n",
           reserved, df, mf);
    printf("Fragment Offset: %u\n", hdr->frag_offset);

    printf("TTL: %u\n", hdr->ttl);
    printf("Protocol: %u\n", hdr->protocol);
    printf("Header Checksum: 0x%04x\n", hdr->checksum);

    printf("Source IP: %u.%u.%u.%u\n",
           hdr->src_ip.b1,
           hdr->src_ip.b2,
           hdr->src_ip.b3,
           hdr->src_ip.b4);

    printf("Destination IP: %u.%u.%u.%u\n",
           hdr->dst_ip.b1,
           hdr->dst_ip.b2,
           hdr->dst_ip.b3,
           hdr->dst_ip.b4);
}

int main(void)
{
    // Пример "сырого" IPv4-заголовка (20 байт без опций)
    uint8_t buf[20] = {
        0x45,       // Version=4, IHL=5
        0x00,       // Service Type
        0x00, 0x28, // Total Length = 40
        0x12, 0x34, // ID
        0x40, 0x00, // Flags+Offset: 0x4000 -> DF=1, offset=0
        0x40,       // TTL = 64
        0x06,       // Protocol = 6 (TCP)
        0x1a, 0x2b, // Header checksum (пример)
        192, 168, 0, 10, // Src IP: 192.168.0.10
        8, 8, 8, 8       // Dst IP: 8.8.8.8
    };

    IP_Header_Parsed hdr;
    parse_ip_header(buf, &hdr);
    print_ip_header_parsed(&hdr);

    return 0;
}
//------------------------------------------------------------------------
//разобираем, как именно из 0x40 0x00 получается DF=1, MF=0, offset=0
//------------------------------------------------------------------------
/*
Берём два байта из заголовка:

0x40 0x00
Склеиваем их в одно 16‑битное число в сетевом порядке (big‑endian):

0x40 0x00 = 0x4000
В двоичном виде (16 бит):

0x4000 = 0100 0000 0000 0000₂
Поле «Flags + Fragment Offset» в IP‑заголовке описано так:

3 старших бита: флаги
bit 15: Reserved (должен быть 0)
bit 14: DF (Don't Fragment)
bit 13: MF (More Fragments)
13 младших бит: Fragment Offset
Разложим
0100 0000 0000 0000₂
по этим полям:

битов всего:  15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
значения:      0  1  0  0  0  0 0 0 0 0 0 0 0 0 0 0
               |  |  |
               |  |  └─ MF (bit 13) = 0
               |  └──── DF (bit 14) = 1
               └────── Reserved (bit 15) = 0
младшие 13 бит (12..0) = 000 0000 0000 000 = 0 → offset = 0
Теперь теми же операциями, что в коде.

В коде: как мы это получаем:

Мы читаем 16‑битное поле:

uint16_t flags_offset = read_u16_be(buf + 6); // вернёт 0x4000
Далее:

hdr->flags       = (flags_offset >> 13) & 0x07;
hdr->frag_offset =  flags_offset        & 0x1FFF;
Разбираем по шагам.

1) Выделить флаги:
(flags_offset >> 13) & 0x07
flags_offset = 0x4000 = 0100 0000 0000 0000₂

flags_offset >> 13:

0100 0000 0000 0000₂ >> 13 = 0000 0000 0000 0010₂ = 0x0002
                             ^^^^^^^^^^^^^^^^^^^
                                тут остались только биты 15..13

Теперь & 0x07 (0000 0000 0000 0111₂):

0x0002 & 0x0007 = 0x0002 = 0000 0010₂
То есть:

hdr->flags = 0b010;
Интерпретация:

бит 2 (старший): Reserved = 0
бит 1:           DF       = 1
бит 0 (младший): MF       = 0
В функции печати мы это и делали:

uint8_t reserved = (hdr->flags >> 2) & 0x01; // 0
uint8_t df       = (hdr->flags >> 1) & 0x01; // 1
uint8_t mf       = (hdr->flags >> 0) & 0x01; // 0
→
Reserved=0, DF=1, MF=0.

2) Выделить смещение фрагмента:
flags_offset & 0x1FFF
Маска:

0x1FFF = 0001 1111 1111 1111₂
Берём:

flags_offset = 0x4000 = 0100 0000 0000 0000₂
0x1FFF       =          0001 1111 1111 1111₂
-------------------------------------------
AND ( & )    =          0000 0000 0000 0000₂ = 0
Значит:

hdr->frag_offset = 0;
Итого из
0x40 0x00
:

flags_offset = 0x4000
flags = 0b010
→ Reserved=0, DF=1, MF=0
frag_offset = 0
Поэтому и говорим: DF=1, MF=0, offset=0.

*/