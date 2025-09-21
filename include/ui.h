// ui.h
#ifndef UI_H
#define UI_H

#include "Player.h"
#include <stdbool.h>

// 颜色定义
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

// 地图大小
#define MAP_SIZE 70

// 单元格类型
typedef enum {
    CELL_EMPTY,      // 空地
    CELL_PROPERTY,   // 房产
    CELL_TOOL_SHOP,  // 道具屋
    CELL_GIFT_HOUSE, // 礼品屋
    CELL_MAGIC_HOUSE,// 魔法屋
    CELL_HOSPITAL,   // 医院
    CELL_JAIL,       // 监狱
    CELL_START,      // 起点
    CELL_MINE        // 矿地
} CellType;

// UI函数
void ui_display_players(PlayerManager* player_manager);
void ui_display_prompt(Player* player);
void ui_display_message(const char* message);
void ui_display_error(const char* error);
void ui_display_help();
void ui_clear_screen();
const char* ui_get_player_color(PlayerCharacter character);

#endif // UI_H