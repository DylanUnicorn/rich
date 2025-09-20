#include "Player.h"
#include <stdexcept>

namespace richgame {

Player::Player(Character character, int initialMoney)
    : character_(character), 
      money_(initialMoney), 
      points_(0), 
      position_(0) {
}

std::string Player::getName() const {
    switch (character_) {
        case Character::QIAN_FUREN:
            return "Qian Furen";
        case Character::ATURBER:
            return "Aturber";
        case Character::SUN_XIAOMEI:
            return "Sun Xiaomei";
        case Character::JIN_BEIBEI:
            return "Jin Beibei";
        default:
            return "Unknown";
    }
}

char Player::getColorCode() const {
    switch (character_) {
        case Character::QIAN_FUREN:
            return 'Q';
        case Character::ATURBER:
            return 'A';
        case Character::SUN_XIAOMEI:
            return 'S';
        case Character::JIN_BEIBEI:
            return 'J';
        default:
            return '?';
    }
}

Player::Character Player::getCharacter() const {
    return character_;
}

int Player::getMoney() const {
    return money_;
}

void Player::setMoney(int money) {
    money_ = money;
}

int Player::getPoints() const {
    return points_;
}

void Player::setPoints(int points) {
    points_ = points;
}

int Player::getPosition() const {
    return position_;
}

void Player::setPosition(int position) {
    position_ = position;
}

bool Player::isBankrupt() const {
    return money_ < 0;
}

// PlayerManager 实现

PlayerManager::PlayerManager() : currentPlayerIndex_(0) {
}

bool PlayerManager::addPlayer(Player::Character character, int initialMoney) {
    // 检查是否已经有4个玩家
    if (players_.size() >= 4) {
        return false;
    }
    
    // 检查是否重复选择角色
    for (const auto& player : players_) {
        if (player.getCharacter() == character) {
            return false;
        }
    }
    
    players_.emplace_back(character, initialMoney);
    return true;
}

const std::vector<Player>& PlayerManager::getPlayers() const {
    return players_;
}

Player& PlayerManager::getCurrentPlayer() {
    if (players_.empty()) {
        throw std::runtime_error("No players available");
    }
    return players_[currentPlayerIndex_];
}

int PlayerManager::getCurrentPlayerIndex() const {
    return currentPlayerIndex_;
}

void PlayerManager::nextPlayer() {
    if (!players_.empty()) {
        currentPlayerIndex_ = (currentPlayerIndex_ + 1) % players_.size();
    }
}

void PlayerManager::removeBankruptPlayer(int index) {
    if (index >= 0 && index < static_cast<int>(players_.size())) {
        players_.erase(players_.begin() + index);
        
        // 调整当前玩家索引
        if (currentPlayerIndex_ >= static_cast<int>(players_.size())) {
            currentPlayerIndex_ = 0;
        }
        if (currentPlayerIndex_ > index) {
            currentPlayerIndex_--;
        }
    }
}

size_t PlayerManager::getPlayerCount() const {
    return players_.size();
}

void PlayerManager::reset() {
    players_.clear();
    currentPlayerIndex_ = 0;
}

} // namespace richgame