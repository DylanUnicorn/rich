#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <Tool.h>
#include <Player.h>

typedef struct {
    int id; //地块编号
    Player owner;//主人信息
    char type;//Hospital/Prison/Magic/Gold...
    int level;//当前地产等级
    int money;//地产基础价格
    Tool tool;//地块被放置道具
} Structure;

#endif