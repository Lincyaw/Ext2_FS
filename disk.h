//
// Created by llincyaw on 2020/12/10.
//

#ifndef EXT2_FS_DISK_H
#define EXT2_FS_DISK_H

#define DEVICE_BLOCK_SIZE 512


void disk_write_whole_block(unsigned int block_num, char *buf);

void disk_read_whole_block(unsigned int block_num, char *buf);

/**
 * 封装后的disk_write_block,读写安全
 * @param block_num
 * @param buf
 */
void disk_write(unsigned int block_num, char *buf);

/**
 * 封装后的disk_write_block,读写安全
 * @param block_num
 * @param buf
 */
void disk_read(unsigned int block_num, char *buf);


// Total disk size in bytes, 4 * 1024 * 1024 bytes (4 MiB) in total
int get_disk_size();

/**
 * @brief Open the virtual disk.
 *
 * @return returns 0 on success, -1 otherwise.
 *
 * @note This function will open a file named "disk" as a vritual disk
 * If the file is not found, it will try to create the file, and fill it with zeros of 4 MiB.
 * This function must be called before any calls to disk_read_block() and disk_write_block().
 * This function will fail if the disk is already opened.
 */
int open_disk();

/**
 * @brief Close the virtual disk.
 *
 * @return returns 0 on success, -1 otherwise.
 *
 * @note This function will close the virtual disk file.
 * After calling this function, all calls to disk_read_block() and disk_write_block() will fail
 * util open_disk() is called again.
 */
int close_disk();

/**
 * @brief Fill buf with the content of the block_num-th block.
 *
 * @param block_num The index of the block to be read.
 * @param buf       The pointer to the space where the function shall place the block content.
 * @return returns 0 on success, -1 otherwise.
 *
 * @note The space of buf should be no less than DEVICE_BLOCK_SIZE.
 * Make sure open_disk() is called before calling this function.
 */
int disk_read_block(unsigned int block_num, char *buf);

/**
 * @brief Write content of buf to the block_num-th block.
 *
 * @param block_num The index of the block to be written.
 * @param buf       The pointer to the space where the data to be written to disk is placed.
 * @return returns 0 on success, -1 otherwise.
 *
 * @note Make sure open_disk() is called before calling this function.
 */
int disk_write_block(unsigned int block_num, char *buf);

#endif //EXT2_FS_DISK_H
