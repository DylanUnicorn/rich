#include "prophouse.h"
#include <stdio.h>  // 确保包含了标准输入输出头文件

void propHouse_init() {
    // 道具屋初始化
    printf("Welcome to the Prop House, please select the props you need:\n");
    printf("1. Roadblock (Cost: 200)\n");
    printf("2. Robot (Cost: 300)\n");
    printf("3. Bomb (Cost: 400)\n");
    printf("Enter the number of the prop you want to buy (or f to exit): ");
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
            printf("Invalid input, please try again.\n");
            printf("Enter the number of the prop you want to buy (or f to exit): ");
            continue;
        }
        
        if (choice == 'f' || choice == 'F') {
            printf("Exiting the Prop House.\n");
            break;          
        } else if (choice == '1') {
            if (currentPlayer->points >= 200) {
                currentPlayer->tool.roadblock++;
                currentPlayer->points -= 200;
                currentPlayer->tool.total++;
                printf("You have purchased a Roadblock. Remaining points: %d\n", currentPlayer->points);
            } else {
                printf("Insufficient points to purchase Roadblock.\n");
            }
        } else if (choice == '2') {
            if (currentPlayer->points >= 300) {
                currentPlayer->tool.doll++;
                currentPlayer->points -= 300;
                currentPlayer->tool.total++;
                printf("You have purchased a Robot. Remaining points: %d\n", currentPlayer->points);
            } else {
                printf("Insufficient points to purchase Robot.\n");
            }
        } else if (choice == '3') {
            if (currentPlayer->points >= 400) {
                currentPlayer->tool.bomb++;
                currentPlayer->points -= 400;
                currentPlayer->tool.total++;
                printf("You have purchased a Bomb. Remaining points: %d\n", currentPlayer->points);
            } else {
                printf("Insufficient points to purchase Bomb.\n");
            }
        } else {
            printf("Invalid choice, please select again.\n");  
        }
        printf("Enter the number of the prop you want to buy (or f to exit): ");    
    }
}