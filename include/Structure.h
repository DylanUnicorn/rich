#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <stdio.h>
#include <Tool.h>
#include <Player.h>

typedef struct {
    int id; //地块编号
    Player owner;//主人信息
    char type;//Hospital/Jail/Magic/MINE/Tool/Start/Gift/Property代表“房产地块”，首字母
    int level;//当前地产等级
    int money;//地产基础价格
    Tool tool;//地块被放置道具
} Structure;

#endif