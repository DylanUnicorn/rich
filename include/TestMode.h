#ifndef TESTMODE_H
#define TESTMODE_H

#include "cJSON.h"
#include "Player.h"
#include "Structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int BeginTestMode(const char* filename,PlayerManager* pm,Structure* structures);
void SetGameEnded(int ended,int winner);
PlayerCharacter get_character_enum(const char* name);
Player* get_map_owner(const char* name,PlayerManager* pm);
const char *get_name_from_enum(PlayerCharacter character);
int load_game_from_json(const char* filename, PlayerManager* pm, Structure* structures);
cJSON* player_to_json(const Player* p, int idx);
void output_game_json(const char* filename,PlayerManager *pm, Structure* structures);
#endif