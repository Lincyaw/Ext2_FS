//
// Created by lincyaw on 2020/12/11.
//

#ifndef EXT2_FS_FS_H
#define EXT2_FS_FS_H
#include "disk.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
char buf[DEVICE_BLOCK_SIZE]; //一次读写的
static char cmd[1024] = "\0";
#define BLOCK_SIZE (1024)
#define SP_SIZE sizeof(sp_block)
#define INODE_SIZE sizeof(iNode)
#define DIR_SIZE sizeof(dirItem)
#define MAGIC (0xaabbccdd)
#define DEBUG (0)
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
/**
 * 打印超级块中的内容
 * @param sp_block_buf
 */
void printSuperBlock(const sp_block* sp_block_buf);
/**
 * 打印Inode中的内容
 * @param node
 */
void printInode(const iNode * node);

/**
 * 新建一个inode,可以是文件或者是文件夹。
 * 创建成功返回node的序号,否则返回-1.
 * @param size 文件/文件夹的大小
 * @param file_type 选择是文件还是文件夹
 * @param link 文件链接数,默认为1
 */
int createInode(uint32_t size, uint16_t file_type, uint16_t link);

/**
 * 新建一个directory entry
 * 创建成功返回1,否则返回-1
 * @param blockNum 在第几块block创建
 * @param inode_id inode的id,这里选择的是inode的序号
 * @param type 是文件还是文件夹
 * @param name 目录项表示的文件/目录的文件名/目录名
 */
int createDirItem(uint32_t blockNum, uint32_t inode_id, uint8_t type, char *name);


/**
 * 遍历给定目录(输入一个id)对应的block中遍历，直到找到名字是 name 的 dir_item , 返回 inodeid
 * @param curDirInode 当前目录所在的id号
 * @param name 要找的文件/文件夹名
 * @return
 */
uint32_t findFolderOrFile(uint32_t curDirInode, char* name);


/**
 * 新建一个文件
 * @param fileName
 */
int touch(char *fileName);

/**
 * 新建一个文件夹
 * @param folderName
 */
int mkdir(char *folderName);

int ls(char *dir);

int cp();

void shutdown();

#endif //EXT2_FS_FS_H
