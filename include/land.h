#ifndef LAND_H
#define LAND_H
#include "map.h"
#include "Structure.h"

void buy_land(Structure* map, int position, Player* player);
void sell_land(Structure* map, int position, Player* player);
void upgrade_land(Structure* map, int position, Player* player);

#endif
