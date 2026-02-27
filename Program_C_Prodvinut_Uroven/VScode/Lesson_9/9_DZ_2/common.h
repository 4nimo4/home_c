// common.h
#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

#define SOP 0x7E
#define EOP 0x7E
#define ESC 0x7D

// Кодирование (байт-стаффинг)
static inline size_t stuff_bytes(const uint8_t *data, size_t len, uint8_t *out)
{
    size_t pos = 0;
    out[pos++] = SOP;  // начало кадра

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = data[i];
        if (b == SOP || b == ESC) {
            out[pos++] = ESC;
            out[pos++] = b ^ 0x20;  // 0x7E->0x5E, 0x7D->0x5D
        } else {
            out[pos++] = b;
        }
    }

    out[pos++] = EOP;  // конец кадра
    return pos;
}

// Декодирование (обратный байт-стаффинг)
static inline size_t destuff_bytes(const uint8_t *frame, size_t len, uint8_t *out)
{
    if (len < 2 || frame[0] != SOP || frame[len - 1] != EOP) {
        return 0; // неверный кадр
    }

    size_t pos = 0;

    // от 1 до len-2 (между SOP и EOP)
    for (size_t i = 1; i < len - 1; ++i) {
        uint8_t b = frame[i];
        if (b == ESC) {
            if (i + 1 >= len - 1) {
                return 0; // обрезанный ESC
            }
            uint8_t next = frame[++i];
            out[pos++] = next ^ 0x20;
        } else {
            out[pos++] = b;
        }
    }

    return pos;
}

#endif // COMMON_H