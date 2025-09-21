#ifndef   PROPHOUSE_H
#define   PROPHOUSE_H

#include <stdio.h>
#include <Player.h>
#include <Tool.h>

void propHouse_init();
/**
 * @brief 道具屋初始化
 * @return 无
 */
void player_getintoPropHouse(Player* currentPlayer);
/**
 * @brief 玩家进入道具屋购买道具
 * @param currentPlayer 当前玩家指针
 * @return 无
 */
#endif