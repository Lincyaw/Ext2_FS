# Ext2_FS
Hitsz [2020秋青春版Ext2](https://hitsz-lab.gitee.io/os_lab/lab5/part1/)

- 环境：ubuntu 20.04
    - 实际上windows也可以跑。只需要在File选项中,选择Reload Cmake Project,即可生成
    一个cmake-build-debug文件夹，然后就可以了。
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
首先在根目录的`inode`指向的`block`中，找到`tmp`所在的`dir_item`, 然后根据这个`dir_item`找到`tmp`文件夹所在的`inode`
，再在`tmp`的`inode`指向的`block`中，找到`testDir`所在的`dir_item`，再找到`testDir`所在的`inode`。

此时如果要进行`ls`命令，就可以根据当前目录，即`testDir`的inode，找到对应的`block`，然后打印出
在这个文件夹下，所有的信息。


# 设计接口

本次实验的任务:

1. 创建文件/文件夹（数据块可预分配）；
2. 读取文件夹内容；
3. 复制文件；
4. 关闭系统；
5. 系统关闭后，再次进入该系统还能还原出上次关闭时系统内的文件部署。

需要实现的命令:

1. `ls` - 展示读取文件夹内容
2. `mkdir `- 创建文件夹
3. `touch` - 创建文件
4. `cp `- 复制文件
5. `shutdown `- 关闭系统



在首次启动文件系统时，需要初始化。这部分的初始化选项可以交给用户，作为一个格式化磁盘的选项。清除所有数据，重写超级块的内容。

对于新建文件的命令，需要以下步骤

1. 判断在哪里创建一个文件，如果是根目录，则执行第二步，否则执行第四步
2. 创建一个`inode`。
3. 为`inode`分配6个数据块的指针，结束
4. 遍历所有类型是目录的`inode`，去里面找第一层文件夹的名字。
   比如`touch config/zsh/zsh_config.txt`，需要遍历所有在当前目录下(即根目录下)对应的`block`，在这些`block`中遍历，直到找到名字是`config`的`dir_item`，然后再根据这个`dir_item`中的`inode_id`找到对应的`inode`。此时当前目录相当于是在`config/`目录下了，一开始是在根目录下。重复以上步骤，直到找到`zsh`所在的inode，然后去这个`inode`的对应的`block`中追加一个`dir_item`，修改`valid`号，修改名称，设置目录项类型为文件。在`inode`数组中追加一个`inode`项，为其指定6个block，将这个inode的id给`dir_item`。
5. 结束。

从上面的几个步骤可以归纳出几个函数。

```C
1. createInode(); // 新建inode
2. createDirItem(); // 新建Diritem
3. findFolder(); // 遍历给定目录(输入一个id)对应的block中遍历，直到找到名字是xxx的dir_item, 返回inodeid
```

其中, 在寻找文件时,与寻找文件夹的步骤是一样的，所以可以合并成同一个函数。具体看`fs.h`中的定义。



对于创建文件夹的命令，与创建文件如出一辙，只需将类型改为文件夹即可。

对于`ls`命令，也差不多，只需要找到那个文件夹的id号之后，在其`block`中打印出所有的是`valid`的`dir_item`项即可。

还剩最后一个复制文件的命令。假设已经找到了对应的目录，则只需要在当前目录里的`block`中找到源文件，拷贝一份（新增）相关信息（`inode`），在这个目录下的`block`新建一个`diritem`，将这个新增的`inode`的`id`填到这个新建的`diritem`中。







# 实验bug记录

## 第五次调用`printIN`,创建第五个`inode`时,出现磁盘读写错误

出错路径为:

```c++
createInode -> disk_read_whole_block -> disk_read -> disk_read_block
    -> fread
```

接下来继续调用`disk_read_whole_block`中的第二个`disk_read`, 出现`open_disk`错误, `disk!=0`导致出错。

但不应该出现这种错误，在上一次调用`disk_read`的时候，已经关闭了磁盘访问了。

