#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Player.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 游戏配置结构体，负责游戏初始化设置
 */
typedef struct {
    int initialMoney;
} GameConfig;

/**
 * @brief 初始化游戏配置
 * @param config 游戏配置指针
 */
void gameConfig_init(GameConfig* config);

/**
 * @brief 设置初始资金
 * @param config 游戏配置指针
 * @param money 初始资金（1000-50000）
 * @return 是否设置成功
 */
bool gameConfig_setInitialMoney(GameConfig* config, int money);

/**
 * @brief 获取初始资金
 * @param config 游戏配置指针
 * @return 初始资金
 */
int gameConfig_getInitialMoney(const GameConfig* config);

/**
 * @brief 从用户输入获取初始资金设置
 * @param config 游戏配置指针
 */
void gameConfig_promptForInitialMoney(GameConfig* config);

/**
 * @brief 从用户输入获取玩家选择
 * @param config 游戏配置指针
 * @param playerManager 玩家管理器指针
 * @return 是否选择成功
 */
bool gameConfig_promptForPlayerSelection(GameConfig* config, PlayerManager* playerManager);

/**
 * @brief 验证玩家选择字符串
 * @param selection 选择字符串
 * @return 是否有效
 */
bool gameConfig_isValidPlayerSelection(const char* selection);

/**
 * @brief 将选择字符串转换为角色数组
 * @param selection 选择字符串
 * @param characters 输出的角色数组
 * @param maxCount 数组最大容量
 * @return 解析出的角色数量
 */
int gameConfig_parsePlayerSelection(const char* selection, PlayerCharacter* characters, int maxCount);

/**
 * @brief 显示玩家选择提示
 */
void gameConfig_displayPlayerSelectionPrompt(void);

#ifdef __cplusplus
}
#endif

#endif /* GAMECONFIG_H */