#include"TollFee.h"
#include"Player.h"
#include<stdio.h>
void GetTollFee(Player *player, Structure *map, PlayerManager *playermanager){
    if(player != map->owner ){
        player->money -= (map->level+1)*(map->money)*0.5;
        map->owner->money += (map->level)*(map->money)*0.5;
        if(player_isBankrupt(player)){
            IBankruptcy(player,playermanager);
        }
    }//路人过路费为（房产等级+1）乘以基础金额乘以0.5
}


void IBankruptcy(Player *player,PlayerManager *playermanager){
    if (player == NULL || playermanager == NULL) return;
    printf("%s 破产!\n\n",player_getName(player->character));
    playerManager_removeBankruptPlayer(playermanager, playerManager_getCurrentPlayerIndex(playermanager)); //player->character-1为玩家索引
}


