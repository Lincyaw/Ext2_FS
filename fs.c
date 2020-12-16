//
// Created by lincyaw on 2020/12/11.
//
#include "fs.h"

void initExt2() {
    int errno = remove("../disks/disk");
    if (errno < 0) {
        fprintf(stderr, "delete failed.\n");
        printf("%s\n", strerror(errno));
    }
#if DEBUG == 1
    fprintf(stderr, "Starting to check disk size.\n");
    fprintf(stderr, "sp_block size = %lu.\n", sizeof(sp_block));
    fprintf(stderr, "iNode size = %lu.\n", sizeof(iNode));
    fprintf(stderr, "dirItem size = %lu.\n", sizeof(dirItem));
    assert(sizeof(sp_block) == 1024);
    assert(sizeof(iNode) == 32);
    assert(sizeof(dirItem) == 128);
#endif
    // 初始化超级块
    char buffer[BLOCK_SIZE];
    sp_block *superB = (sp_block *) buffer;
    memset(superB, 0, SP_SIZE);
    superB->magic_num = MAGIC;
    superB->free_block_count = MAX_BLOCK_NUM - 1 - 4;  //1024-超级块-4个block的inode数组
    superB->free_inode_count = 32 * BLOCK_SIZE / INODE_SIZE - 2; // 32*1024/32
    superB->dir_inode_count = 0;
    // 前5块是超级块+inode数组部分
    bit_set(superB->block_map, 0);
    bit_set(superB->block_map, 1);
    bit_set(superB->block_map, 2);
    bit_set(superB->block_map, 3);
    bit_set(superB->block_map, 4);
    // 前三个inode默认被占用
    bit_set(superB->inode_map, 0);
    bit_set(superB->inode_map, 1);
//    bit_set(superB->inode_map, 2);


#if DEBUG == 1
    assert(superB->magic_num == MAGIC);
    assert(superB->free_block_count == MAX_BLOCK_NUM - 1 - 4);
    assert(superB->free_inode_count == 1022);
#endif
    disk_write_whole_block(0, buffer);
    // createinode一定要放到write后面，因为放前面的话会导致create了个寂寞
    createInode(1, 0x100, FOLDER_T, 1);
}

void printSuperBlock(const sp_block *sp_block_buf) {
    if (sp_block_buf == NULL) {
        fprintf(stderr, "Error:\tNull pointer.\n");
        return;
    }
    int32_t *head = (int32_t *) sp_block_buf;

    printf("====================SUPER-BLOCK================\n");
    for (int i = 0; i < 4; i++) {
        printf("%x ", head[i]);
    }
    printf("\n");
    head += 4;
    uint32_t *mapHead = (uint32_t *) head;
    printf("====================BLOCK-MAP================\n");
    for (int i = 0; i < 128; i++) {
        printf("%x ", *mapHead);
        mapHead++;
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
    printf("====================INODE-MAP================\n");
    for (int i = 0; i < 32; i++) {
        printf("%x ", *mapHead);
        mapHead++;
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
    printf("\n");

}

void printInode(iNode node, FILE *fp) {
//    if (node == NULL) {
//        fprintf(stderr, "Error:\tNull pointer.\n");
//        return;
//    }
    uint32_t *head = (uint32_t *) &node;
    fprintf(fp, "====================SIZE================\n");
    fprintf(fp, "%x\n", *head);
    head++;
    uint16_t *head1 = (uint16_t *) head;
    fprintf(fp, "====================file_type================\n");
    fprintf(fp, "%x\n", *head1);
    head1++;
    fprintf(fp, "====================link================\n");
    fprintf(fp, "%x\n", *head1);
    head1++;
    head = (uint32_t *) head1;
    fprintf(fp, "====================block_point================\n");

    for (int i = 0; i < 6; i++) {
        fprintf(fp, "%x\n", *head);
        head++;
    }
}


void printDirItem(dirItem node, FILE *fp) {
    uint32_t *head = (uint32_t *) &node;
    fprintf(fp, "====================inode_id================\n");
    fprintf(fp, "%x\n", *head);
    head++;
    uint16_t *head1 = (uint16_t *) head;
    fprintf(fp, "====================valid================\n");
    fprintf(fp, "%x\n", *head1);
    head1++;
    uint8_t *head2 = (uint8_t *) head1;
    fprintf(fp, "====================type================\n");
    fprintf(fp, "%x\n", *head2);
    head2++;
    fprintf(fp, "====================name================\n");
    fprintf(fp, "%s\n", head2);

}

/**
 * 新建一个inode,可以是文件或者是文件夹。
 * 只在这个buf中创建文件。
 * @param blockNum 要创建inode的那个block的number
 * @param size 文件/文件夹的大小
 * @param file_type 选择是文件还是文件夹
 * @param link 文件链接数,默认为1
 * @return 创建成功返回node的序号,否则返回-1.
 */
int createInode(uint32_t blockNum, uint32_t size, uint16_t file_type, uint16_t link) {
    // 问题: 第5次调用时出现Errors in disk. Errors in disk_read -- disk_write_block 等字样
    // 原因： windows的问题
    // 读要创建inode的这个block
    char buffer[BLOCK_SIZE];
    disk_read_whole_block(blockNum, buffer);
    iNode *p = (iNode *) buffer;

#if DEBUG == 1
    const char *filename1 = "../logs_before.txt";
    FILE *fp1 = fopen(filename1, "w");
    if (fp1 == NULL) {
        puts("Fail to open file!");
        exit(1);
    }
    fprintf(fp1, "~~~~~~~~~~~~block num: %d~~~~~~~~~~~\n", blockNum);
    for (int j = 0; j < BLOCK_SIZE / INODE_SIZE; j++) {
        fprintf(fp1, "=====================%d=======================\n", j);
        printInode(p[j], fp1);
    }
    fclose(fp1);
#endif

    char head[BLOCK_SIZE]; //读超级块
    disk_read_whole_block(0, head);
    sp_block *sp = (sp_block *) head;
    uint32_t *block_map = sp->block_map;
    // 第2块是根目录,所以从第3开始分配
    for (int i = blockNum == 1 ? 2 : 0; i < BLOCK_SIZE / INODE_SIZE; i++) {
        // 判断该位置是不是被使用过
        if (!bit_isset(sp->inode_map, getTotalInodeNum(blockNum, i))) {
            p[i].size = size;
            p[i].file_type = file_type;
            p[i].link = link;
            // 为这个inode分配6个block, 通过bitmap找
            int count = 0;
            // 前5块一定已被占用
            for (int j = 5; j < MAX_BLOCK_NUM; j++) {
                // 没被占用
                if (!bit_isset(block_map, j)) {
                    // 指向j
                    p[i].block_point[count] = j;
                    count++;
                    // 修改为已占用
                    bit_set(block_map, j);
                    if (count == 6) {
                        sp->free_inode_count--;
                        sp->free_block_count -= 6;
                        if (file_type == FOLDER_T) {
                            sp->dir_inode_count++;
                        }
                        uint32_t *inodeMap = (uint32_t *) sp->inode_map;
                        bit_set(inodeMap, getTotalInodeNum(blockNum, i));
                        break;
                    }
                }
            }
            if (count < 6) {
                fprintf(stderr, "Not enough blocks.\n");
                return -1;
            }
            disk_write_whole_block(blockNum, buffer);
            disk_write_whole_block(0, head);
#if DEBUG == 1
            const char *filename2 = "../logs_after.txt";
            FILE *fp2 = fopen(filename2, "w");
            if (fp2 == NULL) {
                puts("Fail to open file!");
                exit(1);
            }
            fprintf(fp2, "~~~~~~~~~~~~block num: %d~~~~~~~~~~~\n", blockNum);
            for (int j = 0; j < BLOCK_SIZE / INODE_SIZE; j++) {
                fprintf(fp2, "=====================%d=======================\n", j);
                printInode(p[j], fp2);
            }
            fclose(fp2);
#endif
            return i;

        }
    }
    return -1;
}

/**
 * 新建一个directory entry, 这个函数不需要提前读buf
 * @param blockNum 在第几块block创建
 * @param inode_id inode的id,这里选择的是inode的序号
 * @param type 是文件还是文件夹
 * @param name 目录项表示的文件/目录的文件名/目录名
 * @return 创建成功返回1,否则返回-1
 */
int createDirItem(uint32_t blockNum, uint32_t inode_id, uint8_t type, char name[121]) {
    char tempBuf[BLOCK_SIZE];
    char superB[BLOCK_SIZE];
//    disk_read_whole_block(0, tempBuf);
    disk_read_whole_block(blockNum, tempBuf);
    dirItem *di = (dirItem *) tempBuf;
    disk_read_whole_block(0, superB);
    sp_block *sp = (sp_block *) superB;

#if DEBUG == 1
    const char *filename1 = "../dir_logs_before.txt";
    FILE *fp1 = fopen(filename1, "w");
    if (fp1 == NULL) {
        puts("Fail to open file!");
        exit(1);
    }
    fprintf(fp1, "~~~~~~~~~~~~block num: %d~~~~~~~~~~~\n", blockNum);
    for (int j = 0; j < BLOCK_SIZE / DIR_SIZE; j++) {
        fprintf(fp1, "=====================%d=======================\n", j);
        printDirItem(di[j], fp1);
    }
    fclose(fp1);
#endif
    int flag = 0;
    for (int i = 0; i < BLOCK_SIZE / DIR_SIZE; i++) {
        if (!di[i].valid) {
            di[i].valid = 1;
            di[i].type = type;
            strcpy(di[i].name, name);
            di[i].inode_id = inode_id;
            flag = 1;
            if (!bit_isset(sp->block_map, blockNum)) {
                bit_set(sp->block_map, blockNum);
            }
            break;
        }
    }
    if (!flag) {
        fprintf(stderr, "\nWarning: The %d block is full of dirItem.\n", blockNum);
        return -1;
    }
#if DEBUG == 1
    const char *filename2 = "../dir_logs_after.txt";
    FILE *fp2 = fopen(filename2, "w");
    if (fp2 == NULL) {
        puts("Fail to open file!");
        exit(1);
    }
    fprintf(fp2, "~~~~~~~~~~~~block num: %d~~~~~~~~~~~\n", blockNum);

    for (int j = 0; j < BLOCK_SIZE / DIR_SIZE; j++) {
        fprintf(fp2, "=====================%d=======================\n", j);
        printDirItem(di[j], fp2);
    }
    fclose(fp2);
#endif
//    char tempBuf[BLOCK_SIZE];
//    disk_read_whole_block(0, tempBuf);
    disk_write_whole_block(0, superB);
    disk_write_whole_block(blockNum, tempBuf);
    return 1;
}

/**
 * 遍历给定目录(输入一个id)对应的block中遍历，直到找到名字是 name 的 dir_item.
 * @param curDirInode 当前目录所在的id号
 * @param name 要找的文件/文件夹名, 确保长度是121的字符数组
 * @return 返回找到的那个diritem对应的inodeid, 失败则返回-1
 */
uint32_t findFolderOrFile(uint32_t curDirInode, char name[121], int type) {
    uint32_t which = getBlockNum(curDirInode);
    uint32_t id = getInodeNum(curDirInode);
    char buffer[BLOCK_SIZE];
    // inode数组从第1个block开始.第0个是超级块
    disk_read_whole_block(which, buffer);
    iNode *cdi = (iNode *) buffer;
    // 找到这个inode中对应的block_point数组
    uint32_t *block_point = cdi[id].block_point;
    // 遍历这个数组对应的block, 找有没有这个名字
    for (int i = 0; i < 6; i++) {
        int blkNum = block_point[i];
        char tempBuf[BLOCK_SIZE];
        disk_read_whole_block(blkNum, tempBuf);
        dirItem *di = (dirItem *) tempBuf;
        for (int j = 0; j < BLOCK_SIZE / DIR_SIZE; j++) {
            if (strcmp(di[j].name, name) == 0) {
                if (di[j].type == type)
                    return di[j].inode_id;
                else {
                    fprintf(stderr, "Error: %s is not a folder.\n", name);
                    return -1;
                }
            }
        }
    }
    fprintf(stderr, "Error: no such file/folder.\n");
    return -1;
}

/**
 * 在当前目录寻找文件/文件名
 * @param curDirInode 当前目录所在的inode的id
 * @param fileName
 * @return 创建成功返回node的序号,否则返回-1
 */


/**
 * @param dir 形如 "config/zsh/zsh_config.txt" 这样的字符数组
 * @return 执行成功返回inodeid, 否则返回-1
 */
int touch(char *dir) {
    // eg:　把输入的形如"config/zsh/zsh_config.txt"的字符串, 解析成 "config" "zsh" "zsh_config.txt", 存入fileName
    char *left, *right;
//    char *fileName;
    // strsep　函数分割出来的结果：此时fileName是 config
    left = dir;
    right = simple_tok(left, '/');
//    fileName = strsep(&dir, "/");

    uint32_t targetId = 2;
    if (strcmp(left, right) != 0) {
        while ((targetId = findFolderOrFile(targetId, left, FOLDER_T)) != -1) {
            // 往前挪，把指针挪到zsh开头的那里
            left = right;
            right = simple_tok(left, '/');
            // 如果left等于right，则说明此时的left已经是文件名了，详情看simple_tok
            if (left == right) {
                break;
            }
        }
    } else {
        // 根目录
        targetId = 2;
    }

//    if (targetId == -1) {
//        return -1;
//    }
    // 此时的targetId是最后一个文件夹所在的id, 还需要找到这个文件夹对应的blocks,在这个blocks中加入一个diritem,
    // 并且在inode数组中找一个空闲的inode, create一下
    // 下面开始创建一个新的inode
    uint32_t inode_id;
    for (int i = 1; i < 5; i++) {
        inode_id = createInode(i, 100, FILE_T, 1);
        if (inode_id != -1) { // 创建成功就退出
            break;
        }
    }

    if (inode_id == -1) {
        return -1;
    }

    // 下面开始创建一个新的dir
    uint32_t which = getBlockNum(targetId);
    uint32_t id = getInodeNum(targetId);
    // inode数组从第1个block开始.第0个是超级块
    char tempBuf[BLOCK_SIZE];
    disk_read_whole_block(which, tempBuf);
    iNode *cdi = (iNode *) tempBuf;
    // 找到这个inode中对应的block_point数组
    uint32_t *block_point = cdi[id].block_point;
#if DEBUG == 1
    char tB[BLOCK_SIZE];
    disk_read_whole_block(0, tB);
    printSuperBlock((sp_block *) tB);
#endif
    int flag = 0;
    for (int i = 0; i < 6; i++) {
        int blkNum = block_point[i];
        // 此时的left是要产生的文件名
        if (createDirItem(blkNum, inode_id, FILE_T, left) == 1) {
            flag = 1;
            break;
        }
    }

#if DEBUG == 1
    char tB2[BLOCK_SIZE];
    disk_read_whole_block(0, tB2);
    printSuperBlock((sp_block *) tB2);
#endif
    return flag ? inode_id : flag;
}


int mkdir(char *folderName) {
    char *left, *right;
    left = folderName;
    right = simple_tok(left, '/');
    uint32_t targetId = 2;
    if (strcmp(left, right) != 0) {
        while ((targetId = findFolderOrFile(targetId, left, FOLDER_T)) != -1) {
            // 往前挪，把指针挪到zsh开头的那里
            left = right;
            right = simple_tok(left, '/');
            // 如果left等于right，则说明此时的left已经是文件名了，详情看simple_tok
            if (left == right) {
                break;
            }
        }
    } else {
        // 根目录
        targetId = 2;
    }

    // 此时的targetId是最后一个文件夹所在的id, 还需要找到这个文件夹对应的blocks,在这个blocks中加入一个diritem,
    // 并且在inode数组中找一个空闲的inode, create一下
    // 下面开始创建一个新的inode
    uint32_t inode_id;
    for (int i = 1; i < 5; i++) {
        inode_id = createInode(i, 200, FOLDER_T, 1);
        if (inode_id != -1) { // 创建成功就退出
            break;
        }
    }

    if (inode_id == -1) {
        return -1;
    }

    // 下面开始创建一个新的dir
    uint32_t which = getBlockNum(targetId);
    uint32_t id = getInodeNum(targetId);
    // inode数组从第1个block开始.第0个是超级块
    char tempBuf[BLOCK_SIZE];
    disk_read_whole_block(which, tempBuf);
    iNode *cdi = (iNode *) tempBuf;
    // 找到这个inode中对应的block_point数组
    uint32_t *block_point = cdi[id].block_point;
#if DEBUG == 1
    char tB[BLOCK_SIZE];
    disk_read_whole_block(0, tB);
    printSuperBlock((sp_block *) tB);
#endif
    int flag = 0;
    for (int i = 0; i < 6; i++) {
        int blkNum = block_point[i];
        // 此时的left是要产生的文件名
        if (createDirItem(blkNum, inode_id, FOLDER_T, left) == 1) {
            flag = 1;
            break;
        }
    }

#if DEBUG == 1
    char tB2[BLOCK_SIZE];
    disk_read_whole_block(0, tB2);
    printSuperBlock((sp_block *) tB2);
#endif
    return flag;
}

int ls(char *dir) {
    char *left, *right;
    left = dir;
    right = simple_tok(left, '/');
    uint32_t targetId = 2;
    if (strcmp(left, "ls") != 0) {
        while ((targetId = findFolderOrFile(targetId, left, FOLDER_T)) != -1) {
            // 往前挪，把指针挪到zsh开头的那里
            left = right;
            right = simple_tok(left, '/');
            // 如果left等于right，则说明此时的left已经是文件名了，详情看simple_tok
            if (left == right) {
                break;
            }
        }
    } else {
        // 根目录
        targetId = 2;
    }
// TODO:发现目标id少了1
    uint32_t which = getBlockNum(targetId);
    uint32_t id = getInodeNum(targetId);
    // inode数组从第1个block开始.第0个是超级块
    char tempBuf[BLOCK_SIZE];
    disk_read_whole_block(which, tempBuf);
    iNode *cdi = (iNode *) tempBuf;
    // 找到这个inode中对应的block_point数组
    uint32_t *block_point = cdi[id].block_point;
    printf("%-25s%-15s%-15s\n", "name", "type", "inode_id");
    for (int i = 0; i < 6; i++) {
        int blkNum = block_point[i];
        if (blkNum == 0) {
            continue;
        }

        char files[BLOCK_SIZE];
        disk_read_whole_block(blkNum, files);
        dirItem *p = (dirItem *) files;
        for (int j = 0; j < BLOCK_SIZE / DIR_SIZE; j++) {
            if (p[j].valid) {
                printf("%-25s%-15s%-15d\n", p[j].name, p[j].type == 100 ? "File" : "Folder", p[j].inode_id);
            }
        }
    }


    return 1;
}

int cp(char *source, char *target) {
    char des[121];
    strcpy(des,target);
    char *left, *right;
    left = source;
    right = simple_tok(left, '/');
    uint32_t targetId = 2;
    if (strcmp(left, right) != 0) {
        while ((targetId = findFolderOrFile(targetId, left, FOLDER_T)) != -1) {
            // 往前挪，把指针挪到zsh开头的那里
            left = right;
            right = simple_tok(left, '/');
            // 如果left等于right，则说明此时的left已经是文件名了，详情看simple_tok
            if (left == right) {
                break;
            }
        }
    } else {
        // 根目录
        targetId = 2;
    }
    char *left1, *right1;
    left1 = target;
    right1 = simple_tok(left1, '/');
    uint32_t targetId1 = 2;
    if (strcmp(left1, right1) != 0) {
        while ((targetId1 = findFolderOrFile(targetId1, left1, FOLDER_T)) != -1) {
            // 往前挪，把指针挪到zsh开头的那里
            left1 = right1;
            right1 = simple_tok(left1, '/');
            // 如果left等于right，则说明此时的left已经是文件名了，详情看simple_tok
            if (left1 == right1) {
                break;
            }
        }
    } else {
        // 根目录
        targetId1 = 2;
    }
    /**
     * targetId是源文件所在的文件夹的inodeId, left是源文件的文件名
     * targetId1是目标文件需要在的文件夹的inodeId, left1是目标文件的文件名
     * 现在要做的是在targetId对应的6个block中找到源文件的dirItem，再找到dirItem指向的inode，保存一份
     * 创建一个新的inode,类型为file。将上面保存的inode的信息拷贝给这个新的inode，主要是要把6个block的信息拷贝一下
     * 然后在targetId1对应的6个block中添加一个dirItem, 名字是left1
     */
    int sourceInode = findFolderOrFile(targetId, left, FILE_T);
    int which = getBlockNum(sourceInode);
    int id = getInodeNum(sourceInode);
    char buffer[BLOCK_SIZE];
    // inode数组从第1个block开始.第0个是超级块
    disk_read_whole_block(which, buffer);
    iNode sourceI = ((iNode *) buffer)[id];
    uint32_t *block_point_s = sourceI.block_point;


    // 目标文件的inode, 调用touch先创建一个文件
    int targetInode = touch(des);
    int which1 = getBlockNum(targetInode);
    int id1 = getInodeNum(targetInode);
    char buffer1[BLOCK_SIZE];
    // inode数组从第1个block开始.第0个是超级块
    disk_read_whole_block(which1, buffer1);
    uint32_t *block_point_t = ((iNode *) buffer)[id1].block_point;

    for (int i = 0; i < 6; i++) {
        char temp[BLOCK_SIZE];
        disk_read_whole_block(block_point_s[i],temp);
        disk_write_whole_block(block_point_t[i],temp);
    }

    return -1;
}


/**
 *
 * @param inodeNumber
 * @return 返回第inodeNumber个inode，在哪个block
 */
uint32_t getBlockNum(uint32_t inodeNumber) {
    return inodeNumber / (BLOCK_SIZE / INODE_SIZE) + 1;
}

/**
 *
 * @param inodeNumber
 * @return 返回第inodeNumber个inode，在某个block中的第几个
 */
uint32_t getInodeNum(uint32_t inodeNumber) {
    return inodeNumber % (BLOCK_SIZE / INODE_SIZE);
}

/**
 *
 * @param blockNum 在第几块block
 * @param inodeNumInBlock 在块内的第几个inode
 * @return 总的第几个inode
 */
uint32_t getTotalInodeNum(uint32_t blockNum, uint32_t inodeNumInBlock) {
    return (blockNum - 1) * (BLOCK_SIZE / INODE_SIZE) + inodeNumInBlock;
}