#include <iostream>
#include "GameConfig.h"
#include "Player.h"

int main() {
    std::cout << "=== Rich Game ===" << std::endl;
    
    richgame::GameConfig config;
    richgame::PlayerManager playerManager;
    
    // 设置初始资金
    config.promptForInitialMoney();
    
    // 选择玩家
    if (config.promptForPlayerSelection(playerManager)) {
        std::cout << "\nGame initialization completed!" << std::endl;
        std::cout << "Participating players:" << std::endl;
        
        const auto& players = playerManager.getPlayers();
        for (size_t i = 0; i < players.size(); ++i) {
            const auto& player = players[i];
            std::cout << i + 1 << ". " << player.getName() 
                      << "(" << player.getColorCode() << ") - "
                      << "Money: " << player.getMoney() << " yuan" << std::endl;
        }
        
        std::cout << "\nCurrent turn: " << playerManager.getCurrentPlayer().getName() << std::endl;
    } else {
        std::cout << "Game initialization failed!" << std::endl;
        return 1;
    }
    
    return 0;
}