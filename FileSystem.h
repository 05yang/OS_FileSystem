//
// Created by yuany on 2021/5/27.
//
#pragma once

#include <iostream>

#define InodeNum    1024     //i节点数量
#define BlkNum (80*1024) //磁盘块的数目
#define BlkSize 1024     //磁盘块大小为1K
#define BlkPerNode 1024  //每个文件包含的最大的磁盘块数目
#define DISK "./disk/disk.txt"//模拟磁盘
#define BUFF "./disk/buff.txt"  //读写文件时的缓冲文件
#define SuperBeg 0  //超级块的起始地址
#define InodeBeg sizeof(Super) //i节点起始地址
#define BlockBeg (InodeBeg+InodeNum*sizeof(Inode) )//数据区起始地址
#define MaxDirNum (BlkPerNode*(BlkSize/sizeof(Dir)))//每个目录最大的文件数
#define DirPerBlk (BlkSize/sizeof(Dir))//每个磁盘块包含的最大目录项
#define Directory 0
#define File 1
#define CommandNum command.size()//指令数目
#define NameLength 30 //文件名长度
#define ValueSize 256 //权值统计中，一个字节有0 255种可能取值
using namespace std;
struct Super {
    int block_map[BlkNum];//磁盘块位图
    int inode_map[InodeNum];//i节点位图
    int inode_used;//已被使用的i节点
    int block_used;//已被使用的磁盘块
};
class FILE_SYSTEM{
public:
    int Init();
    int Format();
    int Save();
};