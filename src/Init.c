#include "Init.h"
#include "GameConfig.h"
#include "Player.h"
int Game_Init(GameConfig *config,PlayerManager *playerManager){
    printf("=== Rich Game ===\n");
    
    /* Initialize structures */
    gameConfig_init(config);
    playerManager_init(playerManager);
    
    /* Set initial money */
    gameConfig_promptForInitialMoney(config);
    
    /* Select players */
    if (gameConfig_promptForPlayerSelection(config, playerManager)) {
        printf("\nGame initialization completed!\n");
        printf("Participating players:\n");
        
        int playerCount = playerManager_getPlayerCount(playerManager);
        for (int i = 0; i < playerCount; i++) {
            Player* player = playerManager_getPlayer(playerManager, i);
            if (player != NULL) {
                printf("%d. %s(%c) - Money: %d yuan\n", 
                       i + 1, 
                       player_getName(player->character),
                       player_getColorCode(player->character),
                       player_getMoney(player));
            }
        }
        
        Player* currentPlayer = playerManager_getCurrentPlayer(playerManager);
        if (currentPlayer != NULL) {
            printf("\nCurrent turn: %s\n", player_getName(currentPlayer->character));
        }
    } else {
        printf("Game initialization failed!\n");
        return 1;
    }
    return 0;
}