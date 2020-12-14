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

// 把'/'换成终结符'\0',返回剩余的字符
char *simple_tok(char *p, char d)
{
    if(p==NULL){
        return NULL;
    }
    char *t = p;
    while (*t != '\0' && *t != d)
        t++;
    if (*t == '\0')
        return p;
    *t = '\0';
    return t + 1;
}

// [in-place]
// trim spaces on both side
char *trim(char *c)
{
    char *e = c;
    while (*e)
        e++;
    while (*c == ' ')
        *(c++) = '\0';
    while (*(--e) == ' ')
        ;
    *(e + 1) = '\0';
    return c;
}

void rmEnter(char *c){
    int i=0;
    while(c[i]!='\0' && c[i]!='\n'){
        i++;
    }
    if(c[i]=='\n'){
        c[i] = '\0';
    }
}