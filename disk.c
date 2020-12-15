//
// Created by llincyaw on 2020/12/10.
//

#include "disk.h"
#include <stdio.h>

inline int get_disk_size() {
    return 4 * 1024 * 1024;
}

static FILE *disk;

static int create_disk() {
    FILE *tmp = fopen("../disks/disk", "w");
    for (int i = 0; i < get_disk_size(); i++) {
        fputc(0, tmp);
    }
    fclose(tmp);
}

int open_disk() {
    if (disk != 0) {
        fprintf(stderr, "Open_disk: The disk has been opened. \n");
        return -1;
    }
    disk = fopen("../disks/disk", "r+");
    if (disk == 0) {
        create_disk();
        disk = fopen("../disks/disk", "r+");
        if (disk == 0) {
            fprintf(stderr, "Open_disk: Open disk failed.\n");
            return -1;
        }
    }
    return 0;
}

int disk_read_block(unsigned int block_num, char *buf) {
    if (disk == 0) {
        fprintf(stderr, "Disk_read_block: Disk is not opened. \n");
        return -1;
    }
    if (block_num * DEVICE_BLOCK_SIZE >= get_disk_size()) {
        fprintf(stderr, "Disk_read_block: Disk does not have such spaces.\n");
        return -1;
    }
    if (fseek(disk, block_num * DEVICE_BLOCK_SIZE, SEEK_SET)) {
        fprintf(stderr, "Disk_read_block: Move pointer failed.\n");
        return -1;
    }
    if (fread(buf, DEVICE_BLOCK_SIZE, 1, disk) != 1) {
        fprintf(stderr, "Disk_read_block: Read disk failed.\n");
        return -1;
    }
    return 0;
}

int disk_write_block(unsigned int block_num, char *buf) {
    if (disk == 0) {
        fprintf(stderr, "Disk_write_block: Disk is not opened. \n");
        return -1;
    }
    if (block_num * DEVICE_BLOCK_SIZE >= get_disk_size()) {
        fprintf(stderr, "Disk_write_block: Disk does not have such spaces.\n");
        return -1;
    }
    if (fseek(disk, block_num * DEVICE_BLOCK_SIZE, SEEK_SET)) {
        fprintf(stderr, "Disk_write_block: Move pointer failed.\n");
        return -1;
    }
    if (fwrite(buf, DEVICE_BLOCK_SIZE, 1, disk) != 1) {
        fprintf(stderr, "Disk_write_block: Write disk failed.\n");
        return -1;
    }
    return 0;
}

int close_disk() {
    if (disk == 0) {
        fprintf(stderr, "Close_disk: disk has been closed. \n");
        return -1;
    }
    int r = fclose(disk);
    disk = 0;
    return r;
}


void disk_write(unsigned int block_num, char *buf) {
    if (open_disk() == -1) {
        fprintf(stderr, "Errors in disk_write -- open_disk\n");
        return;
    }
    if (disk_write_block(block_num, buf) == -1) {
        fprintf(stderr, "Errors in disk_write -- disk_write_block\n");
        return;
    }
    if (close_disk() == -1) {
        fprintf(stderr, "Errors in disk_write -- close_disk\n");
        return;
    }
}

void disk_read(unsigned int block_num, char *buf) {
    if (open_disk() == -1) {
        fprintf(stderr, "Errors in disk_read -- open_disk\n");
        return;
    }
    if (disk_read_block(block_num, buf) == -1) {
        fprintf(stderr, "Errors in disk_read -- disk_write_block\n");
        return;
    }
    if (close_disk() == -1) {
        fprintf(stderr, "Errors in disk_read -- close_disk\n");
        return;
    }
}


void disk_write_whole_block(unsigned int block_num, char *buf) {
    disk_write(2 * block_num, buf);
    disk_write(2 * block_num + 1, buf + DEVICE_BLOCK_SIZE);
}

void disk_read_whole_block(unsigned int block_num, char *buf) {
    disk_read(2 * block_num, buf);
    disk_read(2 * block_num + 1, buf + DEVICE_BLOCK_SIZE);
}