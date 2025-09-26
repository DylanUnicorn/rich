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
    char choice;
    int result;
    
    while (true) {
        result = scanf(" %c", &choice);
        
        // 检查scanf返回值
        if (result != 1) {
            // 输入失败，清空输入缓冲区
            while (getchar() != '\n');
            printf("无效输入，请重试。\n");
            printf("请输入要获取的礼品编号(或输入f退出): ");
            continue;
        }
        
        if (choice == 'f' || choice == 'F') {
            printf("退出礼品屋。\n");
            break;          
        } else if (choice == '1') {
            currentPlayer->money += 2000;
            printf("您已获取 2000 元。当前资金: %d\n", currentPlayer->money);
            getchar(); 
            break;
        } else if (choice == '2') {
            currentPlayer->points += 200;
            printf("您已获取 200 点数。当前点数: %d\n", currentPlayer->points);
            getchar(); 
            break;
        } else if (choice == '3') {
            currentPlayer->god = true;
            currentPlayer->god_bless_days = 5; // Assuming god bless lasts for 5 rounds
            printf("您已获取神仙保佑！！！接下来五回合您无需支付过路费。\n");
            getchar(); 
            break;
        } else {
            printf("无效选择，请重新选择。\n");
        }
    }
}
