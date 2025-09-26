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
static int is_digits_1_4_unique(const char* s);
static void add_players_from_selection(PlayerManager* pm, const char* sel, int initialMoney);
static int run_test_mode(const char* case_dir);
#define MAX_INPUT 100

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
    char input[MAX_INPUT];
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
            Player* currentPlayer = playerManager_getCurrentPlayer(&playerManager);
            if (currentPlayer == NULL) break;
            if(currentPlayer->bankruptcy == true){
                printf("%s 已破产，跳过此回合。\n", player_getName(currentPlayer->character));
                playerManager_nextPlayer(&playerManager);
                continue;
            }
            print_map(map, &playerManager);

            // 显示带颜色的提示符
            ui_display_prompt(currentPlayer);

            // 这里处理命令，比如 roll, query, help, quit 等*/
            // 读取整行输入
            if(currentPlayer->in_hospital){
                printf("你在医院，无法进行其他操作，回车结束回合。/n");
                currentPlayer->hospital_days--;
                if(currentPlayer->hospital_days <= 0)
                    currentPlayer->in_hospital = false;
                playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
            }
            else if(currentPlayer->in_prison){
                printf("你在监狱，无法进行其他操作，回车结束回合。\n");
                currentPlayer->prison_days--;
                if(currentPlayer->prison_days <= 0)
                    currentPlayer->in_prison = false;
                playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
            }

            if (fgets(input, MAX_INPUT, stdin) == NULL) {
                break;
            }
            ui_clear_screen();
            // 移除换行符
            input[strcspn(input, "\n")] = 0;

            // 分割输入
            char *cmd = strtok(input, " ");
            char *param = strtok(NULL, " ");

            if (cmd == NULL) {
                continue; // 空输入
            }
            
            if (strcmp(cmd, "quit") == 0) {
                printf("游戏结束。\n");
                break;
            }
            else if (strcmp(cmd, "step") == 0) {
                if (param != NULL) {
                    int steps = atoi(param);
                    int is_bombed_or_blocked = 0;
                    //如果接下来遇见炸弹就被送往医院
                    for(int i = 0; i <= steps; i++){
                        int nextPos = (currentPlayer->position + i) % 70;
                        int j = find_place(map, nextPos);
                        if(map[j].type == '@'){
                            map[j].type = '0'; // 清除炸弹
                            printf("你遇见了炸弹，被送往医院！\n");
                            InHospital(currentPlayer);
                            playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                            is_bombed_or_blocked = 1;
                            break;
                        }
                        else if(map[j].type == '#'){
                            map[j].type = '0'; // 清除路障
                            printf("你遇见了路障，停止前进！\n");
                            currentPlayer->position = (currentPlayer->position + i) % 70;
                            playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                            is_bombed_or_blocked = 1;
                            break;
                        }
                    }
                if (is_bombed_or_blocked) continue;
                    currentPlayer->position = (currentPlayer->position + steps) % 70; // 假设地图有70个位置
                    printf("你移动到了位置 %d\n", currentPlayer->position);

                int i = find_place(map, currentPlayer->position);

                if(map[i].type == '0' && map[i].owner== NULL){
                    printf("此处为空地，可以购买。\n");
                    printf("是否购买此地？(y/n): ");
                    char choice;                    
                    if (scanf("%c", &choice) != 1) {
                        // 处理输入错误
                        printf("输入错误，请输入 y 或 n。\n");
                        continue;
                    }
                    while (getchar() != '\n'); // 清除输入缓冲区
                    if (choice == 'y' || choice == 'Y') {
                        buy_land(map, currentPlayer->position, currentPlayer);
                    } 
                    else if(choice == 'n' || choice == 'N') {
                        printf("放弃购买此地。\n");
                    }
                    else {
                        printf("错误指令，输入help寻求帮助。\n");
                    }
                }
                else if(map[i].type == 'P'){
                    printf("你遇见了监狱，停止前进！\n");
                    currentPlayer->in_prison = true;
                    currentPlayer->prison_days = 2; // 监狱停留2天
                    playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                }
                else if(map[i].owner == currentPlayer){
                    printf("此处为你拥有的地产，可以升级或出售。\n");
                    printf("是否升级此地？(u 升级 / n 不操作): ");
                    char choice;
                    if (scanf(" %c", &choice) != 1) {
                        // 处理输入错误
                        printf("输入错误，请输入 u 或 n。\n");
                        continue;
                    }
                    while (getchar() != '\n'); // 清除输入缓冲区
                    if (choice == 'u' || choice == 'U') {
                        upgrade_land(map, currentPlayer->position, currentPlayer);
                    } 
                    else if(choice == 'n' || choice == 'N'){
                        printf("放弃操作此地。\n");
                    }
                    else {
                        printf("错误指令，输入help寻求帮助。\n");
                    }
                }
                else if(map[i].owner != NULL && map[i].owner != currentPlayer){
                    printf("此处为%s的地产\n", player_getName(map[i].owner->character));
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
                    //printf("此处为特殊地块，触发相应事件。\n");
                    game_handle_cell_event(currentPlayer, &map[i], &playerManager);
                    //trigger_special_tile_event(map, currentPlayer);
                } 
                } else {
                printf("错误: step需要指定步数\n");
                }
            } 
            else if (strcmp(cmd, "help") == 0) {
                ui_display_help();
            }//////////////////////////////////////////////////////////////////////////////////////////
            else if (strcmp(cmd, "block") == 0) {
                if (param != NULL) {
                    int blocks = atoi(param);
                    if(blocks >= -10 && blocks <= 10 && blocks != 0){
                        if(currentPlayer->tool.roadblock > 0){
                            currentPlayer->tool.roadblock--;
                            currentPlayer->tool.total--;
                            int i = find_place(map, currentPlayer->position + blocks);
                            map[i].type = '#'; // 设置为路障
                        }
                        else{
                            printf("你没有路障道具，无法使用。\n");
                            continue;
                        }
                    }
                    else{
                        printf("错误: 指令block位置参数应在-10到10之间且不为0\n");
                        continue;
                    }
                }
                else {
                    printf("错误: 指令block需要指定位置\n");
                }
            } ////////////////////////////////////////////////////////////////////////////////////////
            else if (strcmp(cmd, "bomb") == 0) {
                if (param != NULL) {
                    int bombs = atoi(param);
                    if(bombs >= -10 && bombs <= 10 && bombs != 0){
                        if(currentPlayer->tool.bomb > 0){
                            currentPlayer->tool.bomb--;
                            currentPlayer->tool.total--;
                            int i = find_place(map, currentPlayer->position + bombs);
                            map[i].type = '@'; // 设置为路障
                        }
                        else{
                            printf("你没有炸弹道具，无法使用。\n");
                            continue;
                        }
                    }
                    else{
                        printf("错误: 指令bomb位置参数应在-10到10之间且不为0\n");
                    }

                }
                else {
                    printf("错误: 指令bomb需要指定位置\n");
                }
            } //////////////////////////////////////////////////////////////////////////////////////
            else if (strcmp(cmd, "robot") == 0) {
                if (currentPlayer->tool.doll > 0) {
                    currentPlayer->tool.doll--;
                    currentPlayer->tool.total--;
                    int i = find_place(map, currentPlayer->position);
                    for(i = 0; i < 10; i++){
                        map[i].type = TOOL_NONE; // 清除路障或炸弹
                        break;
                    }
                }
                else {
                    printf("你没有机器人道具，无法使用。\n");
                    continue;
                }
            } //////////////////////////////////////////////////////////////////////////////////////
            else if (strcmp(cmd, "query") == 0) {
                // 显示当前玩家资产
                printf("资金：%d，点数：%d，位置：%d\n", 
                    currentPlayer->money, currentPlayer->points, currentPlayer->position);
                printf("您的地产位置：");
                for(int i = 0; i < 70; i++){
                    if(currentPlayer->house[i] == 1)
                        printf("%d ", i);
                }
                printf("\n");
                printf("道具：炸弹 %d 个，路障 %d 个，机器人 %d 个\n", 
                    currentPlayer->tool.bomb, currentPlayer->tool.roadblock, currentPlayer->tool.doll);
            }
            else if (strcmp(cmd, "roll") == 0) {
                //Player_use_roll_dice( currentPlayer);
                srand(time(NULL));
                int roll = roll_dice();
                int is_bombed_or_blocked = 0;
                for(int i = 0; i <= roll; i++){
                    int nextPos = (currentPlayer->position + i) % 70;
                    int j = find_place(map, nextPos);
                    if(map[j].type == '@'){
                        map[j].type = '0'; // 清除炸弹
                        printf("你遇见了炸弹，被送往医院！\n");
                        InHospital(currentPlayer);
                        playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                        is_bombed_or_blocked = 1;
                        break;
                    }
                    else if(map[j].type == '#'){
                        map[j].type = '0'; // 清除路障
                        printf("你遇见了路障，停止前进！\n");
                        currentPlayer->position = (currentPlayer->position + j) % 70;
                        playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                        is_bombed_or_blocked = 1;
                        break;
                    }
                }
                if(is_bombed_or_blocked) continue;
                printf("玩家 %s 掷出了 %d 点，\n", player_getName(currentPlayer->character), roll);
                currentPlayer->position = (currentPlayer->position + roll) % 70; // 假设地图有70个位置
                printf(" 移动到位置 %d\n", currentPlayer->position);
                // 掷骰子逻辑
                printf("（此处掷骰子...）\n");
                 
                int i = find_place(map, currentPlayer->position);

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
                        buy_land(map, currentPlayer->position, currentPlayer);
                    } 
                    else if(choice == 'n' || choice == 'N') {
                        printf("放弃购买此地。\n");
                    }
                    else {
                        printf("错误指令，输入help寻求帮助。\n");
                    }
                }
                else if(map[i].type == 'P'){
                    printf("你遇见了监狱，停止前进！\n");
                    currentPlayer->in_prison = true;
                    currentPlayer->prison_days = 2; // 监狱停留2天
                    playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
                }
                else if(map[i].owner == currentPlayer){
                    printf("此处为你拥有的地产，可以升级或出售。\n");
                    printf("是否升级或出售此地？(u 升级 / s 出售 / n 不操作): ");
                    char choice;
                    if (scanf(" %c", &choice) != 1) {
                        // 处理输入错误
                        printf("输入错误，请输入 u 或 n。\n");
                        continue;
                    }
                    while (getchar() != '\n'); // 清除输入缓冲区
                    if (choice == 'u' || choice == 'U') {
                        upgrade_land(map, currentPlayer->position, currentPlayer);
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
                    //printf("此处为特殊地块，触发相应事件。\n");
                    game_handle_cell_event(currentPlayer, &map[i], &playerManager);
                }
                
                playerManager_nextPlayer(&playerManager); // 轮到下一个玩家
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
        }
    }
    else {
        printf("游戏初始化失败！\n");
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
            return run_test_mode(argv[1]);
        }
        /* Fallback: unknown single-arg command */
        printf("Unknown command: %s\n", argv[1]);
        printf("Available commands:\n");
        printf("  testhelloworld - Run hello world test\n");
        printf("  (no arguments) - Run interactive game or command mode\n");
        printf("  <test_case_dir> - Run automated test mode with the case directory\n");
        return 1;
    }
    
    /* Check if input is redirected (for automated testing) */
    if (!isatty(fileno(stdin))) {
        run_command_mode();
    } else {
        run_interactive_game();
    }
    
    return 0;
}

/* ---------------- Test Mode Implementation ---------------- */

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

static int g_test_ended_flag = 1; /* default ended after dump */
static int g_test_winner_id = -1;

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
    fprintf(f, "    \"ended\": %s,\n", g_test_ended_flag ? "true" : "false");
    fprintf(f, "    \"winner\": %d\n", g_test_winner_id);
    fputs("  }\n", f);

    fputs("}\n", f);
    fclose(f);
}

static int is_digits_1_4_unique(const char* s) {
    if (!s) return 0;
    int len = (int)strlen(s);
    if (len < 2 || len > 4) return 0;
    int used[5] = {0};
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c < '1' || c > '4') return 0;
        int d = c - '0';
        if (used[d]) return 0;
        used[d] = 1;
    }
    return 1;
}

static void add_players_from_selection(PlayerManager* pm, const char* sel, int initialMoney) {
    int len = (int)strlen(sel);
    for (int i = 0; i < len; i++) {
        int d = sel[i] - '0';
        playerManager_addPlayer(pm, (PlayerCharacter)d, initialMoney);
    }
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

    /* game */
    const char* game = strstr(json, "\"game\"");
    if (game) {
        const char* lb = strchr(game, '{');
        const char* rb = lb ? strchr(lb, '}') : NULL;
        if (lb && rb && rb > lb) {
            int nowp = parse_int_in(lb, rb, "now_player", pm->currentPlayerIndex);
            if (nowp >= 0 && nowp < pm->playerCount) pm->currentPlayerIndex = nowp;
            int ended = parse_int_in(lb, rb, "ended", g_test_ended_flag);
            int winner = parse_int_in(lb, rb, "winner", g_test_winner_id);
            g_test_ended_flag = ended ? 1 : 0;
            g_test_winner_id = winner;
        }
    }
}

int run_test_mode(const char* case_dir) {
    /* Initialize game state */
    Structure map[HEIGHT * WIDTH];
    init_map(map);
    GameConfig config; gameConfig_init(&config);
    PlayerManager pm; playerManager_init(&pm);

    /* If preset.json exists, load it and skip money/selection from stdin */
    int use_preset = 0;
    char preset_path[1024];
    snprintf(preset_path, sizeof(preset_path), "%s/%s", case_dir, "preset.json");
    long preset_len = 0; char* preset_json = read_file_all(preset_path, &preset_len);
    if (preset_json) {
        use_preset = 1;
        load_preset(preset_json, map, &pm);
        free(preset_json);
    }

    /* Input handling from stdin: if no preset, first line money, second line selection, then commands */
    char line[256];
    int stage = 0;
    int initialMoney = 10000;
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; /* trim newline */
        if (strlen(line) == 0) continue;

        if (!use_preset && stage == 0) {
            /* initial money */
            char* endptr = NULL;
            long v = strtol(line, &endptr, 10);
            if (endptr != line && v >= 1000 && v <= 50000) {
                initialMoney = (int)v;
            }
            gameConfig_setInitialMoney(&config, initialMoney);
            stage = 1;
            continue;
        }
        if (!use_preset && stage == 1) {
            /* player selection like 421 */
            if (!is_digits_1_4_unique(line)) {
                fprintf(stderr, "Invalid player selection: %s\n", line);
                return 1;
            }
            add_players_from_selection(&pm, line, initialMoney);
            stage = 2;
            continue;
        }

        /* Commands supported in test mode: step n, dump, quit, block n, bomb n, robot */
        if (strcmp(line, "dump") == 0) {
            write_dump_json(case_dir, map, &pm);
            /* End immediately after dump */
            return 0;
        }
        if (strcmp(line, "quit") == 0) {
            /* No dump; exit */
            return 0;
        }
        if (strncmp(line, "step ", 5) == 0) {
            const char* nstr = line + 5;
            long steps = strtol(nstr, NULL, 10);
            if (steps < 0) steps = 0;
            Player* current = playerManager_getCurrentPlayer(&pm);
            if (current) {
                /* If in hospital or prison, skip movement and decrement counters */
                if (current->hospital_days > 0 || current->in_hospital) {
                    if (current->hospital_days > 0) current->hospital_days -= 1;
                    if (current->hospital_days <= 0) { current->hospital_days = 0; current->in_hospital = false; }
                } else if (current->prison_days > 0 || current->in_prison) {
                    if (current->prison_days > 0) current->prison_days -= 1;
                    if (current->prison_days <= 0) { current->prison_days = 0; current->in_prison = false; }
                } else {
                    /* Step-by-step movement with obstacle handling */
                    int pos = current->position;
                    int stopped = 0;
                    for (long d = 1; d <= steps; ++d) {
                        int nextPos = (pos + 1) % 70;
                        int idx = find_place(map, nextPos);
                        if (idx >= 0) {
                            if (map[idx].type == '@') {
                                /* hit a bomb: hospitalized and consume bomb */
                                InHospital(current);
                                map[idx].type = '0';
                                pos = current->position; /* hospital sets absolute position */
                                stopped = 1;
                                break;
                            } else if (map[idx].type == '#') {
                                /* barrier: stop at barrier tile */
                                pos = nextPos;
                                stopped = 1;
                                break;
                            } else {
                                pos = nextPos;
                            }
                        } else {
                            pos = nextPos;
                        }
                    }
                    current->position = pos;
                    if (!stopped) {
                        /* If landed on prison tile, go to prison (simulate landing effect) */
                        int idx2 = find_place(map, current->position);
                        if (idx2 >= 0 && map[idx2].type == 'P') {
                            InPrison(current, map[idx2]);
                        }
                    }
                }
            }
            continue;
        }
        if (strncmp(line, "block ", 6) == 0) {
            const char* nstr = line + 6;
            long off = strtol(nstr, NULL, 10);
            if (off >= -10 && off <= 10 && off != 0) {
                Player* current = playerManager_getCurrentPlayer(&pm);
                if (current) {
                    int target = (current->position + (int)off) % 70; if (target < 0) target += 70;
                    int idx = find_place(map, target);
                    if (idx >= 0) map[idx].type = '#';
                }
            }
            continue;
        }
        if (strncmp(line, "bomb ", 5) == 0) {
            const char* nstr = line + 5;
            long off = strtol(nstr, NULL, 10);
            if (off >= -10 && off <= 10 && off != 0) {
                Player* current = playerManager_getCurrentPlayer(&pm);
                if (current) {
                    int target = (current->position + (int)off) % 70; if (target < 0) target += 70;
                    int idx = find_place(map, target);
                    if (idx >= 0) map[idx].type = '@';
                }
            }
            continue;
        }
        if (strcmp(line, "robot") == 0) {
            Player* current = playerManager_getCurrentPlayer(&pm);
            if (current) {
                for (int d = 1; d <= 10; d++) {
                    int pos = (current->position + d) % 70; if (pos < 0) pos += 70;
                    int idx = find_place(map, pos);
                    if (idx >= 0 && (map[idx].type == '@' || map[idx].type == '#')) {
                        map[idx].type = '0';
                    }
                }
            }
            continue;
        }
        /* Unknown test command: ignore */
    }

    /* If EOF without dump, still write a dump for debugging */
    write_dump_json(case_dir, map, &pm);
    return 0;
}