#pragma once

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

// 玩家颜色
static inline const char* getPlayerColor(char code) {
    switch (code) {
        case 'Q': return COLOR_RED;
        case 'A': return COLOR_GREEN;
        case 'S': return COLOR_BLUE;
        case 'J': return COLOR_YELLOW;
        default:  return COLOR_WHITE;
    }
}