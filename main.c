#include "disk.h"
#include "fs.h"
#include "testModule.h"

int main() {
    printf("Hello, World!\n");
    initExt2();
    while (1) {
        printf("> ");
        memset(cmd, 0, 1024);
        fgets(cmd, sizeof(cmd), stdin);
        if (cmd[0] == 0) {
            exit(0);
        }
        char *newCmd = trim(cmd);
        char *left;
        char *right;
        rmEnter(cmd);
        left = newCmd;
        right = simple_tok(left, ' ');
        if (strcmp(left, "ls") == 0) {
            if (left == right) {
#if DEBUG == 1
                printf("\tInstruction:\tls (current folder)\n");
#endif
            } else {
#if DEBUG == 1
                printf("\tInstruction:\tmkdir\n\tDest Folder:\t%s\n", right);
#endif
            }
        } else if (strcmp(left, "mkdir") == 0) {
            if (left == right) {
                fprintf(stderr, "Please input a folder name.\n");
            } else {
#if DEBUG == 1
                printf("\tInstruction:\tmkdir\n\tFolder name:\t%s\n", right);
#endif
            }
        } else if (strcmp(left, "touch") == 0) {
            if (left == right) {
                fprintf(stderr, "Please input a file name.\n");
            } else {
                touch(right);
#if DEBUG == 1
                printf("\tInstruction:\ttouch\n\tFile name:\t%s\n", right);
#endif
            }

        } else if (strcmp(left, "cp") == 0) {
            if (left == right) {
                fprintf(stderr, "Please input a source file name.\n");
            } else {
                left = right;
                right = simple_tok(left, ' ');
                if (left == right) {
                    fprintf(stderr, "Please input a destination file name.\n");
                } else {
                    // 此时，right指向的应该是目标文件地址串，left指向的是源文件地址串
#if DEBUG == 1
                    printf("\tInstruction:\tcp\n\tSource file:\t%s\n\tDestination file:\t%s\n", left, right);
#endif
                }
            }
        } else if (strcmp(left, "shutdown") == 0) {
            fprintf(stderr, "Thanks to use this file system.\n");
            exit(1);
        } else {
            printf("%s\n", cmd);
        }

#if DEBUG == 1
        if (strcmp(left, "printSP") == 0) {
            disk_read_whole_block(0,buffer);
            printSuperBlock((sp_block *) buffer);
        }
        if (strcmp(left, "printIN") == 0) {
            testCreateInode();
        }
        if (strcmp(left, "bitset") == 0) {
            testBitSet();
        }
#endif
    }
    return 0;
}
