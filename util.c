//
// Created by fay on 2020/12/13.
//

#include "util.h"

int bit_isset(const uint32_t *array, uint32_t index) {
    uint8_t *head = (uint8_t *) array;
    // 整除8,得到在哪个char
    uint8_t b = head[index / 8];
    // 模8,得到在char的第几个bit
    uint8_t m = (uint8_t) (1 << (index % 8));
    return (b & m) == m;
}

void bit_set(uint32_t *array, uint32_t index) {
    uint8_t *head = (uint8_t *) array;
    // 整除8,得到在哪个char
    uint8_t b = head[index / 8];
    // 模8,得到在char的第几个bit
    uint8_t m = (uint8_t) (1 << (index % 8));
    //该位置或上去
    array[index / 8] = (b | m);
}

void bit_clear(uint32_t *array, uint32_t index) {
    uint8_t *head = (uint8_t *) array;
    uint8_t b = head[index / 8];
    uint8_t m = (uint8_t) (1 << (index % 8));
    array[index / 8] = (b & ~m);
}