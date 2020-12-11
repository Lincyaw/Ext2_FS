
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
        } else if (cmd[0] == 'q') {
            break;
        } else if (strcmp(cmd, "printSP\n") == 0) {
            testInitSP();
        } else {
            printf("%s", cmd);
        }
    }
    return 0;
}
