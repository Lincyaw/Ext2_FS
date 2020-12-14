//
// Created by llincyaw on 2020/12/11.
//

#include "testModule.h"

void testInitSP() {
    disk_read(0, buffer);
    sp_block *head = (sp_block *) buffer;
    printSuperBlock(head);
}

void testSep(char *dir) {

//    char * ans;
//    while((ans = strsep(&dir,"/"))!=NULL){
//        printf("%s\n",ans);
//        ans += strlen(ans);
//    }
}

void testCreateInode() {
    static int l = 1;
    // TODO: 追踪错误, 在这里测试. 一个block按道理应该能create 1024/32 = 32个inode, 但是创建了4个就出错了
    createInode(2, 100 + l, FILE_T, 1);
    l++;
}

void testCreateDirItem() {

}

void testBitSet() {
    uint32_t data[10] = {0};
    printBit(data, 10);
    for (int i = 0; i < 10; i++) {
        bit_set(data, i);
        printBit(data, 10);
    }

    bit_set(data, 16);
    printBit(data, 10);

    bit_set(data, 17);
    printBit(data, 10);
}