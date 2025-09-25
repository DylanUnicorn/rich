#include "Structure.h"
#include "map.h"
#include "ui.h"
#include "Player.h"
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
                map[i * WIDTH + j].id = -1;
            }
            map[i * WIDTH + j].owner = NULL; // No owner initially
            map[i * WIDTH + j].level = -1; // Initial land level
        }
    }
}

void print_map(Structure* map, Player* player){
    if (map == NULL) return;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if(map[i * WIDTH + j].owner != NULL){
                // If owned, display the owner's color code
                printf("%s%d%s ", ui_get_player_color(map[i * WIDTH + j].owner->character), 
                        map[i * WIDTH + j].level, COLOR_RESET); // Display land level with color
            }
            else{
                if(map[i * WIDTH + j].id == player->position){
                    // If the player is on this tile, display the player's color code
                    printf("%s%c%s ", ui_get_player_color(player->character), 
                        player_getColorCode(player->character), COLOR_RESET);
                }
                else {
                    printf("%c ", map[i * WIDTH + j].type);
                    //printf("%d ", map[i * WIDTH + j].id);
                }
            }
        }
        printf("\n");
    }
}

int find_place(Structure* map, int position) {
    if (map == NULL || position == -1) return -1;
    if (position < 0 || position >= HEIGHT * WIDTH) return -1;

    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        if (map[i].id == position) {
            return i;
        }
    }
    return -1; // Not found
}
