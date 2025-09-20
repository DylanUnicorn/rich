#include <gtest/gtest.h>
#include <sstream>
#include "Player.h"

using namespace richgame;

class PlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        player = std::make_unique<Player>(Player::Character::QIAN_FUREN, 10000);
    }

    std::unique_ptr<Player> player;
};

TEST_F(PlayerTest, PlayerInitialization) {
    EXPECT_EQ(player->getName(), "Qian Furen");
    EXPECT_EQ(player->getColorCode(), 'Q');
    EXPECT_EQ(player->getCharacter(), Player::Character::QIAN_FUREN);
    EXPECT_EQ(player->getMoney(), 10000);
    EXPECT_EQ(player->getPoints(), 0);
    EXPECT_EQ(player->getPosition(), 0);
    EXPECT_FALSE(player->isBankrupt());
}

TEST_F(PlayerTest, MoneyManagement) {
    player->setMoney(5000);
    EXPECT_EQ(player->getMoney(), 5000);
    EXPECT_FALSE(player->isBankrupt());
    
    player->setMoney(-100);
    EXPECT_EQ(player->getMoney(), -100);
    EXPECT_TRUE(player->isBankrupt());
}

TEST_F(PlayerTest, PointsManagement) {
    player->setPoints(150);
    EXPECT_EQ(player->getPoints(), 150);
}

TEST_F(PlayerTest, PositionManagement) {
    player->setPosition(5);
    EXPECT_EQ(player->getPosition(), 5);
}

TEST(PlayerCharacterTest, AllCharacters) {
    Player qian(Player::Character::QIAN_FUREN, 10000);
    EXPECT_EQ(qian.getName(), "Qian Furen");
    EXPECT_EQ(qian.getColorCode(), 'Q');
    
    Player atu(Player::Character::ATURBER, 10000);
    EXPECT_EQ(atu.getName(), "Aturber");
    EXPECT_EQ(atu.getColorCode(), 'A');
    
    Player sun(Player::Character::SUN_XIAOMEI, 10000);
    EXPECT_EQ(sun.getName(), "Sun Xiaomei");
    EXPECT_EQ(sun.getColorCode(), 'S');
    
    Player jin(Player::Character::JIN_BEIBEI, 10000);
    EXPECT_EQ(jin.getName(), "Jin Beibei");
    EXPECT_EQ(jin.getColorCode(), 'J');
}