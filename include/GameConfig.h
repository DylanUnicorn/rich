#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Player.h"

namespace richgame {

/**
 * @brief 游戏配置类，负责游戏初始化设置
 */
class GameConfig {
public:
    /**
     * @brief 构造函数
     */
    GameConfig();

    /**
     * @brief 设置初始资金
     * @param money 初始资金（1000-50000）
     * @return 是否设置成功
     */
    bool setInitialMoney(int money);

    /**
     * @brief 获取初始资金
     * @return 初始资金
     */
    int getInitialMoney() const;

    /**
     * @brief 从用户输入获取初始资金设置
     * @param input 输入流
     * @param output 输出流
     */
    void promptForInitialMoney(std::istream& input = std::cin, 
                              std::ostream& output = std::cout);

    /**
     * @brief 从用户输入获取玩家选择
     * @param playerManager 玩家管理器
     * @param input 输入流
     * @param output 输出流
     * @return 是否选择成功
     */
    bool promptForPlayerSelection(PlayerManager& playerManager,
                                 std::istream& input = std::cin,
                                 std::ostream& output = std::cout);

    /**
     * @brief 验证玩家选择字符串
     * @param selection 选择字符串
     * @return 是否有效
     */
    static bool isValidPlayerSelection(const std::string& selection);

    /**
     * @brief 将选择字符串转换为角色列表
     * @param selection 选择字符串
     * @return 角色列表
     */
    static std::vector<Player::Character> parsePlayerSelection(const std::string& selection);

private:
    int initialMoney_;
    
    /**
     * @brief 显示玩家选择提示
     * @param output 输出流
     */
    void displayPlayerSelectionPrompt(std::ostream& output) const;
};

} // namespace richgame