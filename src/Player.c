#include "Player.h"

/* Player functions */

void player_init(Player* player, PlayerCharacter character, int initialMoney) {
    if (player == NULL) return;
    
    player->character = character;
    player->money = initialMoney;
    player->points = 0;
    player->position = 0;
}

const char* player_getName(PlayerCharacter character) {
    switch (character) {
        case QIAN_FUREN:
            return "钱夫人";
        case ATURBER:
            return "阿土伯";
        case SUN_XIAOMEI:
            return "孙小美";
        case JIN_BEIBEI:
            return "金贝贝";
        default:
            return "Unknown";
    }
}

char player_getColorCode(PlayerCharacter character) {
    switch (character) {
        case QIAN_FUREN:
            return 'Q';
        case ATURBER:
            return 'A';
        case SUN_XIAOMEI:
            return 'S';
        case JIN_BEIBEI:
            return 'J';
        default:
            return '?';
    }
}

int player_getMoney(const Player* player) {
    if (player == NULL) return 0;
    return player->money;
}

void player_setMoney(Player* player, int money) {
    if (player == NULL) return;
    player->money = money;
}

int player_getPoints(const Player* player) {
    if (player == NULL) return 0;
    return player->points;
}

void player_setPoints(Player* player, int points) {
    if (player == NULL) return;
    player->points = points;
}

int player_getPosition(const Player* player) {
    if (player == NULL) return 0;
    return player->position;
}

void player_setPosition(Player* player, int position) {
    if (player == NULL) return;
    player->position = position;
}

bool player_isBankrupt(const Player* player) {
    if (player == NULL) return false;
    return player->money < 0;
}

/* PlayerManager functions */

void playerManager_init(PlayerManager* manager) {
    if (manager == NULL) return;
    
    manager->playerCount = 0;
    manager->currentPlayerIndex = 0;
    
    /* Initialize all players to empty state */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        player_init(&manager->players[i], QIAN_FUREN, 0);
    }
}

bool playerManager_addPlayer(PlayerManager* manager, PlayerCharacter character, int initialMoney) {
    if (manager == NULL) return false;
    
    /* Check if already have maximum players */
    if (manager->playerCount >= MAX_PLAYERS) {
        return false;
    }
    
    /* Check for duplicate character */
    for (int i = 0; i < manager->playerCount; i++) {
        if (manager->players[i].character == character) {
            return false;
        }
    }
    
    /* Add new player */
    player_init(&manager->players[manager->playerCount], character, initialMoney);
    manager->playerCount++;
    
    return true;
}

int playerManager_getPlayerCount(const PlayerManager* manager) {
    if (manager == NULL) return 0;
    return manager->playerCount;
}

Player* playerManager_getCurrentPlayer(PlayerManager* manager) {
    if (manager == NULL || manager->playerCount == 0) return NULL;
    if (manager->currentPlayerIndex >= manager->playerCount) return NULL;
    
    return &manager->players[manager->currentPlayerIndex];
}

int playerManager_getCurrentPlayerIndex(const PlayerManager* manager) {
    if (manager == NULL) return 0;
    return manager->currentPlayerIndex;
}

void playerManager_nextPlayer(PlayerManager* manager) {
    if (manager == NULL || manager->playerCount == 0) return;
    
    manager->currentPlayerIndex = (manager->currentPlayerIndex + 1) % manager->playerCount;
}

void playerManager_removeBankruptPlayer(PlayerManager* manager, int index) {
    if (manager == NULL || index < 0 || index >= manager->playerCount) return;
    
    /* Shift all players after the removed one */
    for (int i = index; i < manager->playerCount - 1; i++) {
        manager->players[i] = manager->players[i + 1];
    }
    
    manager->playerCount--;
    
    /* Adjust current player index */
    if (manager->currentPlayerIndex >= manager->playerCount && manager->playerCount > 0) {
        manager->currentPlayerIndex = 0;
    }
    if (manager->currentPlayerIndex > index && manager->currentPlayerIndex > 0) {
        manager->currentPlayerIndex--;
    }
}

void playerManager_reset(PlayerManager* manager) {
    if (manager == NULL) return;
    
    manager->playerCount = 0;
    manager->currentPlayerIndex = 0;
}

Player* playerManager_getPlayer(PlayerManager* manager, int index) {
    if (manager == NULL || index < 0 || index >= manager->playerCount) return NULL;
    
    return &manager->players[index];
}

bool playerManager_isGameWon(const PlayerManager* manager) {
    if (manager == NULL) return false;   
    // 如果只剩下1个玩家，游戏胜利
    return manager->playerCount == 1;
}

Player* playerManager_getWinner(PlayerManager* manager) {
    if (manager == NULL || manager->playerCount != 1) return NULL;
    
    // 返回最后剩下的玩家
    return &manager->players[0];
}
