//--------------------------------------------------------
//          Упаковка заголовка IP в буфер для отправки
//--------------------------------------------------------
//-----------------------------------------------------------------
//Ниже полный, цельный вариант программы в одном файле с комментариями.
/*
Она:
описывает структуры
IP_Address и IP_Header_Parsed, умеет:
упаковать структуру заголовка IP в «сырой» 20‑байтовый буфер,
распаковать буфер обратно в структуру,
напечатать содержимое заголовка.
*/

#include <stdio.h>
#include <stdint.h>

// Структура для IPv4-адреса в виде 4 отдельных байтов: a.b.c.d
typedef struct
{
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
    uint8_t b4;
} IP_Address;

// "Разобранный" IP-заголовок в удобном для нас виде
typedef struct
{
    uint8_t  version;       // 4 бита — версия IP (обычно 4)
    uint8_t  ihl;           // 4 бита — длина заголовка в 32-битных словах
    uint8_t  serv_type;     // Type of Service / DSCP/ECN
    uint16_t total_length;  // полная длина пакета (заголовок + данные), в байтах
    uint16_t id;            // идентификатор фрагментации
    uint8_t  flags;         // 3 бита: [Reserved, DF, MF] храним в младших битах 2..0
    uint16_t frag_offset;   // смещение фрагмента (13 бит, в блоках по 8 байт)
    uint8_t  ttl;           // Time To Live
    uint8_t  protocol;      // протокол верхнего уровня (TCP=6, UDP=17 и т.д.)
    uint16_t checksum;      // контрольная сумма заголовка IP
    IP_Address src_ip;      // исходный IP
    IP_Address dst_ip;      // целевой IP
} IP_Header_Parsed;

//----------------------------------------------------------
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ 16-БИТНЫХ ЧИСЕЛ В BIG-ENDIAN
//----------------------------------------------------------

// Записать 16-битное число в big-endian (сетевой порядок) в 2 байта
static void write_u16_be(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)(v & 0xFF);
}

// Прочитать 16-битное число из 2 байт в big-endian
static uint16_t read_u16_be(const uint8_t *p)
{
    return ((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

//----------------------------------------------------------
// УПАКОВКА: ЗАПОЛНЕННАЯ СТРУКТУРА -> "СЫРОЙ" БУФЕР (20 БАЙТ)
//----------------------------------------------------------

void build_ip_header(const IP_Header_Parsed *hdr, uint8_t *buf)
{
    if (!hdr || !buf) return;

    // Byte 0: Version (старшие 4 бита) + IHL (младшие 4 бита)
    buf[0] = (uint8_t)((hdr->version << 4) | (hdr->ihl & 0x0F));

    // Byte 1: Service Type
    buf[1] = hdr->serv_type;

    // Bytes 2–3: Total Length (big-endian)
    write_u16_be(buf + 2, hdr->total_length);

    // Bytes 4–5: ID
    write_u16_be(buf + 4, hdr->id);

    // Bytes 6–7: Flags (3 бита) + Fragment Offset (13 бит)
    //
    // В 16-битном поле IP:
    // bits 15..13 = флаги (Reserved, DF, MF)
    // bits 12..0  = offset
    //
    // В hdr->flags храним флаги в младших 3 битах: [2:Reserved, 1:DF, 0:MF]
    uint16_t flags_offset =
        ((uint16_t)(hdr->flags & 0x07) << 13) |  // флаги в старшие 3 бита
        (hdr->frag_offset & 0x1FFF);             // младшие 13 бит — offset

    write_u16_be(buf + 6, flags_offset);

    // Byte 8: TTL
    buf[8] = hdr->ttl;

    // Byte 9: Protocol
    buf[9] = hdr->protocol;

    // Bytes 10–11: Header Checksum
    write_u16_be(buf + 10, hdr->checksum);

    // Bytes 12–15: Source IP
    buf[12] = hdr->src_ip.b1;
    buf[13] = hdr->src_ip.b2;
    buf[14] = hdr->src_ip.b3;
    buf[15] = hdr->src_ip.b4;

    // Bytes 16–19: Destination IP
    buf[16] = hdr->dst_ip.b1;
    buf[17] = hdr->dst_ip.b2;
    buf[18] = hdr->dst_ip.b3;
    buf[19] = hdr->dst_ip.b4;
}

//----------------------------------------------------------
// РАСПАКОВКА: "СЫРОЙ" БУФЕР (20 БАЙТ) -> РАЗОБРАННАЯ СТРУКТУРА
//----------------------------------------------------------

void parse_ip_header(const uint8_t *buf, IP_Header_Parsed *hdr)
{
    if (!buf || !hdr) return;

    // Byte 0: Version + IHL
    uint8_t ver_ihl = buf[0];
    hdr->version = ver_ihl >> 4;     // биты 7..4
    hdr->ihl     = ver_ihl & 0x0F;   // биты 3..0

    // Byte 1: Service Type
    hdr->serv_type = buf[1];

    // Bytes 2–3: Total Length
    hdr->total_length = read_u16_be(buf + 2);

    // Bytes 4–5: ID
    hdr->id = read_u16_be(buf + 4);

    // Bytes 6–7: Flags + Fragment Offset
    uint16_t flags_offset = read_u16_be(buf + 6);

    // Старшие 3 бита — флаги
    hdr->flags       = (flags_offset >> 13) & 0x07; // 3 бита
    // Младшие 13 бит — offset
    hdr->frag_offset =  flags_offset        & 0x1FFF;

    // Byte 8: TTL
    hdr->ttl = buf[8];

    // Byte 9: Protocol
    hdr->protocol = buf[9];

    // Bytes 10–11: Header Checksum
    hdr->checksum = read_u16_be(buf + 10);

    // Bytes 12–15: Source IP
    hdr->src_ip.b1 = buf[12];
    hdr->src_ip.b2 = buf[13];
    hdr->src_ip.b3 = buf[14];
    hdr->src_ip.b4 = buf[15];

    // Bytes 16–19: Destination IP
    hdr->dst_ip.b1 = buf[16];
    hdr->dst_ip.b2 = buf[17];
    hdr->dst_ip.b3 = buf[18];
    hdr->dst_ip.b4 = buf[19];
}

//----------------------------------------------------------
// ПЕЧАТЬ РАЗОБРАННОГО IP-ЗАГОЛОВКА В ЧИТАЕМОМ ВИДЕ
//----------------------------------------------------------

void print_ip_header_parsed(const IP_Header_Parsed *hdr)
{
    if (!hdr) return;

    printf("Version: %u\n", hdr->version);
    printf("IHL: %u (words), %u bytes\n",
           hdr->ihl, hdr->ihl * 4);
    printf("Service Type: %u\n", hdr->serv_type);
    printf("Total Length: %u\n", hdr->total_length);
    printf("ID: %u\n", hdr->id);

    // В стандартном IP:
    // bit 2 (старший из трёх) — Reserved
    // bit 1 — DF (Don't Fragment)
    // bit 0 — MF (More Fragments)
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

//----------------------------------------------------------
// ПРИМЕР ИСПОЛЬЗОВАНИЯ: СОБРАТЬ ЗАГОЛОВОК, УПАКОВАТЬ, РАСПАКОВАТЬ, ВЫВЕСТИ
//----------------------------------------------------------

int main(void)
{
    // Заполняем структуру заголовка некоторыми значениями
    IP_Header_Parsed hdr = {0};

    hdr.version      = 4;       // IPv4
    hdr.ihl          = 5;       // 5 * 4 = 20 байт заголовка
    hdr.serv_type    = 0;
    hdr.total_length = 40;      // 20 байт заголовка + 20 байт данных (пример)
    hdr.id           = 0x1234;

    // flags: Reserved=0, DF=1, MF=0 -> биты [2..0] = 0b010
    uint8_t reserved = 0;
    uint8_t df       = 1;
    uint8_t mf       = 0;
    hdr.flags = (uint8_t)((reserved << 2) | (df << 1) | (mf << 0));

    hdr.frag_offset  = 0;       // не фрагментирован
    hdr.ttl          = 64;
    hdr.protocol     = 6;       // TCP

    // Здесь контрольная сумма просто пример (не вычисленная реально)
    hdr.checksum     = 0x1a2b;

    // Исходный IP: 192.168.0.10
    hdr.src_ip.b1 = 192;
    hdr.src_ip.b2 = 168;
    hdr.src_ip.b3 = 0;
    hdr.src_ip.b4 = 10;

    // Целевой IP: 8.8.8.8
    hdr.dst_ip.b1 = 8;
    hdr.dst_ip.b2 = 8;
    hdr.dst_ip.b3 = 8;
    hdr.dst_ip.b4 = 8;

    // Буфер для "сырых" 20 байт IP-заголовка
    uint8_t buf[20];

    // 1. Упаковываем структуру в буфер
    build_ip_header(&hdr, buf);

    // (по желанию можно здесь вывести buf в hex для наглядности)
    printf("Raw header bytes:\n");
    for (int i = 0; i < 20; ++i) {
        printf("%02x ", buf[i]);
    }
    printf("\n\n");

    // 2. Для проверки распарсим обратно из buf в другую структуру
    IP_Header_Parsed parsed;
    parse_ip_header(buf, &parsed);

    // 3. Печатаем разобранный заголовок
    print_ip_header_parsed(&parsed);

    return 0;
}