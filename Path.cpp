//
// Created by yuany on 2021/5/28.
//

#include "Path.h"
#include "Dir.h"
#include "Inode.h"

extern char path[40];
extern int inode_num;//当前目录的inode编号
extern int dir_num;//相应编号的目录项数
extern Dir dir_table[MaxDirNum];//将当前目录文件的内容都载入内存
extern FILE *Disk;

int check_name(int inode, string name) {
    for (int i = 0; i < dir_num; ++i) {
        if(name == dir_table[i].name)
            return dir_table[i].id;
    }
    return -1;
}

string Get_abslutionpath(string str) {
    string abslutionpath;
    int len_str = str.size();
    for (int i = len_str; i >= 0; i--) {
        if(str[i] == '/') break;
        else abslutionpath += str[i];
    }
    reverse(abslutionpath.begin(), abslutionpath.end());
    return abslutionpath;
}

int type_check(string name) {
    Inode temp;
    for (int i = 0; i < dir_num; ++i) {
        if(name == dir_table[i].name) {
            int inode = dir_table[i].id;
            fseek(Disk, InodeBeg + sizeof(Inode) * inode, SEEK_SET);
            fread(&temp, sizeof(Inode), 1, Disk);
            return temp.type;
        }
    }
    return -1;//该文件或目录不存在
}

int split(vector<string> &dst, string str, char spl) {
    string tmp;
    for (char i : str) {
        if(i == spl) {
            dst.push_back(tmp);
        } else tmp += i;
    }
    if(!tmp.empty()) dst.push_back(tmp);
    return dst.size();
}

int eat_path(string &name) {
    int tmp = inode_num;//记录原始inode节点
    vector<string> dst;
    int cnt = split(dst, name, '/');
    if(name[0] == '/') {//从根目录开始
        //printf("1111111111\n");
        close_dir(inode_num);
        inode_num = 0;
        open_dir(inode_num);
    }
    for (int i = 0; i < cnt - 1; i++) {
        //printf("%d\n", i);
        int res = enter_child_dir(inode_num, dst[i]);
        if(res == -1) {
            inode_num = tmp;
            open_dir(inode_num);
            return -1;
        }
    }
    if(cnt == 0) name = ".";
    else name = dst[cnt - 1];
    return 0;
}