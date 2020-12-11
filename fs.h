//
// Created by llincyaw on 2020/12/11.
//

#ifndef EXT2_FS_FS_H
#define EXT2_FS_FS_H
#include "disk.h"
#include <stdio.h>
#include <stdint-gcc.h>
#include <stdlib.h>
#include <string.h>

char buf[DEVICE_BLOCK_SIZE]; //一次读写的
static char cmd[1024] = "\0";
#define BLOCK_SIZE (1024)
#define SP_SIZE sizeof(sp_block)
#define INODE_SIZE sizeof(iNode)
#define DIR_SIZE sizeof(dirItem)
#define MAGIC (0xaabbccdd)
#pragma pack(1)
// size = (32*4+32*128+32*32)/4 = 656byte
typedef struct super_block {
    int32_t magic_num;                  // 幻数
    int32_t free_block_count;           // 空闲数据块数
    int32_t free_inode_count;           // 空闲inode数
    int32_t dir_inode_count;            // 目录inode数
    uint32_t block_map[128];            // 数据块占用位图
    uint32_t inode_map[32];             // inode占用位图
    int8_t un_used[1024-656];          // 填充
} sp_block;
#pragma pack()

#pragma pack(1)
// size = 4+2+2+4*6 = 32 byte
typedef struct inode {
    uint32_t size;              // 文件大小
    uint16_t file_type;         // 文件类型（文件/文件夹）
    uint16_t link;              // 连接数
    uint32_t block_point[6];    // 数据块指针
} iNode;
#pragma pack()

// size = 4+2+1+72 = 128 byte
#pragma pack(1)
typedef struct dir_item {               // 目录项一个更常见的叫法是 dirent(directory entry)
    uint32_t inode_id;          // 当前目录项表示的文件/目录的对应inode
    uint16_t valid;             // 当前目录项是否有效
    uint8_t type;               // 当前目录项类型（文件/目录）
    char name[121];             // 目录项表示的文件/目录的文件名/目录名
} dirItem;
#pragma pack()


void initExt2();
void printSuperBlock(const sp_block* sp_block_buf);
void printInode(const iNode * node);
#endif //EXT2_FS_FS_H
