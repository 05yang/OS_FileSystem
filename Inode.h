//
// Created by yuany on 2021/5/28.
//
#pragma once

#include "FileSystem.h"

extern Super super_blk;//文件系统的超级块
extern FILE *Disk;
struct Inode {
    int block_id[BlkPerNode];//占用磁盘块编号
    int block_num;//占用的磁盘块数目
    int file_size;//文件大小
    int type;//文件类型
    int access[2];//权限
    time_t last_interview_time;//最后访问时间
    time_t last_modify_time;//最后修改时间
    time_t last_state_change_time;//最后状态改变时间
};

int apply_inode();//申请Inode
int init_dir_inode(int, int);//初始化新建目录的inode
int init_file_inode(int);//初始化新建文件的inode
int free_inode(int);//释放相应的inode