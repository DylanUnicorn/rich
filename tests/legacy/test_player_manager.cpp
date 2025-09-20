#include <gtest/gtest.h>
#include "Player.h"

using namespace richgame;

class PlayerManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<PlayerManager>();
    }

    std::unique_ptr<PlayerManager> manager;
};

TEST_F(PlayerManagerTest, InitialState) {
    EXPECT_EQ(manager->getPlayerCount(), 0);
}

TEST_F(PlayerManagerTest, AddPlayers) {
    EXPECT_TRUE(manager->addPlayer(Player::Character::QIAN_FUREN, 10000));
    EXPECT_EQ(manager->getPlayerCount(), 1);
    
    EXPECT_TRUE(manager->addPlayer(Player::Character::ATURBER, 10000));
    EXPECT_EQ(manager->getPlayerCount(), 2);
    
    EXPECT_TRUE(manager->addPlayer(Player::Character::SUN_XIAOMEI, 10000));
    EXPECT_EQ(manager->getPlayerCount(), 3);
    
    EXPECT_TRUE(manager->addPlayer(Player::Character::JIN_BEIBEI, 10000));
    EXPECT_EQ(manager->getPlayerCount(), 4);
}

TEST_F(PlayerManagerTest, PreventDuplicateCharacters) {
    EXPECT_TRUE(manager->addPlayer(Player::Character::QIAN_FUREN, 10000));
    EXPECT_FALSE(manager->addPlayer(Player::Character::QIAN_FUREN, 10000));
    EXPECT_EQ(manager->getPlayerCount(), 1);
}

TEST_F(PlayerManagerTest, MaxPlayersLimit) {
    manager->addPlayer(Player::Character::QIAN_FUREN, 10000);
    manager->addPlayer(Player::Character::ATURBER, 10000);
    manager->addPlayer(Player::Character::SUN_XIAOMEI, 10000);
    manager->addPlayer(Player::Character::JIN_BEIBEI, 10000);
    
    // 尝试添加第5个玩家应该失败
    EXPECT_FALSE(manager->addPlayer(Player::Character::QIAN_FUREN, 15000));
    EXPECT_EQ(manager->getPlayerCount(), 4);
}

TEST_F(PlayerManagerTest, PlayerTurns) {
    manager->addPlayer(Player::Character::QIAN_FUREN, 10000);
    manager->addPlayer(Player::Character::ATURBER, 10000);
    
    EXPECT_EQ(manager->getCurrentPlayerIndex(), 0);
    EXPECT_EQ(manager->getCurrentPlayer().getCharacter(), Player::Character::QIAN_FUREN);
    
    manager->nextPlayer();
    EXPECT_EQ(manager->getCurrentPlayerIndex(), 1);
    EXPECT_EQ(manager->getCurrentPlayer().getCharacter(), Player::Character::ATURBER);
    
    manager->nextPlayer();
    EXPECT_EQ(manager->getCurrentPlayerIndex(), 0);
    EXPECT_EQ(manager->getCurrentPlayer().getCharacter(), Player::Character::QIAN_FUREN);
}

TEST_F(PlayerManagerTest, RemoveBankruptPlayer) {
    manager->addPlayer(Player::Character::QIAN_FUREN, 10000);
    manager->addPlayer(Player::Character::ATURBER, 10000);
    manager->addPlayer(Player::Character::SUN_XIAOMEI, 10000);
    
    EXPECT_EQ(manager->getPlayerCount(), 3);
    
    // 移除中间的玩家
    manager->removeBankruptPlayer(1);
    EXPECT_EQ(manager->getPlayerCount(), 2);
    
    // 验证剩余玩家
    const auto& players = manager->getPlayers();
    EXPECT_EQ(players[0].getCharacter(), Player::Character::QIAN_FUREN);
    EXPECT_EQ(players[1].getCharacter(), Player::Character::SUN_XIAOMEI);
}

TEST_F(PlayerManagerTest, Reset) {
    manager->addPlayer(Player::Character::QIAN_FUREN, 10000);
    manager->addPlayer(Player::Character::ATURBER, 10000);
    
    EXPECT_EQ(manager->getPlayerCount(), 2);
    
    manager->reset();
    EXPECT_EQ(manager->getPlayerCount(), 0);
    EXPECT_EQ(manager->getCurrentPlayerIndex(), 0);
}