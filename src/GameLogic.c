#include "GameLogic.h"
#include "Player.h"
#include "Structure.h"
#include "prophouse.h"
#include "gifthouse.h"
void game_handle_jail(Player* player) {
    if (player == NULL) return;
    player->in_prison = true;
    player->prison_days = 2; // Example: stay for 2 turns
}


void game_handle_tool_shop(Player* player) {
    if (player == NULL) return;
    // Example: allow player to buy a tool if they have enough money
    player_getintoPropHouse(player);
}

void game_handle_gift_house(Player* player) {
    if (player == NULL) return;
    // Example: give player a random gift (money, points, etc.)
    player_getintoGiftHouse(player);
}

void game_handle_cell_event(Player* player, Structure* cell, PlayerManager* player_manager){
    if (cell == NULL || player == NULL || player_manager == NULL) return;

    switch (cell->type) {
        // case 'H': // Hospital
        //  printf("此处为医院。\n");  
        // break;
        // case 'P': // Prison
        //     game_handle_jail(player);
        //     break;
        case 'G': // Gift House
            game_handle_gift_house(player);

            break;
        case 'T': // Tool Shop
            if(player->points < 30){
                printf("点数不足，无法进入道具屋。\n");
                break;
                if(player->points < 30 ){
                    printf("点数不足，无法进入道具屋。\n");
                    break;
                }
            }
            game_handle_tool_shop(player);
            break;
        case '$':
            printf("此处为矿地。\n");
            player->points += cell->points;
            printf("你获得了 %d 点数！\n", cell->points);
            break;
        case 'P':
            printf("此处为公园。\n");
            break;
        default:
            // Handle other cell types if necessary
            break;
    }
}