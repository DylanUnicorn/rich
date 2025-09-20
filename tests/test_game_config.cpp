#include <gtest/gtest.h>
#include <sstream>
#include "GameConfig.h"

using namespace richgame;

class GameConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_unique<GameConfig>();
    }

    std::unique_ptr<GameConfig> config;
};

TEST_F(GameConfigTest, DefaultInitialMoney) {
    EXPECT_EQ(config->getInitialMoney(), 10000);
}

TEST_F(GameConfigTest, SetValidInitialMoney) {
    EXPECT_TRUE(config->setInitialMoney(5000));
    EXPECT_EQ(config->getInitialMoney(), 5000);
    
    EXPECT_TRUE(config->setInitialMoney(1000));
    EXPECT_EQ(config->getInitialMoney(), 1000);
    
    EXPECT_TRUE(config->setInitialMoney(50000));
    EXPECT_EQ(config->getInitialMoney(), 50000);
}

TEST_F(GameConfigTest, SetInvalidInitialMoney) {
    EXPECT_FALSE(config->setInitialMoney(999));
    EXPECT_EQ(config->getInitialMoney(), 10000); // 应该保持原值
    
    EXPECT_FALSE(config->setInitialMoney(50001));
    EXPECT_EQ(config->getInitialMoney(), 10000); // 应该保持原值
    
    EXPECT_FALSE(config->setInitialMoney(-1000));
    EXPECT_EQ(config->getInitialMoney(), 10000); // 应该保持原值
}

TEST_F(GameConfigTest, PromptForInitialMoneyWithDefault) {
    std::istringstream input("\n"); // 空行，使用默认值
    std::ostringstream output;
    
    config->promptForInitialMoney(input, output);
    
    EXPECT_EQ(config->getInitialMoney(), 10000);
    
    std::string outputStr = output.str();
    EXPECT_TRUE(outputStr.find("Please enter initial money") != std::string::npos);
    EXPECT_TRUE(outputStr.find("Initial money set to: 10000 yuan") != std::string::npos);
}

TEST_F(GameConfigTest, PromptForInitialMoneyWithValidInput) {
    std::istringstream input("15000\n");
    std::ostringstream output;
    
    config->promptForInitialMoney(input, output);
    
    EXPECT_EQ(config->getInitialMoney(), 15000);
    
    std::string outputStr = output.str();
    EXPECT_TRUE(outputStr.find("Initial money set to: 15000 yuan") != std::string::npos);
}

TEST_F(GameConfigTest, PromptForInitialMoneyWithInvalidThenValid) {
    std::istringstream input("100\n60000\n20000\n");
    std::ostringstream output;
    
    config->promptForInitialMoney(input, output);
    
    EXPECT_EQ(config->getInitialMoney(), 20000);
    
    std::string outputStr = output.str();
    EXPECT_TRUE(outputStr.find("Error: Initial money must be between 1000-50000") != std::string::npos);
    EXPECT_TRUE(outputStr.find("Initial money set to: 20000 yuan") != std::string::npos);
}

TEST(GameConfigValidationTest, ValidPlayerSelection) {
    EXPECT_TRUE(GameConfig::isValidPlayerSelection("12"));
    EXPECT_TRUE(GameConfig::isValidPlayerSelection("13"));
    EXPECT_TRUE(GameConfig::isValidPlayerSelection("1234"));
    EXPECT_TRUE(GameConfig::isValidPlayerSelection("24"));
    EXPECT_TRUE(GameConfig::isValidPlayerSelection("134"));
}

TEST(GameConfigValidationTest, InvalidPlayerSelection) {
    // 长度不对
    EXPECT_FALSE(GameConfig::isValidPlayerSelection("1"));
    EXPECT_FALSE(GameConfig::isValidPlayerSelection("12345"));
    
    // 包含无效字符
    EXPECT_FALSE(GameConfig::isValidPlayerSelection("15"));
    EXPECT_FALSE(GameConfig::isValidPlayerSelection("10"));
    EXPECT_FALSE(GameConfig::isValidPlayerSelection("1a"));
    
    // 重复字符
    EXPECT_FALSE(GameConfig::isValidPlayerSelection("11"));
    EXPECT_FALSE(GameConfig::isValidPlayerSelection("112"));
    EXPECT_FALSE(GameConfig::isValidPlayerSelection("1223"));
}

TEST(GameConfigParsingTest, ParsePlayerSelection) {
    auto result = GameConfig::parsePlayerSelection("13");
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], Player::Character::QIAN_FUREN);
    EXPECT_EQ(result[1], Player::Character::SUN_XIAOMEI);
    
    auto result2 = GameConfig::parsePlayerSelection("1234");
    EXPECT_EQ(result2.size(), 4);
    EXPECT_EQ(result2[0], Player::Character::QIAN_FUREN);
    EXPECT_EQ(result2[1], Player::Character::ATURBER);
    EXPECT_EQ(result2[2], Player::Character::SUN_XIAOMEI);
    EXPECT_EQ(result2[3], Player::Character::JIN_BEIBEI);
}

TEST_F(GameConfigTest, PromptForPlayerSelectionValid) {
    std::istringstream input("13\n");
    std::ostringstream output;
    
    PlayerManager manager;
    bool result = config->promptForPlayerSelection(manager, input, output);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(manager.getPlayerCount(), 2);
    
    const auto& players = manager.getPlayers();
    EXPECT_EQ(players[0].getCharacter(), Player::Character::QIAN_FUREN);
    EXPECT_EQ(players[1].getCharacter(), Player::Character::SUN_XIAOMEI);
}

TEST_F(GameConfigTest, PromptForPlayerSelectionInvalidThenValid) {
    std::istringstream input("1\n15\n12\n");
    std::ostringstream output;
    
    PlayerManager manager;
    bool result = config->promptForPlayerSelection(manager, input, output);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(manager.getPlayerCount(), 2);
    
    std::string outputStr = output.str();
    EXPECT_TRUE(outputStr.find("Error: Please enter 2-4 unique player numbers") != std::string::npos);
}