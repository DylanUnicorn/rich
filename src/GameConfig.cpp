#include "GameConfig.h"
#include <algorithm>
#include <sstream>

namespace richgame {

GameConfig::GameConfig() : initialMoney_(10000) {
}

bool GameConfig::setInitialMoney(int money) {
    if (money >= 1000 && money <= 50000) {
        initialMoney_ = money;
        return true;
    }
    return false;
}

int GameConfig::getInitialMoney() const {
    return initialMoney_;
}

void GameConfig::promptForInitialMoney(std::istream& input, std::ostream& output) {
    while (true) {
        output << "Please enter initial money (1000-50000), default 10000: ";
        
        std::string line;
        std::getline(input, line);
        
        // 如果是空行，使用默认值
        if (line.empty()) {
            initialMoney_ = 10000;
            break;
        }
        
        try {
            int money = std::stoi(line);
            if (setInitialMoney(money)) {
                break;
            } else {
                output << "Error: Initial money must be between 1000-50000, please re-enter." << std::endl;
            }
        } catch (const std::exception&) {
            output << "Error: Please enter a valid number." << std::endl;
        }
    }
    
    output << "Initial money set to: " << initialMoney_ << " yuan" << std::endl;
}

bool GameConfig::promptForPlayerSelection(PlayerManager& playerManager,
                                        std::istream& input,
                                        std::ostream& output) {
    playerManager.reset();
    
    while (true) {
        displayPlayerSelectionPrompt(output);
        
        std::string selection;
        std::getline(input, selection);
        
        if (isValidPlayerSelection(selection)) {
            auto characters = parsePlayerSelection(selection);
            
            // 添加所有选择的玩家
            bool allSuccess = true;
            for (auto character : characters) {
                if (!playerManager.addPlayer(character, initialMoney_)) {
                    allSuccess = false;
                    break;
                }
            }
            
            if (allSuccess) {
                output << "Player selection successful!" << std::endl;
                const auto& players = playerManager.getPlayers();
                for (const auto& player : players) {
                    output << player.getName() << "(" << player.getColorCode() << ") ";
                }
                output << std::endl;
                return true;
            }
        }
        
        output << "Error: Please enter 2-4 unique player numbers (e.g.: 13, 234, 1234)." << std::endl;
    }
}

bool GameConfig::isValidPlayerSelection(const std::string& selection) {
    // 检查长度是否在2-4之间
    if (selection.length() < 2 || selection.length() > 4) {
        return false;
    }
    
    // 检查是否都是有效数字（1-4），且不重复
    std::vector<bool> used(5, false); // 索引0不使用，1-4对应角色
    
    for (char c : selection) {
        if (c < '1' || c > '4') {
            return false; // 无效字符
        }
        
        int digit = c - '0';
        if (used[digit]) {
            return false; // 重复
        }
        used[digit] = true;
    }
    
    return true;
}

std::vector<Player::Character> GameConfig::parsePlayerSelection(const std::string& selection) {
    std::vector<Player::Character> characters;
    
    for (char c : selection) {
        int digit = c - '0';
        characters.push_back(static_cast<Player::Character>(digit));
    }
    
    return characters;
}

void GameConfig::displayPlayerSelectionPrompt(std::ostream& output) const {
    output << "Please select 2-4 unique players, enter numbers:" << std::endl;
    output << "1. Qian Furen (Q)" << std::endl;
    output << "2. Aturber (A)" << std::endl;
    output << "3. Sun Xiaomei (S)" << std::endl;
    output << "4. Jin Beibei (J)" << std::endl;
    output << "Please enter (e.g.: 13, 234, 1234): ";
}

} // namespace richgame