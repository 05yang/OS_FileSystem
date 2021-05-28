//
// Created by yuany on 2021/5/28.
//

#include <ctime>
#include "Inode.h"
#include "Block.h"
#include "Dir.h"

int apply_inode()//申请Inode
{
    if(super_blk.inode_used >= InodeNum) {
        return -1;//inode节点用完
    }
    super_blk.inode_used++;
    for (int i = 0; i < InodeNum; ++i) {
        if(!super_blk.inode_map[i]) {//找到一个空的i节点
            super_blk.inode_map[i] = 1;
            return i;
        }
    }
    return 0;
}

int free_inode(int inode) {
    Inode temp;
    fseek(Disk, InodeBeg + sizeof(Inode) * inode, SEEK_SET);
    fread(&temp, sizeof(Inode), 1, Disk);

    for (int i = 0; i < temp.block_num; ++i)
        free_blk(temp.block_id[i]);
    super_blk.inode_map[inode] = 0;
    super_blk.inode_used--;
    return 1;
}

int init_file_inode(int inode)//初始化新建文件的indoe
{
    Inode temp;
    fseek(Disk, InodeBeg + sizeof(Inode) * inode, SEEK_SET);//读取相应的Inode
    fread(&temp, sizeof(Inode), 1, Disk);

    temp.block_num = 0;
    temp.type = File;
    temp.file_size = 0;
    temp.access[0] = 1;//可读
    temp.access[1] = 1;//可写
    temp.last_modify_time = time(nullptr);
    temp.last_interview_time = time(nullptr);
    temp.last_state_change_time = time(nullptr);

    fseek(Disk, InodeBeg + sizeof(Inode) * inode, SEEK_SET);//将初始化的Inode写回
    fwrite(&temp, sizeof(Inode), 1, Disk);
    return 1;
}

/*初始化新建目录的inode*/
int init_dir_inode(int child, int father) {
    Inode temp;
    Dir dot[2];
    int blk_pos;

    fseek(Disk, InodeBeg + sizeof(Inode) * child, SEEK_SET);
    fread(&temp, sizeof(Inode), 1, Disk);

    blk_pos = get_block();//获取新磁盘块的编号

    temp.block_num = 1;
    temp.block_id[0] = blk_pos;
    temp.type = Directory;
    //printf("temp.type = %d\n", temp.type);
    temp.file_size = 2 * sizeof(Dir);
    temp.access[0] = 1;//可读
    temp.access[1] = 1;//可写
    temp.last_modify_time = time(nullptr);
    temp.last_interview_time = time(nullptr);
    temp.last_state_change_time = time(nullptr);

    /*将初始化完毕的Inode结构写回*/
    fseek(Disk, InodeBeg + sizeof(Inode) * child, SEEK_SET);
    fwrite(&temp, sizeof(Inode), 1, Disk);

    dot[0].name = ".";
    dot[0].id = child;

    dot[1].name = "..";
    dot[1].id = father;

    fseek(Disk, BlockBeg + BlkSize * blk_pos, SEEK_SET);//将新目录的数据写进数据块
    fwrite(dot, sizeof(Dir), 2, Disk);

    return 1;
}