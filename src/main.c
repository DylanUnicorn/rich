#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif
#include "GameConfig.h"
#include "Player.h"
#include "ui.h"
#include "roll.h"
#include "prophouse.h"
#include "map.h"


void run_test_helloworld() {
    printf("Hello World!\n");
}

void run_command_mode() {
    char line[256];
    
    while (fgets(line, sizeof(line), stdin)) {
        /* Remove newline character */
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) == 0) continue;
        
        if (strcmp(line, "testhelloworld") == 0) {
            printf("Hello World!\n");
        } else if (strcmp(line, "dump") == 0) {
            /* For now, just output a simple dump */
            printf("system dump complete\n");
            break;  /* Exit after dump */
        } else {
            printf("Unknown command: %s\n", line);
        }
    }
}

void run_interactive_game() {
    printf("=== Rich Game ===\n");
    Structure map[HEIGHT * WIDTH];
    init_map(map);
    GameConfig config;
    PlayerManager playerManager;
    
    /* Initialize structures */
    gameConfig_init(&config);
    playerManager_init(&playerManager);
    
    /* Set initial money */
    gameConfig_promptForInitialMoney(&config);
    
    /* Select players */
    if (gameConfig_promptForPlayerSelection(&config, &playerManager)) {
        printf("\nGame initialization completed!\n");
        printf("Participating players:\n");
        
        int playerCount = playerManager_getPlayerCount(&playerManager);
        for (int i = 0; i < playerCount; i++) {
            Player* player = playerManager_getPlayer(&playerManager, i);
            if (player != NULL) {
                printf("%d. %s(%c) - Money: %d yuan\n", 
                       i + 1, 
                       player_getName(player->character),
                       player_getColorCode(player->character),
                       player_getMoney(player));
            }
        }
        
        Player* currentPlayer = playerManager_getCurrentPlayer(&playerManager);
        if (currentPlayer != NULL) {
            printf("\nCurrent turn: %s\n", player_getName(currentPlayer->character));
        }

         // 游戏主循环
        while (1) {
            
            print_map(map);
            Player* currentPlayer = playerManager_getCurrentPlayer(&playerManager);
            if (currentPlayer == NULL) break;

            // 显示带颜色的提示符
            ui_display_prompt(currentPlayer);

            // 读取并处理命令
            char cmd[128];
            if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;
            ui_clear_screen();
            cmd[strcspn(cmd, "\n")] = 0;

            // 这里处理命令，比如 roll, query, help, quit 等
            if (strcmp(cmd, "quit") == 0) {
                printf("游戏结束。\n");
                break;
            } else if (strcmp(cmd, "help") == 0) {
                ui_display_help();
            } else if (strcmp(cmd, "query") == 0) {
                // 显示当前玩家资产
                printf("资金：%d，点数：%d，位置：%d\n", 
                    currentPlayer->money, currentPlayer->points, currentPlayer->position);
            } else if (strcmp(cmd, "roll") == 0) {
                Player_use_roll_dice( currentPlayer);
                // 掷骰子逻辑
                printf("（此处掷骰子...）\n");
                // playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
            } else {
                printf("未知命令，请输入 help 查看帮助。\n");
            }
        }

    } else {
        printf("Game initialization failed!\n");
    }
}

int main(int argc, char* argv[]) {
    /* Check command line arguments */
    if (argc > 1) {
        if (strcmp(argv[1], "testhelloworld") == 0) {
            run_test_helloworld();
            return 0;
        } else {
            printf("Unknown command: %s\n", argv[1]);
            printf("Available commands:\n");
            printf("  testhelloworld - Run hello world test\n");
            printf("  (no arguments) - Run interactive game or command mode\n");
            return 1;
        }
    }
    
    /* Check if input is redirected (for automated testing) */
    if (!isatty(fileno(stdin))) {
        run_command_mode();
    } else {
        run_interactive_game();
    }
    
    return 0;
}

// int main() {
//     GameConfig config;
//     PlayerManager playerManager;
//     Game_Init(&config,&playerManager);
//     while (1)
//     {
//         for (int i = 0;i<playerManager.playerCount;i++)
//         {
//             printf("\033[2J\033[H");
//             printf("%s",player_getName(playerManager.players[i].character));
//             getchar();
//         }
        
//     }
// }