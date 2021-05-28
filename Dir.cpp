//
// Created by yuany on 2021/5/28.
//
#pragma once

#include "Dir.h"
#include "Path.h"
#include "Block.h"
#include "Inode.h"

using namespace std;
extern Dir dir_table[MaxDirNum];//将当前目录文件的内容都载入内存
extern int dir_num;             //相应编号的目录项数
extern int inode_num;           //当前目录的inode编号
extern Inode curr_inode;        //当前目录的inode结构
extern Super super_blk;         //文件系统的超级块
extern FILE *Disk;
extern string path;

int open_dir(int inode) {
    int pos = 0;
    int left;
    fseek(Disk, InodeBeg + sizeof(Inode) * inode, SEEK_SET);//SEEK_SET	文件的开头

    /*读出相应的i节点*/
    fread(&curr_inode, sizeof(Inode), 1, Disk);

    for (int i = 0; i < curr_inode.block_num - 1; i++) {
        fseek(Disk, BlockBeg + BlkSize * curr_inode.block_id[i], SEEK_SET);
        fread(dir_table + pos, sizeof(Dir), DirPerBlk, Disk);
        pos += DirPerBlk;//每次跳一个磁盘块这么多的目录项
    }

    /*left为最后一个磁盘块内的目录项数*/
    left = curr_inode.file_size / (int) sizeof(Dir) - DirPerBlk * (curr_inode.block_num - 1);
    fseek(Disk, BlockBeg + BlkSize * curr_inode.block_id[curr_inode.block_num], SEEK_SET);
    fread(dir_table + pos, sizeof(Dir), left, Disk);//最后一部分的文件
    pos += left;
    dir_num = pos;

    return 1;
}

int close_dir(int inode) {
    int i, pos = 0, left;

    fseek(Disk, InodeBeg + sizeof(Inode) * inode, SEEK_SET);//读出相应的i节点
    fread(&curr_inode, sizeof(Inode), 1, Disk);

    for (i = 0; i < curr_inode.block_num - 1; ++i) {//数据写回磁盘块
        fseek(Disk, BlockBeg + BlkSize * curr_inode.block_id[i], SEEK_SET);
        fwrite(dir_table + pos, sizeof(Dir), DirPerBlk, Disk);
        pos += DirPerBlk;
    }

    left = dir_num - pos;
    //	printf("left:%d",left);
    fseek(Disk, BlockBeg + BlkSize * curr_inode.block_id[i], SEEK_SET);
    fwrite(dir_table + pos, sizeof(Dir), left, Disk);

    curr_inode.file_size = dir_num * sizeof(Dir);//inode写回
    fseek(Disk, InodeBeg + inode * sizeof(Inode), SEEK_SET);
    fwrite(&curr_inode, sizeof(curr_inode), 1, Disk);

    return 1;

}

int enter_child_dir(int inode, string name) {//进入子目录
    if(type_check(name) != Directory)
        return -1;
    int child = check_name(inode, name);
    /*关闭当前目录,进入下一级目录*/
    close_dir(inode);
    inode_num = child;
    open_dir(child);
    return 1;
}

int make_file(int inode, string name, int type) {
    string original_name_path;
    int original_inode = inode_num;//记录当前的inode
    original_name_path = name;
    if(eat_path(name) == -1) {
        if(type == File) cout << "touch: cannot touch‘" << original_name_path << "’: No such file or directory" << endl;
        if(type == Directory)
            cout << "touch: cannot touch‘" << original_name_path << "’: No such file or directory" << endl;
        return -1;
    }

    int new_node;
    int blk_need = 1;//本目录需要增加磁盘块则blk_need=2
    int t;
    Inode temp;

    fseek(Disk, InodeBeg + sizeof(Inode) * inode, SEEK_SET);//读取当前目录的Inode
    fread(&temp, sizeof(Inode), 1, Disk);

    if(temp.access[1] == 0) { //当前目录不允许写
        if(type == Directory)
            cout << "mkdir: cannot create directory‘" << original_name_path << "’: Permission denied" << endl;
        if(type == File) cout << "touch: cannot touch‘" << original_name_path << "’: No such file or directory" << endl;
        close_dir(inode_num);
        inode_num = original_inode;
        open_dir(inode_num);
        return -1;
    }
    if(dir_num > MaxDirNum) {//超过了目录文件能包含的最大目录项
        if(type == Directory)
            cout << "mkdir: cannot create directory '" << original_name_path << "' : Directory full" << endl;
        if(type == File)
            cout << "touch: cannot create file '" << original_name_path << "' : Directory full" << endl;
        close_dir(inode_num);
        inode_num = original_inode;
        open_dir(inode_num);
        return -1;
    }

    if(check_name(inode, name) != -1) {//防止重命名
        if(type == Directory)
            cout << "mkdir: cannnot create directory '" << original_name_path << "' : Directory exist" << endl;
        if(type == File)
            cout << "touch: cannot create file '" << original_name_path << "' : File exist" << endl;
        close_dir(inode_num);
        inode_num = original_inode;
        open_dir(inode_num);
        return -1;
    }

    if(dir_num / DirPerBlk != (dir_num + 1) / DirPerBlk) {//本目录也要增加磁盘块
        blk_need = 2;
    }

    //	printf("blk_used:%d\n",super_blk.blk_used);
    if(super_blk.block_used + blk_need > BlkNum) {
        if(type == Directory)
            cout << "mkdir: cannot create directory '" << original_name_path << "' :Block used up" << endl;
        if(type == File)
            cout << "touch: cannot create file '" << original_name_path << "' : Block used up" << endl;
        close_dir(inode_num);
        inode_num = original_inode;
        open_dir(inode_num);
        return -1;
    }

    if(blk_need == 2) {//本目录需要增加磁盘块
        t = curr_inode.block_num++;
        curr_inode.block_id[t] = get_block();
    }

    /*申请inode*/
    new_node = apply_inode();

    if(new_node == -1) {
        if(type == Directory)
            cout << "mkdir: cannot create directory '" << original_name_path << "' :Inode used up" << endl;
        if(type == File)
            cout << "touch: cannot create file '" << original_name_path << "' : Inode used up" << endl;
        close_dir(inode_num);
        inode_num = original_inode;
        open_dir(inode_num);
        return -1;
    }

    if(type == Directory) {//初始化新建目录的inode
        init_dir_inode(new_node, inode);
    } else if(type == File) {//初始化新建文件的inode
        init_file_inode(new_node);
    }
    dir_table[dir_num].name = name;
    dir_table[dir_num++].id = new_node;

    close_dir(inode_num);
    inode_num = original_inode;
    open_dir(inode_num);
    return 0;
}

int show_dir(int inode) {
    Inode temp;
    fseek(Disk, InodeBeg + sizeof(Inode) * inode, SEEK_SET);
    fread(&temp, sizeof(Inode), 1, Disk);//读超级块
    if(temp.access[0] == 0) { //目录无读写权限
        printf("cannot open directory .: Permission denied\n");
        return -1;
    }
    for (int i = 0; i < dir_num; i++) {
        int inode_tmp = check_name(inode_num, dir_table[i].name);//获取inode值
        Inode tempInode;
        fseek(Disk, InodeBeg + sizeof(Inode) * inode_tmp, SEEK_SET);
        fread(&tempInode, sizeof(Inode), 1, Disk);//定位到当前位置
        cout << dir_table[i].name << "    ";
        if(!((i + 1) % 5)) cout << endl;
    }
    if(dir_num % 5 != 0) cout << endl;
    return 1;
}