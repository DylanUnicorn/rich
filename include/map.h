#define WIDTH 29
#define HEIGHT 8 
#define MAX_LEVEL 3

#include "Structure.h"

void init_map(Structure* map);
void print_map(Structure* map);
void buy_land(Structure* map, int position, Player* player);
void sell_land(Structure* map, int position, Player* player);
void upgrade_land(Structure* map, int position, Player* player);
