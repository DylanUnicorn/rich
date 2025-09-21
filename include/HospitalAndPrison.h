#ifndef HOSPITALANDPRISON_H
#define HOSPITALANDPRISON_H

#include"Player.h"
#include"Structure.h"

void InHospital(Player *player,Structure *structure);
void InPrison(Player *player,Structure *structure);
void HospitalAndPrisonReduceOne(Player *player);
void HospitalDayReduceOne(Player *player);
void PrisonDayReduceOne(Player *player);

#endif