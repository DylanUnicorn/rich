#include "prophouse.h"
#include <stdio.h>  // 确保包含了标准输入输出头文件

void propHouse_init() {
    // 道具屋初始化
    printf("欢迎来到道具屋，请选择您需要的道具：\n");
    printf("1. 路障 (价格: 200点)\n");
    printf("2. 机器人 (价格: 300点)\n");
    printf("3. 炸弹 (价格: 400点)\n");
    printf("请输入要购买的道具编号(或输入f退出): ");
}

void player_getintoPropHouse(Player* currentPlayer) {
    propHouse_init();
    char choice;
    int result;
    
    while (true) {
        result = scanf(" %c", &choice);
        
        // 检查scanf返回值
        if (result != 1) {
            // 输入失败，清空输入缓冲区
            while (getchar() != '\n');
            printf("无效输入，请重试。\n");
            printf("请输入要购买的道具编号(或输入f退出): ");
            continue;
        }
        
        if (choice == 'f' || choice == 'F') {
            printf("退出道具屋。\n");
            break;          
        } else if (choice == '1') {
            if (currentPlayer->points >= 200) {
                currentPlayer->tool.roadblock++;
                currentPlayer->points -= 200;
                currentPlayer->tool.total++;
                printf("您已购买了一个路障。剩余点数: %d\n", currentPlayer->points);
            } else {
                printf("点数不足，无法购买路障。\n");
            }
        } else if (choice == '2') {
            if (currentPlayer->points >= 300) {
                currentPlayer->tool.doll++;
                currentPlayer->points -= 300;
                currentPlayer->tool.total++;
                printf("您已购买了一个机器人。剩余点数: %d\n", currentPlayer->points);
            } else {
                printf("点数不足，无法购买机器人。\n");
            }
        } else if (choice == '3') {
            if (currentPlayer->points >= 400) {
                currentPlayer->tool.bomb++;
                currentPlayer->points -= 400;
                currentPlayer->tool.total++;
                printf("您已购买了一个炸弹。剩余点数: %d\n", currentPlayer->points);
            } else {
                printf("点数不足，无法购买炸弹。\n");
            }
        } else {
            printf("无效选择，请重新选择。\n");  
        }
        printf("请输入要购买的道具编号(或输入f退出): ");    
    }
}