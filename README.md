# Ext2_FS
Hitsz [2020秋青春版Ext2](https://hitsz-lab.gitee.io/os_lab/lab5/part1/)

- 环境：ubuntu 20.04
- IDE: clion 2020.3
- 语言：C
- 标准：C11

需要注意的是，目录和文件的存放类型都是`inode`，通过`file_type`字段来区分。但是不同的是，如果一个
`inode`是文件类型的，他指向的一些`block`是 **全都是专属于这个文件的**，文件越大，占的`block`数
量越多。

如下图所示

![Ext2内存分布1](https://hitsz-lab.gitee.io/os_lab/lab5/part2.assets/file.svg)

而如果`inode`类型是文件夹。则该`inode`指向的`block`里存放的是一堆`dir_item`。`dir_item`
同样也有着文件和文件夹两种类型。通过`dir_item`结构体中的`type`字段标明。如果是文件，则这个
`dir_item`指向了`inode`数组中的某个`inode`，从而指向正确的文件。如果是文件夹，也同理，
再指向`inode`数组中的某个`inode`。然后重复这个过程，直到找到正确的文件或文件夹。

换句话说，在切换多级目录的时候，比如`cd /tmp/testDir`时，需要在`inode`和`block`之间来回跳跃两次。
首先在根目录的inode指向的block中，找到tmp所在的dir_item, 然后根据这个dir_item找到tmp文件夹所在的inode
，再在tmp的inode指向的block中，找到testDir所在的dir_item，再找到testDir所在的inode。

此时如果要进行`ls`命令，就可以根据当前目录，即`testDir`的inode，找到对应的`block`，然后打印出
在这个文件夹下，所有的信息。


![Ext2内存分布2](https://hitsz-lab.gitee.io/os_lab/lab5/part2.assets/dir.svg)