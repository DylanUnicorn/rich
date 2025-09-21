#include "roll.h"
#include <stdlib.h>
#include <time.h>
/**
 * @brief 模拟掷骰子，返回1到6之间的随机数
 * @return 掷骰子的结果
 */
int roll_dice() {
    return (rand() % 6) + 1; // 生成1到6之间的随机数
}

void Player_use_roll_dice(Player* currentPlayer) {
    srand(time(NULL));
    int roll = roll_dice();
    printf("玩家 %s 掷出了 %d 点，\n", player_getName(currentPlayer->character), roll);
    currentPlayer->position = (currentPlayer->position + roll) % 70; // 假设地图有70个位置
    printf(" 移动到位置 %d\n", currentPlayer->position);
}