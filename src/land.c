#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "land.h"


void buy_land(Structure* map, int position, Player* player) {
    if (map == NULL || player == NULL || position < 0 || position >= HEIGHT * WIDTH) return;

    if (map -> type == '0' && map -> owner== NULL) { // Only unowned normal lands can be bought
        if (player_getMoney(player) >= map->money) {
            player_setMoney(player, player_getMoney(player) - map->money);
            map->owner = player;
            printf("%s bought land at position %d for %d .\n", player_getName(player->character), position, map->money);
        } else {
            printf("%s does not have enough money to buy this land.\n", player_getName(player->character));
        }
    }
    else {
        printf("This land cannot be bought.\n");
    }
}

void sell_land(Structure* map, int position, Player* player) {
    if (map == NULL || player == NULL || position < 0 || position >= HEIGHT * WIDTH) return;

    if (map -> owner == player) { // Only the owner can sell the land
        int sellPrice = map->money / 2; // Selling price is half of the purchase price
        player_setMoney(player, player_getMoney(player) + sellPrice);
        map->owner = NULL;
        map->level = 0; // Reset land level
        printf("%s sold land at position %d for %d .\n", player_getName(player->character), position, sellPrice);
    } else {
        printf("You do not own this land and cannot sell it.\n");
    }
}

void upgrade_land(Structure* map, int position, Player* player) {
    if (map == NULL || player == NULL || position < 0 || position >= HEIGHT * WIDTH) return;

    if (map -> owner == player) { // Only the owner can upgrade the land
        if (map->level < MAX_LEVEL) {
            int upgradeCost = map->money / 2 * (map->level + 1); // Upgrade cost increases with level
            if (player_getMoney(player) >= upgradeCost) {
                player_setMoney(player, player_getMoney(player) - upgradeCost);
                map->level++;
                printf("%s upgraded land at position %d to level %d for %d .\n", player_getName(player->character), position, map->level, upgradeCost);
            } else {
                printf("%s does not have enough money to upgrade this land.\n", player_getName(player->character));
            }
        } else {
            printf("This land is already at maximum level.\n");
        }
    } else {
        printf("You do not own this land and cannot upgrade it.\n");
    }
}

/*
void sell_all_land(Structure* map, Player* player) {
    if (map == NULL || player == NULL) return;

    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        if (map[i].owner == player) {
            int sellPrice = map[i].money / 2; // Selling price is half of the purchase price
            player_setMoney(player, player_getMoney(player) + sellPrice);
            map[i].owner = NULL;
            map[i].level = 0; // Reset land level
            printf("%s sold land at position %d for %d .\n", player_getName(player->character), map[i].id, sellPrice);
        }
    }
}
*/