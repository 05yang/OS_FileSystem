//
// Created by yuany on 2021/5/27.
//
#include <cstring>
#include <ctime>
#include "FileSystem.h"
#include "Dir.h"
#include "Inode.h"

using namespace std;
extern Dir dir_table[MaxDirNum];//将当前目录文件的内容都载入内存
extern int dir_num;//相应编号的目录项数
extern int inode_num;//当前目录的inode编号
extern Inode curr_inode;//当前目录的inode结构
extern Super super_blk;//文件系统的超级块
extern FILE *Disk;
extern string path;

int FILE_SYSTEM::Init() {
    fseek(Disk, SuperBeg, SEEK_SET);
    fread(&super_blk, sizeof(Super), 1, Disk);//读取超级块
    inode_num = 0;//当前根目录的inode为0
    if(!open_dir(inode_num)) {
        printf("CANT'T OPEN ROOT DIRECTORY\n");
        return 0;
    }
    return 1;
}

int FILE_SYSTEM::Format() {
    /*格式化inode_map,保留根目录*/
    memset(super_blk.inode_map, 0, sizeof(super_blk.inode_map));
    super_blk.inode_map[0] = 1;
    super_blk.inode_used = 1;
    /*格式化blk_map,保留第一个磁盘块给根目录*/
    memset(super_blk.block_map, 0, sizeof(super_blk.block_map));
    super_blk.block_map[0] = 1;
    super_blk.block_used = 1;

    inode_num = 0;//将当前目录改为根目录

    /*读取根目录的i节点*/
    fseek(Disk, InodeBeg, SEEK_SET);
    fread(&curr_inode, sizeof(Inode), 1, Disk);
    //	printf("%d\n",curr_inode.file_size/sizeof(Dir));

    curr_inode.file_size = 2 * sizeof(Dir);
    curr_inode.block_num = 1;
    curr_inode.block_id[0] = 0;//第零块磁盘一定是根目录的
    curr_inode.type = Directory;
    curr_inode.access[0] = 1;//可读
    curr_inode.access[1] = 1;//可写
    curr_inode.last_interview_time = time(nullptr);
    curr_inode.last_modify_time = time(nullptr);
    curr_inode.last_state_change_time = time(nullptr);

    fseek(Disk, InodeBeg, SEEK_SET);
    fwrite(&curr_inode, sizeof(Inode), 1, Disk);

    dir_num = 2;//仅.和..目录项有效

    dir_table[0].name = ".";
    dir_table[0].id = 0;
    dir_table[1].name = "..";
    dir_table[1].id = 0;
    path = "root@YYH_2021:";
    return 1;
}

int FILE_SYSTEM::Save() {
    fseek(Disk, SuperBeg, SEEK_SET);
    fwrite(&super_blk, sizeof(Super), 1, Disk);
    close_dir(inode_num);
    return 1;
}