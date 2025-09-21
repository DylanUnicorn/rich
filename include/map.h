#ifndef MAP_H
#define MAP_H
#define WIDTH 29
#define HEIGHT 8 
#define MAX_LEVEL 3

#include "Structure.h"

void init_map(Structure* map);
void print_map(Structure* map);
int find_place(Structure* map, Player* player);

#endif