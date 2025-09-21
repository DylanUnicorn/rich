#include "Structure.h"
#include "map.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void init_map(Structure* map){
    if (map == NULL) return;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if(i == 0){
                if(j == 0) {
                    map[i * WIDTH + j].type = 'S'; // Start
                    map[i * WIDTH + j].id = 0;
                }
                else if(j == 14) {
                    map[i * WIDTH + j].type = 'H'; // Hospital
                    map[i * WIDTH + j].id = 14;
                }
                else if(j == 28) {
                    map[i * WIDTH + j].type = 'T'; // Tool
                    map[i * WIDTH + j].id = 28;
                }
                else {
                    map[i * WIDTH + j].type = '0'; // Normal
                    map[i * WIDTH + j].id = j;
                    map[i * WIDTH + j].money = 200; // Land pricing
                }
            }   
            else if(i == 7){
                if(j == 0) {
                    map[i * WIDTH + j].type = 'M'; // Magic
                    map[i * WIDTH + j].id = 63;
                }
                else if(j == 14) {
                    map[i * WIDTH + j].type = 'P'; // Prison
                    map[i * WIDTH + j].id = 49;
                }
                else if(j == 28) {
                    map[i * WIDTH + j].type = 'G'; // Gift
                    map[i * WIDTH + j].id = 35;
                }
                else {
                    map[i * WIDTH + j].type = '0'; // Normal
                    map[i * WIDTH + j].id = 63 - j;
                    map[i * WIDTH + j].money = 300; // Land pricing
                }
            } 
            else if(j == 0){
                if(i != 0 && i != 7) {
                    map[i * WIDTH + j].type = '$'; // Magic
                    map[i * WIDTH + j].id = 70 - i;
                }
            } 
            else if(j == 28){
                if(i != 0 && i != 7) {
                    map[i * WIDTH + j].type = '0'; // Normal
                    map[i * WIDTH + j].id = 28 + i;
                    map[i * WIDTH + j].money = 500; // Land pricing
                }
            } 
            else {  
                map[i * WIDTH + j].type = ' '; // Non-playable area
            }
        }
    }
}

void print_map(Structure* map){
    if (map == NULL) return;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c ", map[i * WIDTH + j].type);
        }
        printf("\n");
    }
}

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

