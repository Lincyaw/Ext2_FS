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


#endif //EXT2_FS_UTIL_H
