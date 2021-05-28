//
// Created by yuany on 2021/5/28.
//

#include "Block.h"
#include "FileSystem.h"

using namespace std;
extern Super super_blk;//文件系统的超级块
int get_block()//申请未被使用的磁盘块
{
    super_blk.block_used++;//占用磁盘块+1
    for (int i = 0; i < BlkNum; ++i) {//找到未被使用的块
        if(!super_blk.block_map[i]) {
            super_blk.block_map[i] = 1;//标记占用
            return i;
        }
    }
    return -1;//没有多余的磁盘块
}

/*释放磁盘块*/
int free_blk(int blk_pos) {
    super_blk.block_used--;
    super_blk.block_map[blk_pos] = 0;
    return 0;
}