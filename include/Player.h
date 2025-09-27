#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Tool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 玩家角色枚举
 */
typedef enum {
    QIAN_FUREN = 1,  /* 钱夫人（Q） */
    ATURBER = 2,     /* 阿土伯（A） */
    SUN_XIAOMEI = 3, /* 孙小美（S） */
    JIN_BEIBEI = 4   /* 金贝贝（J） */
} PlayerCharacter;

/**
 * @brief 玩家结构体，表示大富翁游戏中的玩家
 */
typedef struct {
    PlayerCharacter character;
    int money;
    int points;
    int position;
    bool in_hospital;       //标志是否在医院
    bool in_prison;         //标志是否在监狱
    int hospital_days;      //住院天数
    int prison_days;        //服刑天数
    int points_multiplier;  //点数倍数（1或2）
    int god_bless_days;     //倍数剩余天数
    bool bankruptcy;        //标志是否破产 
    bool god;               //标志是否财神附体
    int house[70];          //当前拥有的房产编号
    Tool tool;              //当前拥有的道具
} Player;

/**
 * @brief 玩家管理器结构体，负责管理所有玩家
 */
#define MAX_PLAYERS 4
typedef struct {
    Player players[MAX_PLAYERS];
    int playerCount; // 开局玩家数量
    int playerIndex;  // 实时玩家数量
    int currentPlayerIndex;
} PlayerManager;

/**
 * @brief 创建玩家
 * @param player 玩家指针
 * @param character 玩家角色
 * @param initialMoney 初始资金
 */
void player_init(Player* player, PlayerCharacter character, int initialMoney);

/**
 * @brief 获取玩家名称
 * @param character 玩家角色
 * @return 玩家名称
 */
const char* player_getName(PlayerCharacter character);

/**
 * @brief 获取玩家颜色代码
 * @param character 玩家角色
 * @return 颜色代码字符
 */
char player_getColorCode(PlayerCharacter character);

/**
 * @brief 获取当前资金
 * @param player 玩家指针
 * @return 当前资金
 */
int player_getMoney(const Player* player);

/**
 * @brief 设置资金
 * @param player 玩家指针
 * @param money 资金数额
 */
void player_setMoney(Player* player, int money);

/**
 * @brief 获取点数
 * @param player 玩家指针
 * @return 当前点数
 */
int player_getPoints(const Player* player);

/**
 * @brief 设置点数
 * @param player 玩家指针
 * @param points 点数
 */
void player_setPoints(Player* player, int points);

/**
 * @brief 获取当前位置
 * @param player 玩家指针
 * @return 地图位置
 */
int player_getPosition(const Player* player);

/**
 * @brief 设置位置
 * @param player 玩家指针
 * @param position 地图位置
 */
void player_setPosition(Player* player, int position);

/**
 * @brief 判断玩家是否破产
 * @param player 玩家指针
 * @return 是否破产
 */
bool player_isBankrupt(const Player* player);

/* PlayerManager 函数 */

/**
 * @brief 初始化玩家管理器
 * @param manager 玩家管理器指针
 */
void playerManager_init(PlayerManager* manager);

/**
 * @brief 添加玩家
 * @param manager 玩家管理器指针
 * @param character 玩家角色
 * @param initialMoney 初始资金
 * @return 是否成功添加
 */
bool playerManager_addPlayer(PlayerManager* manager, PlayerCharacter character, int initialMoney);

/**
 * @brief 获取玩家数量
 * @param manager 玩家管理器指针
 * @return 玩家数量
 */
int playerManager_getPlayerCount(const PlayerManager* manager);

/**
 * @brief 获取当前玩家
 * @param manager 玩家管理器指针
 * @return 当前玩家指针
 */
Player* playerManager_getCurrentPlayer(PlayerManager* manager);

/**
 * @brief 获取当前玩家索引
 * @param manager 玩家管理器指针
 * @return 当前玩家索引
 */
int playerManager_getCurrentPlayerIndex(const PlayerManager* manager);

/**
 * @brief 切换到下一个玩家
 * @param manager 玩家管理器指针
 */
void playerManager_nextPlayer(PlayerManager* manager);

/**
 * @brief 移除破产玩家
 * @param manager 玩家管理器指针
 * @param index 玩家索引
 */
void playerManager_removeBankruptPlayer(PlayerManager* manager, int index);

/**
 * @brief 重置所有玩家状态
 * @param manager 玩家管理器指针
 */
void playerManager_reset(PlayerManager* manager);

/**
 * @brief 获取指定索引的玩家
 * @param manager 玩家管理器指针
 * @param index 玩家索引
 * @return 玩家指针，如果索引无效则返回NULL
 */
Player* playerManager_getPlayer(PlayerManager* manager, int index);
bool playerManager_isGameWon(const PlayerManager* manager);
Player* playerManager_getWinner(PlayerManager* manager);

#ifdef __cplusplus
}
#endif

#endif /* PLAYER_H */