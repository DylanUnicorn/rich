#include "GameLogic.h"
#include "Player.h"
#include "Structure.h"
#include "prophouse.h"
#include "gifthouse.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
# include"map.h"
# include"Tool.h"
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

// 财神相关逻辑封装
void god_disappear(Structure* map, int* god_pos, int* god_turn) {
    if (*god_pos != -1) {
        printf("财神消失了！位置：%d\n", *god_pos);
        map[find_place(map, *god_pos)].tool = 0;
        if(map[find_place(map, *god_pos)].id == 0 ){
            map[find_place(map, *god_pos)].type = 'S'; // 恢复为开始
        }
        else if(map[find_place(map, *god_pos)].id == 14 || map[find_place(map, *god_pos)].id == 63 || 
                map[find_place(map, *god_pos)].id == 49){
            map[find_place(map, *god_pos)].type = 'P'; // 恢复为公园
        }
        else if(map[find_place(map, *god_pos)].id == 35){
            map[find_place(map, *god_pos)].type = 'G'; // 恢复为礼品屋
        }
        else if(map[find_place(map, *god_pos)].id == 28){
            map[find_place(map, *god_pos)].type = 'T'; // 恢复为道具屋
        }
        else if(map[find_place(map, *god_pos)].id == 64 || map[find_place(map, *god_pos)].id == 65 || 
                map[find_place(map, *god_pos)].id == 66 || map[find_place(map, *god_pos)].id == 67 || 
                map[find_place(map, *god_pos)].id == 68 || map[find_place(map, *god_pos)].id == 69){
            map[find_place(map, *god_pos)].type = '$'; // 恢复为矿地
        }
        else{
            map[find_place(map, *god_pos)].type = '0'; // 恢复为普通地块
        }
        *god_pos = -1;
        *god_turn = 0;
    }
}

void god_refresh(Structure* map, PlayerManager* pm, int* god_pos, int* god_turn, bool* god_used, int* game_turns) {
    int new_god_pos = rand() % 70;
    while (1) {
        int occupied = 0;
        for (int i = 0; i < pm->playerCount; i++) {
            Player* p = &pm->players[i];
            if (p->position == new_god_pos) {
                occupied = 1;
                break;
            } else {
                int idx = find_place(map, new_god_pos);
                if (map[idx].tool != 0) { occupied = 1; break; }
                char t = map[idx].type;
                if (t == 'G' || t == 'T' || t == '@' || t == '#') { occupied = 1; break; }
            }
        }
        if (!occupied) break;
        new_god_pos = rand() % 70;
        
    }
    *god_pos = new_god_pos;
    map[find_place(map, *god_pos)].tool = 1;
    map[find_place(map, *god_pos)].type = 'F'; // 'F'表示财神位置
    printf("财神已出现，位置 %d ！[DEBUG] tool已设为1\n", *god_pos);
    *game_turns = 0;
    *god_turn = 1;
    *god_used = 0;
}

void game_handle_turn(int* god_pos, int* god_turn, bool* god_used, int* game_turns, Structure* map, PlayerManager* playermanager) {
    printf("[DEBUG] god_turn=%d, god_pos=%d, god_used=%d, game_turns=%d\n", *god_turn, *god_pos, *god_used, *game_turns);
    if(!*god_used && *god_pos != -1 ){
        (*god_turn)++;
        printf("[DEBUG] 财神未被使用，god_turn++，当前god_turn=%d\n", *god_turn);
        if (*god_turn == 5+1){
            god_disappear(map, god_pos, god_turn);
            *game_turns = (rand() % 10) + 1; // 财神消失后10回合内随机掉落
            printf("[DEBUG] 财神消失，冷却开始，冷却回合：%d\n", *game_turns);
        }
    } else if(*god_used){
        printf("[DEBUG] 财神被使用，重置god_used和god_turn\n");
        *god_used = false;
        *god_turn = 0;
        god_disappear(map, god_pos, god_turn);
    *game_turns = (rand() % 10) + 1; // 财神被使用后10回合内随机掉落
    printf("[DEBUG] 财神被使用，冷却开始，冷却回合：%d\n", *game_turns);
    }

    // 冷却为0且未有财神时才刷新
    if (*god_pos == -1 && *game_turns == 0) {
        // 刷新财神
        god_refresh(map, playermanager, god_pos, god_turn, god_used, game_turns);
        printf("[DEBUG] 财神刷新成功\n");
    }
    
}
void game_handle_cell_event(Player* player, Structure* cell, PlayerManager* player_manager) {
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