#include "GameConfig.h"

void gameConfig_init(GameConfig* config) {
    if (config == NULL) return;
    config->initialMoney = 10000;
}

bool gameConfig_setInitialMoney(GameConfig* config, int money) {
    if (config == NULL) return false;
    
    if (money >= 1000 && money <= 50000) {
        config->initialMoney = money;
        return true;
    }
    return false;
}

int gameConfig_getInitialMoney(const GameConfig* config) {
    if (config == NULL) return 10000;
    return config->initialMoney;
}

void gameConfig_promptForInitialMoney(GameConfig* config) {
    if (config == NULL) return;
    
    char line[256];
    int money;
    
    while (true) {
        printf("Please enter initial money (1000-50000), default 10000: ");
        
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        
        /* Remove newline character */
        line[strcspn(line, "\n")] = 0;
        
        /* If empty line, use default value */
        if (strlen(line) == 0) {
            config->initialMoney = 10000;
            break;
        }
        
    /* Try to parse as integer (portable: sscanf on non-MSVC, sscanf_s on MSVC) */
    #ifdef _MSC_VER
    if (sscanf_s(line, "%d", &money) == 1) {
    #else
    if (sscanf(line, "%d", &money) == 1) {
    #endif
            if (gameConfig_setInitialMoney(config, money)) {
                break;
            } else {
                printf("Error: Initial money must be between 1000-50000, please re-enter.\n");
            }
        } else {
            printf("Error: Please enter a valid number.\n");
        }
    }
    
    printf("Initial money set to: %d yuan\n", config->initialMoney);
}

bool gameConfig_promptForPlayerSelection(GameConfig* config, PlayerManager* playerManager) {
    if (config == NULL || playerManager == NULL) return false;
    
    char selection[10];
    PlayerCharacter characters[MAX_PLAYERS];
    int characterCount;
    
    playerManager_reset(playerManager);
    
    while (true) {
        gameConfig_displayPlayerSelectionPrompt();
        
        if (fgets(selection, sizeof(selection), stdin) == NULL) {
            return false;
        }
        
        /* Remove newline character */
        selection[strcspn(selection, "\n")] = 0;
        
        if (gameConfig_isValidPlayerSelection(selection)) {
            characterCount = gameConfig_parsePlayerSelection(selection, characters, MAX_PLAYERS);
            
            /* Add all selected players */
            bool allSuccess = true;
            for (int i = 0; i < characterCount; i++) {
                if (!playerManager_addPlayer(playerManager, characters[i], config->initialMoney)) {
                    allSuccess = false;
                    break;
                }
            }
            
            if (allSuccess) {
                printf("Player selection successful!\n");
                for (int i = 0; i < playerManager_getPlayerCount(playerManager); i++) {
                    Player* player = playerManager_getPlayer(playerManager, i);
                    if (player != NULL) {
                        printf("%s(%c) ", player_getName(player->character), 
                               player_getColorCode(player->character));
                    }
                }
                printf("\n");
                return true;
            }
        }
        
        printf("Error: Please enter 2-4 unique player numbers (e.g.: 13, 234, 1234).\n");
    }
}

bool gameConfig_isValidPlayerSelection(const char* selection) {
    if (selection == NULL) return false;
    
    int len = (int)strlen(selection);
    
    /* Check length is between 2-4 */
    if (len < 2 || len > 4) {
        return false;
    }
    
    /* Check if all are valid digits (1-4) and no duplicates */
    bool used[5] = {false}; /* Index 0 not used, 1-4 for characters */
    
    for (int i = 0; i < len; i++) {
        char c = selection[i];
        if (c < '1' || c > '4') {
            return false; /* Invalid character */
        }
        
        int digit = c - '0';
        if (used[digit]) {
            return false; /* Duplicate */
        }
        used[digit] = true;
    }
    
    return true;
}

int gameConfig_parsePlayerSelection(const char* selection, PlayerCharacter* characters, int maxCount) {
    if (selection == NULL || characters == NULL || maxCount <= 0) return 0;
    
    int len = (int)strlen(selection);
    int count = 0;
    
    for (int i = 0; i < len && count < maxCount; i++) {
        int digit = selection[i] - '0';
        characters[count] = (PlayerCharacter)digit;
        count++;
    }
    
    return count;
}

void gameConfig_displayPlayerSelectionPrompt(void) {
    printf("Please select 2-4 unique players, enter numbers:\n");
    printf("1. Qian Furen (Q)\n");
    printf("2. Aturber (A)\n");
    printf("3. Sun Xiaomei (S)\n");
    printf("4. Jin Beibei (J)\n");
    printf("Please enter (e.g.: 13, 234, 1234): ");
}