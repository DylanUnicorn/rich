// ui.c
#include "ui.h"
#include "Player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// 获取玩家颜色代码
const char* ui_get_player_color(PlayerCharacter character) {
    switch (character) {
        case QIAN_FUREN: return COLOR_RED;
        case ATURBER:    return COLOR_GREEN;
        case SUN_XIAOMEI: return COLOR_BLUE;
        case JIN_BEIBEI: return COLOR_YELLOW;
        default:         return COLOR_WHITE;
    }
}


// 显示玩家信息
void ui_display_players(PlayerManager* player_manager) {
    printf("\n玩家信息:\n");
    for (int i = 0; i < player_manager->playerCount; i++) {
        Player* player = &player_manager->players[i];
        printf("%s%s%s: 位置=%d, 金钱=%d, 点数=%d\n", 
               ui_get_player_color(player->character),
               player_getName(player->character),
               COLOR_RESET,
               player->position,
               player->money,
               player->points);
    }
}

// 显示命令提示符
void ui_display_prompt(Player* player) {
    if (player) {
        printf("%s%s>%s ", ui_get_player_color(player->character), 
               player_getName(player->character), COLOR_RESET);
    } else {
        printf("> ");
    }
}

// 显示消息
void ui_display_message(const char* message) {
    printf("%s\n", message);
}

// 显示错误信息
void ui_display_error(const char* error) {
    printf("%s错误: %s%s\n", COLOR_RED, error, COLOR_RESET);
}

// 显示帮助信息
void ui_display_help() {
    printf("\n可用命令:\n");
    printf("  roll        - 掷骰子移动\n");
    printf("  sell <位置> - 出售指定位置的房产\n");
    printf("  block <步数> - 放置路障（正数向前，负数向后）\n");
    printf("  bomb <步数>  - 放置炸弹（正数向前，负数向后）\n");
    printf("  robot       - 使用机器娃娃清除前方10步内的道具\n");
    printf("  query       - 查看自己的资产信息\n");
    printf("  help        - 显示帮助信息\n");
    printf("  quit        - 退出游戏\n");
    printf("\n");
}

// 清屏
void ui_clear_screen() {
    // 使用 ANSI 转义序列清屏
    printf("\033[2J\033[H");
    fflush(stdout);
}