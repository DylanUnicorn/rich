#include "gifthouse.h"
#include <stdio.h>
void giftHouse_init() {
    // 礼品屋初始化
    printf("Welcome to the Gift House, please select your gift:\n");
    printf("1. Receive 2000 yuan \n");
    printf("2. Receive 200 points \n");
    printf("3. Receive god bless!!!(Five rounds without paying tolls)\n");
    printf("Enter the number of the gift you want to receive (or f to exit): ");
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
            printf("Invalid input, please try again.\n");
            printf("Enter the number of the gift you want to receive (or f to exit): ");
            continue;
        }
        
        if (choice == 'f' || choice == 'F') {
            printf("Exiting the Gift House.\n");
            break;          
        } else if (choice == '1') {
            currentPlayer->money += 2000;
            printf("You have received 2000 yuan. Current money: %d\n", currentPlayer->money);
            break;
        } else if (choice == '2') {
            currentPlayer->points += 200;
            printf("You have received 200 points. Current points: %d\n", currentPlayer->points);
            break;
        } else if (choice == '3') {
            currentPlayer->god = true;
            currentPlayer->god_bless_days = 5; // Assuming god bless lasts for 5 rounds
            printf("You have received god bless!!! You won't pay tolls for the next five rounds.\n");
            break;
        } else {
            printf("Invalid choice, please select again.\n");
        }
    }
}
