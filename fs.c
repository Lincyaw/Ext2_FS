//
// Created by llincyaw on 2020/12/11.
//

#include "fs.h"

void initExt2() {
    open_disk();
    fprintf(stderr, "Starting to check disk size.\n");
    fprintf(stderr, "sp_block size = %lu.\n", sizeof(sp_block));
    fprintf(stderr, "iNode size = %lu.\n", sizeof(iNode));
    fprintf(stderr, "dirItem size = %lu.\n", sizeof(dirItem));

    sp_block *superB = (sp_block *) malloc(SP_SIZE);
    memset(superB, 0, SP_SIZE);
    superB->magic_num = MAGIC;
    superB->free_block_count = BLOCK_SIZE - 1 - 32;  //1024-超级块-inode数组
    superB->free_inode_count = 32 * BLOCK_SIZE / INODE_SIZE; // 32*1024/32
    superB->dir_inode_count = 0;

    disk_write_block(0,(char*)superB);
    close_disk();
}
void printSuperBlock(const sp_block* sp_block_buf){
    int32_t *head = (int32_t *)sp_block_buf;

    printf("====================SUPER-BLOCK================\n");
    for (int i = 0; i < 4; i++) {
        printf("%.8x ", head[i]);
    }
    printf("\n");
    head+=4;
    uint32_t *mapHead = (uint32_t*) head;
    printf("====================BLOCK-MAP================\n");
    for (int i = 0; i < 128; i++) {
        printf("%.8x ", *mapHead);
        mapHead++;
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
    printf("====================INODE-MAP================\n");
    for (int i = 0; i < 32; i++) {
        printf("%.8x ", *mapHead);
        mapHead++;
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
    printf("\n");

}
void printInode(const iNode *node){
    uint32_t *head = (uint32_t*) node;
    printf("====================SIZE================\n");
    printf("%.8x",*head);
    head++;
    uint16_t *head1 = (uint16_t*)head;
    printf("====================file_type================\n");
    printf("%.4x",*head1);
    head1++;
    printf("====================link================\n");
    printf("%.4x",*head1);
    head1++;
    head = (uint32_t*)head1;
    printf("====================block_point================\n");

    for(int i = 0;i<6;i++){
        printf("%.8x",*head);
        head++;
    }
}