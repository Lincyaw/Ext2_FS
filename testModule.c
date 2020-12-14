//
// Created by llincyaw on 2020/12/11.
//

#include "testModule.h"

void testInitSP() {
    disk_read(0, buffer);
    sp_block *head = (sp_block *)buffer;
    printSuperBlock(head);
}

void testSep(char* dir){

    char * ans;
    while((ans = strsep(&dir,"/"))!=NULL){
        printf("%s\n",ans);
        ans += strlen(ans);
    }
}