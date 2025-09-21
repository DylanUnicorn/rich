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
#include "land.h"
#include "Structure.h"
#include "TollFee.h"
#include "Tool.h"


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
            } 
            else if (strcmp(cmd, "help") == 0) {
                ui_display_help();
            } 
            else if (strcmp(cmd, "query") == 0) {
                // 显示当前玩家资产
                printf("资金：%d，点数：%d，位置：%d\n", 
                    currentPlayer->money, currentPlayer->points, currentPlayer->position);
            } 
            else if (strcmp(cmd, "roll") == 0) {
                Player_use_roll_dice( currentPlayer);
                // 掷骰子逻辑
                printf("（此处掷骰子...）\n");
                 
                int i = find_place(map, currentPlayer);

                if(map[i].type == '0' && map[i].owner== NULL){
                    printf("此处为空地，可以购买。\n");
                    printf("是否购买此地？(y/n): ");
                    char choice;
                    //scanf(" %c", &choice);
                    if (scanf(" %c", &choice) != 1) {
                        // 处理输入错误
                        printf("输入错误，请输入 y 或 n。\n");
                        continue;
                    }
                    while (getchar() != '\n'); // 清除输入缓冲区
                    if (choice == 'y' || choice == 'Y') {
                        buy_land(map + currentPlayer->position, currentPlayer->position, currentPlayer);
                    } 
                    else if(choice == 'n' || choice == 'N') {
                        printf("放弃购买此地。\n");
                    }
                    else {
                        printf("错误指令，输入help寻求帮助。\n");
                    }
                }
                else if(map[i].owner == currentPlayer){
                    printf("此处为你拥有的地产，可以升级或出售。\n");
                    printf("是否升级此地？(u 升级 / s 出售 / n 不操作): ");
                    char choice;
                    //scanf(" %c", &choice);
                    if (scanf(" %c", &choice) != 1) {
                        // 处理输入错误
                        printf("输入错误，请输入 u, s 或 n。\n");
                        continue;
                    }
                    while (getchar() != '\n'); // 清除输入缓冲区
                    if (choice == 'u' || choice == 'U') {
                        upgrade_land(map + currentPlayer->position, currentPlayer->position, currentPlayer);
                    } 
                    else if (choice == 's' || choice == 'S') {
                        sell_land(map + currentPlayer->position, currentPlayer->position, currentPlayer);
                    }
                    else if(choice == 'n' || choice == 'N'){
                        printf("放弃操作此地。\n");
                    }
                    else {
                        printf("错误指令，输入help寻求帮助。\n");
                    }
                }
                else if(map[i].owner != NULL && map[i].owner != currentPlayer){
                    printf("此处为%s的地产，你需要支付过路费。\n", player_getName(map -> owner -> character));
                    GetTollFee(currentPlayer,map + currentPlayer->position,&playerManager);
                }
                else{
                    printf("此处为特殊地块，触发相应事件。\n");
                    //trigger_special_tile_event(map, currentPlayer);
                }
                
                playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
            } 
            else {
                printf("未知命令，请输入 help 查看帮助。\n");
            }
        }
    }
    else {
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