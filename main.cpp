#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "FileSystem.h"
#include "Dir.h"
#include "Path.h"
#include "Inode.h"

using namespace std;
Dir dir_table[MaxDirNum];//将当前目录文件的内容都载入内存
int dir_num;//相应编号的目录项数
int inode_num;//当前目录的inode编号
FILE *Disk;
Inode curr_inode;//当前目录的inode结构
Super super_blk;//文件系统的超级块
string path = "root@YYH_2021:";
const vector<string> command = {
        "mkfs",//0 初始化文件系统
        "q",//1 退出
        "mkdir",//2 新建文件夹
        "rmdir",//3 删除文件夹
        "cd",//4 进入子目录
        "ls",//5 列出目录内容
        "touch",//6 新建文件
        "rm",//7 删除文件
        "vi",//8 编辑文件
        "cp",//9 复制文件
        "mv",//10 移动文件
        "stat",//11 查看文件属性
        "chmod",//12 权限设置
        "df",//13 查看磁盘和inode节点使用情况
        "man",//14 查看使用手册
        "ps"
};

FILE_SYSTEM Sys;

int main() {
    string com;
    FILE *Disk = fopen(DISK, "rb+");
    if(!Disk) {
        cout << "open fail" << endl;
        exit(-1);
    }
    Sys.Init();
    while (1) {
        cout << path << "# ";
        cin >> com;
        int op = -1;
        for (int i = 0; i < CommandNum; ++i) {
            if(com == command[i]) {
                op = i;
                break;
            }
        }
        if(op == 0) {//初始化文件系统
            Sys.Format();
        } else if(op == 1) {//退出
            Sys.Save();
            break;
        } else if(op == 2) {
            string name;
            cin >> name;
        } else if(op == 5) {
            show_dir(inode_num);
        } else if(op == 6) {
            string name;
            cin >> name;
            make_file(inode_num, name, File);
        } else {
            cout << com << " command not found" << endl;
            char ch;
            while ((ch = getchar()) != '\n' && ch != EOF);//指令错误，清空输入缓冲区
        };
        Sys.Save();
    }
    fclose(Disk);
    return 0;
}
