#ifndef  ROLL_H
#define  ROLL_H
#include <stdio.h>
#include <Player.h>

int roll_dice() ;
/**
 * @brief 模拟掷骰子，返回1到6之间的随机数
 * @return 掷骰子的结果
 */
void Player_use_roll_dice(Player* currentPlayer);
/**
 * @brief 模拟玩家掷骰子，并更新玩家位置
 * @param currentPlayer 当前玩家指针
 * @return 掷骰子的结果
 */
#endif