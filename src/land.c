#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "land.h"
#include "map.h"


void buy_land(Structure* map, int position, Player* player) {
    if (map == NULL || player == NULL || position < 0 || position >= HEIGHT * WIDTH) return;

    int i = find_place(map, position);
    if (map[i].type == '0' && map[i].owner== NULL) { // Only unowned normal lands can be bought
        if (player_getMoney(player) >= map[i].money) {
            player_setMoney(player, player_getMoney(player) - map[i].money);
            map[i].owner = player;
            map[i].level = 0; // Initial land level
            printf("%s以%d元购买了位置%d的土地。\n", player_getName(player->character), map[i].money, position);
        }
        else {
            printf("%s没有足够的资金购买这块土地。\n", player_getName(player->character));
        }
    }
    else {
        printf("这块土地不能购买。\n");
    }
}

void sell_land(Structure* map, int position, Player* player) {
    if (map == NULL || player == NULL || position < 0 || position >= HEIGHT * WIDTH) return;

    int i = find_place(map, position);
    if (map[i].owner == player) { // Only the owner can sell the land
        int sellPrice = map[i].money * (map[i].level + 1) * 2; // Selling price is twice of the purchase price
        player_setMoney(player, player_getMoney(player) + sellPrice);
        map[i].owner = NULL;
        map[i].level = -1; // Reset land level
        printf("%s以%d元出售了位置%d的土地。\n", player_getName(player->character), sellPrice, position);
    }
    else {
        printf("不是你的地产，你无法出售。\n");
    }
}

void upgrade_land(Structure* map, int position, Player* player) {
    if (map == NULL || player == NULL || position < 0 || position >= HEIGHT * WIDTH) return;

    int i = find_place(map, position);
    if (map[i].owner == player) { // Only the owner can upgrade the land
        if (map[i].level < MAX_LEVEL) {
            int upgradeCost = map[i].money / 2 * (map[i].level + 1); // Upgrade cost increases with level
            if (player_getMoney(player) >= upgradeCost) {
                player_setMoney(player, player_getMoney(player) - upgradeCost);
                map[i].level++;
                printf("%s花费%d元将位置%d的土地升级到%d级。\n", player_getName(player->character), upgradeCost, position, map[i].level);
            }
            else {
                printf("%s没有足够的资金升级这块土地。\n", player_getName(player->character));
            }
        }
        else {
            printf("这块土地已达到最高等级。\n");
        }
    }
    else {
        printf("无法升级。\n");
    }
}