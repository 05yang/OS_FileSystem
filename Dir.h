//
// Created by yuany on 2021/5/28.
//
#pragma once

#include <string>
#include "FileSystem.h"

using namespace std;
struct Dir {
    string name;
    int id;//目录对应的inode
};

int open_dir(int);//打开相应inode对应的目录
int close_dir(int);//保存相应inode的目录
int show_dir(int);//显示目录
int make_file(int, string, int);//创建新的目录或文件
int enter_child_dir(int, string);//进入子目录