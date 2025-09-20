#pragma once

#include <string>
#include <vector>

namespace richgame {

/**
 * @brief 玩家类，表示大富翁游戏中的玩家
 */
class Player {
public:
    /**
     * @brief 玩家角色枚举
     */
    enum class Character {
        QIAN_FUREN = 1,  // 钱夫人（Q）
        ATURBER = 2,     // 阿土伯（A）
        SUN_XIAOMEI = 3, // 孙小美（S）
        JIN_BEIBEI = 4   // 金贝贝（J）
    };

    /**
     * @brief 构造函数
     * @param character 玩家角色
     * @param initialMoney 初始资金
     */
    Player(Character character, int initialMoney);

    /**
     * @brief 获取玩家名称
     * @return 玩家名称
     */
    std::string getName() const;

    /**
     * @brief 获取玩家颜色代码
     * @return 颜色代码字符
     */
    char getColorCode() const;

    /**
     * @brief 获取玩家角色
     * @return 玩家角色
     */
    Character getCharacter() const;

    /**
     * @brief 获取当前资金
     * @return 当前资金
     */
    int getMoney() const;

    /**
     * @brief 设置资金
     * @param money 资金数额
     */
    void setMoney(int money);

    /**
     * @brief 获取点数
     * @return 当前点数
     */
    int getPoints() const;

    /**
     * @brief 设置点数
     * @param points 点数
     */
    void setPoints(int points);

    /**
     * @brief 获取当前位置
     * @return 地图位置
     */
    int getPosition() const;

    /**
     * @brief 设置位置
     * @param position 地图位置
     */
    void setPosition(int position);

    /**
     * @brief 判断玩家是否破产
     * @return 是否破产
     */
    bool isBankrupt() const;

private:
    Character character_;
    int money_;
    int points_;
    int position_;
};

/**
 * @brief 玩家管理器类，负责管理所有玩家
 */
class PlayerManager {
public:
    /**
     * @brief 构造函数
     */
    PlayerManager();

    /**
     * @brief 添加玩家
     * @param character 玩家角色
     * @param initialMoney 初始资金
     * @return 是否成功添加
     */
    bool addPlayer(Player::Character character, int initialMoney);

    /**
     * @brief 获取所有玩家
     * @return 玩家列表
     */
    const std::vector<Player>& getPlayers() const;

    /**
     * @brief 获取当前玩家
     * @return 当前玩家的引用
     */
    Player& getCurrentPlayer();

    /**
     * @brief 获取当前玩家索引
     * @return 当前玩家索引
     */
    int getCurrentPlayerIndex() const;

    /**
     * @brief 切换到下一个玩家
     */
    void nextPlayer();

    /**
     * @brief 移除破产玩家
     * @param index 玩家索引
     */
    void removeBankruptPlayer(int index);

    /**
     * @brief 获取剩余玩家数量
     * @return 剩余玩家数量
     */
    size_t getPlayerCount() const;

    /**
     * @brief 重置所有玩家状态
     */
    void reset();

private:
    std::vector<Player> players_;
    int currentPlayerIndex_;
};

} // namespace richgame