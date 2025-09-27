#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <Tool.h>
#include <Player.h>

typedef struct {
    int id; //地块编号
    Player* owner;//主人信息
    char type;//Hospital/Prison/Magic/Gold...
    int level;//当前地产等级
    int money;//地产基础价格
    int points;//地产基础点数
    ToolType tool;//地块被放置道具
} Structure;

typedef struct {
    bool god;           //是否财神附体
    int god_turn;      //财神出现的回合数
    int god_pos;       //财神位置
    int god_appear_turn; //财神出现的回合数
} God;


#endif