#include "gifthouse.h"
#include <stdio.h>
void giftHouse_init() {
    // 礼品屋初始化
    printf("欢迎来到礼品屋，请选择您的礼品：\n");
    printf("1. 获取 2000 元 \n");
    printf("2. 获取 200 点数 \n");
    printf("3. 获取财神（五回合内不用支付过路费）\n");
    printf("请输入要获取的礼品编号(或输入f退出): ");
}

void player_getintoGiftHouse(Player* currentPlayer) {
    giftHouse_init();
    char choice[256];
     if (fgets(choice, sizeof(choice), stdin) == NULL) {
        printf("读取输入失败。\n");
        return;
    }
    
    // 移除换行符
    size_t len = strlen(choice);
    if (len > 0 && choice[len-1] == '\n') {
        choice[len-1] = '\0';
    }
    
    // 使用完整的字符串比较而不是只比较第一个字符
    if (strcmp(choice, "f") == 0 || strcmp(choice, "F") == 0) {
        printf("退出礼品屋。\n");
        return;          
    } else if (strcmp(choice, "1") == 0) {
        currentPlayer->money += 2000;
        printf("您已获得 2000 元。当前余额: %d\n", currentPlayer->money);
    } else if (strcmp(choice, "2") == 0) {
        currentPlayer->points += 200;
        printf("您已获得 200 点数。当前点数: %d\n", currentPlayer->points);
    } else if (strcmp(choice, "3") == 0) {
        currentPlayer->god = true;
        currentPlayer->god_bless_days += 5;
        printf("您已获得财神附体，五回合内不用支付过路费！\n");
    } else {
        printf("无效输入 '%s'，很遗憾您放弃了本次礼品屋的选择。\n", choice);
    }

}