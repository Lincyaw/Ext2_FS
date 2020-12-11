//
// Created by llincyaw on 2020/12/11.
//

#include "testModule.h"

void testInitSP() {
    open_disk();
    disk_read_block(0, buf);
    sp_block *head = (sp_block *)buf;
    printSuperBlock(head);
    head+=SP_SIZE;
    close_disk();
}