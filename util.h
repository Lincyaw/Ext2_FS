//
// Created by fay on 2020/12/13.
//

#ifndef EXT2_FS_UTIL_H
#define EXT2_FS_UTIL_H

#include <stdio.h>
#include <stdint.h>

int bit_isset(const uint32_t *array, uint32_t index);

void bit_set(uint32_t *array, uint32_t index);

void bit_clear(uint32_t *array, uint32_t index);

char *simple_tok(char *p, char d);

char *trim(char *c);

void rmEnter(char *c);

void printBit(uint32_t *array, uint32_t size);

#endif //EXT2_FS_UTIL_H
