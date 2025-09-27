#include "TollFee.h"
#include "Player.h"
#include "Structure.h"
#include "map.h"
#include "land.h"
#include <stdio.h>

void GetTollFee(Player *player, Structure *map){
    if(player != map->owner ){
        if (player->god){
            printf("财神附体，可免过路费。\n");
            return;
        }
        else if(map->owner->in_hospital){
            printf("%s在医院中，免收过路费。\n",player_getName(map->owner->character));
            return;
        }
        else if(map->owner->in_prison){
            printf("%s在监狱中，免收过路费。\n",player_getName(map->owner->character));
            return;
        }
        else{
            printf("你需要支付过路费。\n");
        player->money -= (map->level+1)*(map->money)*0.5;
        map->owner->money += (map->level+1)*(map->money)*0.5; //路人过路费为（房产等级+1）乘以基础金额乘以0.5
        }
        
    }
}


void IBankruptcy(Player *player,PlayerManager *playermanager,Structure *map){
    if (player == NULL || playermanager == NULL) return;
    printf("%s 破产!\n",player_getName(player->character));
        // 清除玩家拥有的房产
    reset_all_land(map, player);
    player->bankruptcy = true; // 标记玩家为破产状态
    playermanager->playerIndex--;
}
