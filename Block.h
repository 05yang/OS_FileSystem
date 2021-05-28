//
// Created by yuany on 2021/5/28.
//
#pragma once

int free_blk(int);//释放相应的磁盘块
int get_block();//获取磁盘块
void show_disk_usage();//显示磁盘使用情况
void show_inode_usage();//显示inode节点使用情况