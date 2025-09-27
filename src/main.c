#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif
#include "GameConfig.h"
#include "GameLogic.h"
#include "Player.h"
#include "ui.h"
#include "roll.h"
#include "prophouse.h"
#include "map.h"
#include "land.h"
#include "Structure.h"
#include "TollFee.h"
#include "Tool.h"
#include "HospitalAndPrison.h"
#include <sys/stat.h>

/* Forward declarations for test mode */
static int path_is_dir(const char* p);
static void json_escape_string(FILE* f, const char* s);
static char char_from_player(PlayerCharacter pc);
static void write_dump_json(const char* case_dir, Structure* map, PlayerManager* pm);
static void load_preset(const char* json, Structure* map, PlayerManager* pm);
static char* read_file_all(const char* path, long* out_len);

#define MAX_INPUT 100

void run_test_helloworld() {
    printf("Hello World!\n");
}

void run_game_loop(int is_test_mode, const char* case_dir) {
    char input[MAX_INPUT];
    Structure map[HEIGHT * WIDTH];
    init_map(map);
    GameConfig config;
    PlayerManager playerManager;
    
    /* Initialize structures */
    gameConfig_init(&config);
    playerManager_init(&playerManager);

    int use_preset = 0;
    int round_count = 1; // 记录完整回合数
    int game_turns = 10; // 直接作为冷却计数，0时可刷新财神
    bool god_used = false;//标志财神是否被使用
    int god_turn = 0;//记录财神出现的回合数
    int god_pos = -1;//记录财神位置 且god_pos = -1表示未出现

    if (is_test_mode && case_dir) {
        char preset_path[1024];
        snprintf(preset_path, sizeof(preset_path), "%s/%s", case_dir, "preset.json");
        long preset_len = 0;
        char* preset_json = read_file_all(preset_path, &preset_len);
        if (preset_json) {
            use_preset = 1;
            load_preset(preset_json, map, &playerManager);
            free(preset_json);
        }
    }

    if (!use_preset) {
        /* Set initial money */
        gameConfig_promptForInitialMoney(&config);
        
        /* Select players */
        if (!gameConfig_promptForPlayerSelection(&config, &playerManager)) {
            printf("游戏初始化失败！\n");
            return;
        }
    }
    
    if (!is_test_mode) {
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
    }

    // 游戏主循环
    while (1) {
        
        Player* currentPlayer = playerManager_getCurrentPlayer(&playerManager);
        if (currentPlayer == NULL) break;
        
        if (!is_test_mode) {
            print_map(map, &playerManager);
            ui_display_prompt(currentPlayer);
        }

        // if (currentPlayer->in_hospital) {
        //     if (!is_test_mode) printf("你在医院，无法进行其他操作，回车结束回合。\n");
        //     HospitalDayReduceOne(currentPlayer);
        //     playerManager_nextPlayer(&playerManager);
        //     // In both modes, consume a line of input to simulate "press enter"
        //     if (fgets(input, MAX_INPUT, stdin) == NULL) break;
        //     if (!is_test_mode) ui_clear_screen();
        //     continue;
        // }
        // if (currentPlayer->in_prison) {
        //     if (!is_test_mode) printf("你在监狱，无法进行其他操作，回车结束回合。\n");
        //     PrisonDayReduceOne(currentPlayer);
        //     playerManager_nextPlayer(&playerManager);
        //     if (fgets(input, MAX_INPUT, stdin) == NULL) break;
        //     if (!is_test_mode) ui_clear_screen();
        //     continue;
        // }

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            if (is_test_mode) write_dump_json(case_dir, map, &playerManager);
            break;
        }
        if (!is_test_mode) ui_clear_screen();
        
        input[strcspn(input, "\n")] = 0;

        char *cmd = strtok(input, " ");
        char *param = strtok(NULL, " ");

        if (cmd == NULL || strlen(cmd) == 0) continue;
        
        if (strcmp(cmd, "quit") == 0) {
            if (!is_test_mode) printf("游戏结束。\n");
            break;
        }
        if (is_test_mode && strcmp(cmd, "dump") == 0) {
            write_dump_json(case_dir, map, &playerManager);
            break;
        }
        else if (strcmp(cmd, "step") == 0) {
            if (param != NULL) {
                int steps = atoi(param);
                int turn_advanced = 0;
                //如果接下来遇见炸弹就被送往医院
                // for(int i = 0; i <= steps; i++){
                //     int nextPos = (currentPlayer->position + i) % 70;
                //     int j = find_place(map, nextPos);
                //     if(map[j].type == '@'){
                //         printf("你遇见了炸弹，被送往医院！\n");
                //         InHospital(currentPlayer);
                //         map[j].type = '0'; // Consume bomb
                //         playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                //         turn_advanced = 1;
                //         break;
                //     }
                //     else if(map[j].type == '#'){
                //         printf("你遇见了路障，停止前进！\n");
                //         currentPlayer->position = (currentPlayer->position + i) % 70;
                //         playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                //         turn_advanced = 1;
                //         break;
                //     }
                // }
                // if (turn_advanced) continue;
                for(int i = 0; i <= steps; i++){
                    int nextPos = (currentPlayer->position + i) % 70;
                    int j = find_place(map, nextPos);
                    if(map[j].type == 'F'){
                        printf("你遇见了天降财神，你太幸运啦！\n");
                        map[j].type = '0'; // Consume bomb
                        break;
                    }
                }
                currentPlayer->position = (currentPlayer->position + steps) % 70; // 假设地图有70个位置
                if(currentPlayer->position < 0) currentPlayer->position += 70;  
                printf("你移动到了位置 %d\n", currentPlayer->position);

                int i = find_place(map, currentPlayer->position);

                if(map[i].type == '0' && map[i].owner== NULL){
                    printf("此处为空地，可以购买。\n");
                    printf("是否购买此地？(y/n): ");
                    char choice_buf[MAX_INPUT];
                    if (fgets(choice_buf, sizeof(choice_buf), stdin) == NULL) break;
                    char choice = choice_buf[0];
                    if (choice == 'y' || choice == 'Y') {
                        buy_land(map + i, map[i].id, currentPlayer);
                    } 
                    else if(choice == 'n' || choice == 'N') {
                        printf("放弃购买此地。\n");
                    }
                    else {
                        printf("错误指令，输入help寻求帮助。\n");
                    }
                }
                // else if(map[i].type == 'P'){
                //     printf("你遇见了监狱，停止前进！\n");
                //     InPrison(currentPlayer, map[i]);
                //     playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                //     turn_advanced = 1;
                // }
                else if(map[i].owner == currentPlayer){
                    printf("此处为你拥有的地产，可以升级或出售。\n");
                    printf("是否升级此地？(u 升级 / n 不操作): ");
                    char choice_buf[MAX_INPUT];
                    if (fgets(choice_buf, sizeof(choice_buf), stdin) == NULL) break;
                    char choice = choice_buf[0];
                    if (choice == 'u' || choice == 'U') {
                        upgrade_land(map + i, map[i].id, currentPlayer);
                    } 
                    else if(choice == 'n' || choice == 'N'){
                        printf("放弃操作此地。\n");
                    }
                    else {
                        printf("错误指令，输入help寻求帮助。\n");
                    }
                }
                else if(map[i].owner != NULL && map[i].owner != currentPlayer){
                    printf("此处为%s的地产，", player_getName(map[i].owner->character));
                    GetTollFee(currentPlayer,&map[i]);
                    if(player_isBankrupt(currentPlayer)){
                    IBankruptcy(currentPlayer,&playerManager,map);
                    }
                    if (playerManager_isGameWon(&playerManager)) {
                        Player* winner = playerManager_getWinner(&playerManager);
                        if (winner != NULL) {
                            printf("游戏结束！%s 获胜！\n", player_getName(winner->character));
                        }
                        break;
                    }
                    else{
                        printf("游戏继续。\n");
                    }
                }
                else{
                    game_handle_cell_event(currentPlayer, &map[i], &playerManager);
                }
                if (!turn_advanced) {
                    playerManager_nextPlayer(&playerManager);
                    if (playerManager.currentPlayerIndex == 0) {
                        round_count++;
                        // 财神冷却机制
                        if (god_pos == -1 && game_turns > 0) {
                            game_turns--;
                            printf("[DEBUG] 财神冷却中，剩余 %d 回合\n", game_turns);
                        }
                        game_handle_turn(&god_pos, &god_turn, &god_used, &game_turns, map, &playerManager);
                        printf("当前到达 %d 回合\n", round_count);
                    }
                }
                continue;
            } else {
                printf("错误: step需要指定步数\n");
            }
        } 
        else if (strcmp(cmd, "help") == 0) {
            ui_display_help();
        }
        else if (strcmp(cmd, "block") == 0) {
            if (param != NULL) {
                int offset = atoi(param);
                if(offset >= -10 && offset <= 10 && offset != 0){
                    if(currentPlayer->tool.roadblock > 0){
                        int target_pos = (currentPlayer->position + offset + 70) % 70;
                        int i = find_place(map, target_pos);
                        bool owner_on_land = false;
                        
                        for(int p = 0; p < playerManager.playerCount; p++){
                            Player* player = &playerManager.players[p];
                            if(player->position == target_pos && player->bankruptcy == false){
                                printf("该位置有玩家，无法放置路障。\n");
                                owner_on_land = true;
                            }
                        }
                        if(map[i].type == '@'){
                            printf("该位置已有炸弹，无法放置路障。\n");
                            continue;
                        }
                        else if(map[i].type == '#'){
                            printf("该位置已有路障，无法重复放置。\n");
                            continue;
                        }
                        else if(owner_on_land == true){
                            owner_on_land = false;
                            continue;
                        }    
                        map[i].type = '#'; // 设置为路障
                        currentPlayer->tool.roadblock--;
                        currentPlayer->tool.total--;
                        
                    }
                    else{
                        printf("你没有路障道具，无法使用。\n");
                    }
                }
                else{
                    printf("错误: 指令block位置参数应在-10到10之间且不为0\n");
                }
            }
            else {
                printf("错误: 指令block需要指定位置\n");
            }
        }
        // else if (strcmp(cmd, "bomb") == 0) {
        //     if (param != NULL) {
        //         int offset = atoi(param);
        //         if(offset >= -10 && offset <= 10 && offset != 0){
        //             if(currentPlayer->tool.bomb > 0){
        //                 int target_pos = (currentPlayer->position + offset + 70) % 70;
        //                 int i = find_place(map, target_pos);
        //                 bool owner_on_land = false;
        //                 for(int p = 0; p < playerManager.playerCount; p++){
        //                     Player* player = &playerManager.players[p];
        //                     if(player->position == target_pos && player->bankruptcy == false){
        //                         printf("该位置有玩家，无法放置炸弹。\n");
        //                         owner_on_land = true;
        //                     }
        //                 }
        //                 if(map[i].type == '@'){
        //                     printf("该位置已有炸弹，无法重复放置。\n");
        //                     continue;
        //                 }
        //                 else if(map[i].type == '#'){
        //                     printf("该位置已有路障，无法放置炸弹。\n");
        //                     continue;
        //                 }
        //                 else if(owner_on_land == true){
        //                     owner_on_land = false;
        //                     continue;
        //                 }
        //                 currentPlayer->tool.bomb--;
        //                 currentPlayer->tool.total--;              
        //                 map[i].type = '@'; // 设置为炸弹
        //             }
        //             else{
        //                 printf("你没有炸弹道具，无法使用。\n");
        //             }
        //         }
        //         else{
        //             printf("错误: 指令bomb位置参数应在-10到10之间且不为0\n");
        //         }
        //     }
        //     else {
        //         printf("错误: 指令bomb需要指定位置\n");
        //     }
        // }
        else if (strcmp(cmd, "robot") == 0) {
            if (currentPlayer->tool.doll > 0) {
                currentPlayer->tool.doll--;
                currentPlayer->tool.total--;
                for(int i = 1; i <= 10; i++){
                    int clear_pos = (currentPlayer->position + i) % 70;
                    int map_idx = find_place(map, clear_pos);
                    if (map[map_idx].type == '#' || map[map_idx].type == '@') {
                        map[map_idx].type = '0';
                    }
                }
            }
            else {
                printf("你没有机器人道具，无法使用。\n");
            }
        }
        else if (strcmp(cmd, "query") == 0) {
            printf("资金：%d，点数：%d，位置：%d\n", 
                currentPlayer->money, currentPlayer->points, currentPlayer->position);
            printf("地产：");
            for (int i = 0; i < 70; i++) {
                if (currentPlayer->house[i] == 1) {
                    printf("%d ", i);
                }
            }
            printf("\n道具：炸弹 %d，路障 %d，机器人 %d\n",
                currentPlayer->tool.bomb,
                currentPlayer->tool.roadblock,
                currentPlayer->tool.doll);
        }
        else if (strcmp(cmd, "roll") == 0) {
            srand(time(NULL));
            int roll = roll_dice();
            printf("玩家 %s 掷出了 %d 点，\n", player_getName(currentPlayer->character), roll);

            // for(int i = 1; i <= roll; i++){
            //     int nextPos = (currentPlayer->position + i) % 70;
            //     int j = find_place(map, nextPos);
            //     if(map[j].type == '@'){
            //         printf("你遇见了炸弹，被送往医院！\n");
            //         InHospital(currentPlayer);
            //         map[j].type = '0'; // Consume bomb
            //         playerManager_nextPlayer(&playerManager);
            //         goto next_turn;
            //     }
            //     else if(map[j].type == '#'){
            //         printf("你遇见了路障，停止前进！\n");
            //         currentPlayer->position = nextPos;
            //         playerManager_nextPlayer(&playerManager);
            //         goto next_turn;
            //     }
            // }
            for(int i = 0; i <= roll; i++){
                    int nextPos = (currentPlayer->position + i) % 70;
                    int j = find_place(map, nextPos);
                    if(map[j].type == 'F'){
                        printf("你遇见了天降财神，你太幸运啦！\n");
                        map[j].type = '0'; // Consume bomb
                        break;
                    }
                }
            currentPlayer->position = (currentPlayer->position + roll) % 70;
            printf(" 移动到位置 %d\n", currentPlayer->position);
             
            int i = find_place(map, currentPlayer->position);

            if(map[i].type == '0' && map[i].owner== NULL){
                printf("此处为空地，可以购买。\n");
                printf("是否购买此地？(y/n): ");
                char choice_buf[MAX_INPUT];
                if (fgets(choice_buf, sizeof(choice_buf), stdin) == NULL) break;
                char choice = choice_buf[0];
                if (choice == 'y' || choice == 'Y') {
                    buy_land(map + i, map[i].id, currentPlayer);
                } 
                else if(choice == 'n' || choice == 'N') {
                    printf("放弃购买此地。\n");
                }
                else {
                    printf("错误指令，输入help寻求帮助。\n");
                }
            }
            // else if(map[i].type == 'P'){
            //     printf("你遇见了监狱，停止前进！\n");
            //     InPrison(currentPlayer, map[i]);
            // }
            else if(map[i].owner == currentPlayer){
                printf("此处为你拥有的地产，可以升级或出售。\n");
                printf("是否升级或出售此地？(u 升级 / s 出售 / n 不操作): ");
                char choice_buf[MAX_INPUT];
                if (fgets(choice_buf, sizeof(choice_buf), stdin) == NULL) break;
                char choice = choice_buf[0];
                if (choice == 'u' || choice == 'U') {
                    upgrade_land(map + i, map[i].id, currentPlayer);
                } 
                else if(choice == 'n' || choice == 'N'){
                    printf("放弃操作此地。\n");
                }
                else {
                    printf("错误指令，输入help寻求帮助。\n");
                }
            }
            else if(map[i].owner != NULL && map[i].owner != currentPlayer){
                printf("此处为%s的地产，", player_getName(map[i].owner->character));
                GetTollFee(currentPlayer,&map[i]);
                if(player_isBankrupt(currentPlayer)){
                    IBankruptcy(currentPlayer,&playerManager,map);
                }
                if (playerManager_isGameWon(&playerManager)) {
                    Player* winner = playerManager_getWinner(&playerManager);
                    if (winner != NULL) {
                        printf("游戏结束！%s 获胜！\n", player_getName(winner->character));
                    }
                    break;
                }
                else{
                    printf("游戏继续。\n");
                }
            }
            else{
                game_handle_cell_event(currentPlayer, &map[i], &playerManager);
            }
            
            playerManager_nextPlayer(&playerManager);
            if (playerManager.currentPlayerIndex == 0) {
                        round_count++;
                        // 财神冷却机制
                        if (god_pos == -1 && game_turns > 0) {
                            game_turns--;
                            printf("[DEBUG] 财神冷却中，剩余 %d 回合\n", game_turns);
                        }
                        game_handle_turn(&god_pos, &god_turn, &god_used, &game_turns, map, &playerManager);
                        printf("当前到达 %d 回合\n", round_count);
                    }
        } 
        else if (strcmp(cmd, "sell") == 0) {
            if (param != NULL) {
                int pos = atoi(param);
                if(pos >= 0 && pos <= 69){
                    sell_land(map, pos, currentPlayer);
                }
                else{
                    printf("错误: 指令sell位置参数应在0到69之间\n");
                }
            }
            else {
                printf("错误: 指令sell需要指定位置\n");
            }
        }
        else {
            printf("未知命令，请输入 help 查看帮助。\n");
        }
        //next_turn:;
    }
}

int main(int argc, char* argv[]) {
    /* Check command line arguments */
    if (argc > 1) {
        if (strcmp(argv[1], "testhelloworld") == 0) {
            run_test_helloworld();
            return 0;
        }
        /* Test mode: if argv[1] is a directory, enter automated test mode */
        if (path_is_dir(argv[1])) {
            run_game_loop(1, argv[1]);
            return 0;
        }
        /* Fallback: unknown single-arg command */
        printf("Unknown command: %s\n", argv[1]);
        printf("Available commands:\n");
        printf("  testhelloworld - Run hello world test\n");
        printf("  (no arguments) - Run interactive game\n");
        printf("  <test_case_dir> - Run automated test mode with the case directory\n");
        return 1;
    }
    
    run_game_loop(0, NULL);
    
    return 0;
}

/* ---------------- Test Mode Helper Implementation ---------------- */

static int path_is_dir(const char* p) {
    if (!p) return 0;
    struct stat st;
    if (stat(p, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
}

static void json_escape_string(FILE* f, const char* s) {
    fputc('"', f);
    for (const unsigned char* p = (const unsigned char*)s; *p; ++p) {
        unsigned char c = *p;
        switch (c) {
            case '"': fputs("\\\"", f); break;
            case '\\': fputs("\\\\", f); break;
            case '\b': fputs("\\b", f); break;
            case '\f': fputs("\\f", f); break;
            case '\n': fputs("\\n", f); break;
            case '\r': fputs("\\r", f); break;
            case '\t': fputs("\\t", f); break;
            default:
                if (c < 0x20) {
                    fprintf(f, "\\u%04x", c);
                } else {
                    fputc(c, f);
                }
        }
    }
    fputc('"', f);
}

static char char_from_player(PlayerCharacter pc) {
    switch (pc) {
        case QIAN_FUREN: return 'Q';
        case ATURBER: return 'A';
        case SUN_XIAOMEI: return 'S';
        case JIN_BEIBEI: return 'J';
        default: return '?';
    }
}

/* Test mode: game.ended and winner are derived from real game logic, not preset */

static void write_dump_json(const char* case_dir, Structure* map, PlayerManager* pm) {
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", case_dir, "dump.json");
    FILE* f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "Failed to open %s for writing\n", path);
        return;
    }

    fputs("{\n", f);

    /* players */
    fputs("  \"players\": [\n", f);
    for (int i = 0; i < pm->playerCount; i++) {
        Player* p = &pm->players[i];
        fprintf(f, "    {\n");
        fprintf(f, "      \"index\": %d,\n", i);
        fprintf(f, "      \"name\": "); json_escape_string(f, (char[]){char_from_player(p->character), 0}); fputs(",\n", f);
        fprintf(f, "      \"fund\": %d,\n", p->money);
        fprintf(f, "      \"credit\": %d,\n", p->points);
        fprintf(f, "      \"location\": %d,\n", p->position);
        fprintf(f, "      \"alive\": %s,\n", (p->money >= 0 ? "true" : "false"));
        fprintf(f, "      \"prop\": {\n");
        fprintf(f, "        \"bomb\": %d,\n", p->tool.bomb);
        fprintf(f, "        \"barrier\": %d,\n", p->tool.roadblock);
        fprintf(f, "        \"robot\": %d,\n", p->tool.doll);
        fprintf(f, "        \"total\": %d\n", p->tool.total);
        fprintf(f, "      },\n");
        /* Buff durations not fully tracked in current data structures; default to 0 */
    fprintf(f, "      \"buff\": {\n");
    fprintf(f, "        \"god\": %d,\n", p->god_bless_days);
    fprintf(f, "        \"prison\": %d,\n", p->prison_days);
    fprintf(f, "        \"hospital\": %d\n", p->hospital_days);
    fprintf(f, "      }\n");
        fprintf(f, "    }%s\n", (i == pm->playerCount - 1) ? "" : ",");
    }
    fputs("  ],\n", f);

    /* houses */
    fputs("  \"houses\": {\n", f);
    int first_house = 1;
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        if (map[i].id >= 0 && map[i].owner != NULL && map[i].level >= 0) {
            if (!first_house) fputs(",\n", f); else first_house = 0;
            fprintf(f, "    \"%d\": { \"owner\": ", map[i].id);
            char owner_name[2] = { char_from_player(map[i].owner->character), 0 };
            json_escape_string(f, owner_name);
            fprintf(f, ", \"level\": %d }", map[i].level);
        }
    }
    if (!first_house) fputs("\n", f);
    fputs("  },\n", f);

    /* placed_prop */
    fputs("  \"placed_prop\": {\n", f);
    /* bombs */
    fputs("    \"bomb\": [", f);
    int first = 1;
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        if (map[i].type == '@') {
            if (!first) fputs(", ", f); else first = 0;
            fprintf(f, "%d", map[i].id);
        }
    }
    fputs("],\n", f);
    /* barriers */
    fputs("    \"barrier\": [", f);
    first = 1;
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        if (map[i].type == '#') {
            if (!first) fputs(", ", f); else first = 0;
            fprintf(f, "%d", map[i].id);
        }
    }
    fputs("]\n", f);
    fputs("  },\n", f);

    /* game */
    fputs("  \"game\": {\n", f);
    fprintf(f, "    \"now_player\": %d,\n", pm->currentPlayerIndex);
    int next_index = pm->playerCount > 0 ? ((pm->currentPlayerIndex + 1) % pm->playerCount) : 0;
    fprintf(f, "    \"next_player\": %d,\n", next_index);
    /* Compute ended/winner logically */
    int ended = playerManager_isGameWon(pm) ? 1 : 0;
    int winner_id = -1;
    if (ended) {
        Player* w = playerManager_getWinner(pm);
        if (w != NULL) {
            /* find index within current players */
            for (int i = 0; i < pm->playerCount; i++) {
                if (&pm->players[i] == w) { winner_id = i; break; }
            }
        }
    }
    fprintf(f, "    \"ended\": %s,\n", ended ? "true" : "false");
    fprintf(f, "    \"winner\": %d\n", winner_id);
    fputs("  }\n", f);

    fputs("}\n", f);
    fclose(f);
}

static char* read_file_all(const char* path, long* out_len) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc((size_t)n + 1);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(buf); return NULL; }
    buf[n] = '\0';
    fclose(f);
    if (out_len) *out_len = n;
    return buf;
}

static int parse_int_in(const char* start, const char* end, const char* key, int defval) {
    if (!start) return defval;
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char* p = start;
    while (p && p < end) {
        const char* k = strstr(p, pattern);
        if (!k || k >= end) break;
        const char* colon = strchr(k, ':');
        if (!colon || colon >= end) break;
        long v = strtol(colon + 1, NULL, 10);
        return (int)v;
    }
    return defval;
}

static int parse_str_char_in(const char* start, const char* end, const char* key, char defchar) {
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char* k = strstr(start, pattern);
    if (!k || k >= end) return defchar;
    const char* colon = strchr(k, ':');
    if (!colon || colon >= end) return defchar;
    const char* q1 = strchr(colon, '"');
    if (!q1 || q1 >= end) return defchar;
    const char* q2 = strchr(q1 + 1, '"');
    if (!q2 || q2 > end) return defchar;
    if (q2 > q1 + 1) return *(q1 + 1);
    return defchar;
}

static PlayerCharacter pc_from_char(char c) {
    switch (c) {
        case 'Q': return QIAN_FUREN;
        case 'A': return ATURBER;
        case 'S': return SUN_XIAOMEI;
        case 'J': return JIN_BEIBEI;
        default: return QIAN_FUREN;
    }
}

static void load_preset(const char* json, Structure* map, PlayerManager* pm) {
    const char* players = strstr(json, "\"players\"");
    if (players) {
        const char* lb = strchr(players, '[');
        const char* rb = lb ? strchr(lb, ']') : NULL;
        if (lb && rb && rb > lb) {
            const char* p = lb;
            int max_index = -1;
            while (1) {
                const char* ob = strchr(p, '{');
                if (!ob || ob >= rb) break;
                int braces = 1;
                const char* q = ob + 1;
                while (q < rb && braces > 0) {
                    if (*q == '{') braces++; else if (*q == '}') braces--; q++;
                }
                const char* oe = q; /* one past '}' */
                int idx = parse_int_in(ob, oe, "index", -1);
                if (idx >= 0 && idx < MAX_PLAYERS) {
                    if (idx + 1 > pm->playerCount) pm->playerCount = idx + 1;
                    if (idx > max_index) max_index = idx;
                    Player* pl = &pm->players[idx];
                    char namec = parse_str_char_in(ob, oe, "name", 'Q');
                    pl->character = pc_from_char(namec);
                    pl->money = parse_int_in(ob, oe, "fund", pl->money);
                    pl->points = parse_int_in(ob, oe, "credit", pl->points);
                    pl->position = parse_int_in(ob, oe, "location", pl->position);
                    /* props */
                    pl->tool.bomb = parse_int_in(ob, oe, "bomb", pl->tool.bomb);
                    pl->tool.roadblock = parse_int_in(ob, oe, "barrier", pl->tool.roadblock);
                    pl->tool.doll = parse_int_in(ob, oe, "robot", pl->tool.doll);
                    pl->tool.total = parse_int_in(ob, oe, "total", pl->tool.total);
                    /* buffs (read from nested buff object keys) */
                    int god_turns = parse_int_in(ob, oe, "god", 0);
                    int prison_turns = parse_int_in(ob, oe, "prison", 0);
                    int hospital_turns = parse_int_in(ob, oe, "hospital", 0);
                    pl->god_bless_days = god_turns;
                    pl->prison_days = prison_turns;
                    pl->hospital_days = hospital_turns;
                    pl->god = god_turns > 0;
                    pl->in_prison = prison_turns > 0;
                    pl->in_hospital = hospital_turns > 0;
                    /* guard in case fields exist */
                    #ifdef __GNUC__
                    /* best-effort: some builds have these fields */
                    extern void __attribute__((weak)) _ignore(void*);
                    #endif
                    /* If fields exist, set them via macros or direct; otherwise ignore */
                    /* Not strictly portable; kept simple */
                }
                p = oe;
            }
        }
    }

    /* placed_prop */
    const char* pprop = strstr(json, "\"placed_prop\"");
    if (pprop) {
        const char* bombk = strstr(pprop, "\"bomb\"");
        if (bombk) {
            const char* lb = strchr(bombk, '[');
            const char* rb = lb ? strchr(lb, ']') : NULL;
            if (lb && rb && rb > lb) {
                const char* s = lb + 1;
                while (s < rb) {
                    char* endptr;
                    long v = strtol(s, &endptr, 10);
                    if (endptr == s) { s++; continue; }
                    int id = (int)v;
                    int idx = find_place(map, id);
                    if (idx >= 0) map[idx].type = '@';
                    s = endptr;
                }
            }
        }
        const char* bark = strstr(pprop, "\"barrier\"");
        if (bark) {
            const char* lb = strchr(bark, '[');
            const char* rb = lb ? strchr(lb, ']') : NULL;
            if (lb && rb && rb > lb) {
                const char* s = lb + 1;
                while (s < rb) {
                    char* endptr;
                    long v = strtol(s, &endptr, 10);
                    if (endptr == s) { s++; continue; }
                    int id = (int)v;
                    int idx = find_place(map, id);
                    if (idx >= 0) map[idx].type = '#';
                    s = endptr;
                }
            }
        }
    }

    /* houses */
    const char* houses = strstr(json, "\"houses\"");
    if (houses) {
        const char* lb = strchr(houses, '{');
        const char* rb = NULL;
        if (lb) {
            int depth = 1; const char* p = lb + 1;
            while (*p) {
                if (*p == '{') depth++;
                else if (*p == '}') { depth--; if (depth == 0) { rb = p; break; } }
                p++;
            }
        }
        if (lb && rb && rb > lb) {
            const char* s = lb + 1;
            while (s < rb) {
                while (s < rb && *s != '"') s++;
                if (s >= rb) break;
                const char* q1 = s + 1;
                const char* q2 = strchr(q1, '"');
                if (!q2 || q2 >= rb) break;
                int id = atoi(q1);
                const char* colon = strchr(q2, ':');
                if (!colon || colon >= rb) break;
                const char* ob = strchr(colon, '{');
                if (!ob || ob >= rb) { s = q2 + 1; continue; }
                int braces = 1; const char* p2 = ob + 1;
                while (p2 < rb && braces > 0) { if (*p2 == '{') braces++; else if (*p2 == '}') braces--; p2++; }
                const char* oe = p2; /* one past */
                char ownerc = parse_str_char_in(ob, oe, "owner", 'Q');
                int level = parse_int_in(ob, oe, "level", 0);
                int mi = find_place(map, id);
                if (mi >= 0) {
                    map[mi].level = level;
                    /* set owner pointer */
                    for (int i = 0; i < pm->playerCount; i++) {
                        if (char_from_player(pm->players[i].character) == ownerc) {
                            map[mi].owner = &pm->players[i];
                            break;
                        }
                    }
                }
                s = oe + 1;
            }
        }
    }

    /* game: only respect now_player; ignore ended/winner in presets */
    const char* game = strstr(json, "\"game\"");
    if (game) {
        const char* lb = strchr(game, '{');
        const char* rb = lb ? strchr(lb, '}') : NULL;
        if (lb && rb && rb > lb) {
            int nowp = parse_int_in(lb, rb, "now_player", pm->currentPlayerIndex);
            if (nowp >= 0 && nowp < pm->playerCount) pm->currentPlayerIndex = nowp;
        }
    }
}