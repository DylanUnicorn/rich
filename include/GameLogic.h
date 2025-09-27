#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include "Player.h"
#include "Structure.h"
void game_handle_cell_event(Player* player, Structure* cell, PlayerManager* player_manager);
void game_buy_property(Player* player, Structure* property);
void game_upgrade_property(Player* player, Structure* property);
void game_pay_toll(Player* player, Structure* property, PlayerManager* player_manager);
void game_handle_tool_shop(Player* player);
void game_handle_gift_house(Player* player);
void game_handle_magic_house(Player* player, PlayerManager* player_manager, void* map);
void game_handle_hospital(Player* player);
void game_handle_jail(Player* player);  
void god_disappear(Structure* map, int* god_pos, int* god_turn);
void god_refresh(Structure* map, PlayerManager* pm, int* god_pos, int* god_turn, bool* god_used, int* game_turns);
void game_handle_turn(int* god_pos, int* god_turn, bool* god_used, int* game_turns, Structure* map, PlayerManager* playermanager);
#endif