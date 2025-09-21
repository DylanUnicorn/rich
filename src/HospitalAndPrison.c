#include"HospitalAndPrison.h"

void InHospital(Player *player,Structure *structure){
    if(structure->type == 'H'){
    player->in_hospital = true;
    player->hospital_days = 3;
    player->position = structure->id;
    }
}


void InPrison(Player *player,Structure *structure){
    if(structure->type == 'P'){
    player->in_prison = true;
    player->prison_days = 3;
    player->position = structure->id;
    }
}


void HospitalAndPrisonReduceOne(Player *player){//外界调用，自动判断医院和监狱
    HospitalDayReduceOne(player);
    PrisonDayReduceOne(player);
}

void HospitalDayReduceOne(Player *player){//每回合检测是否在医院,请勿直接调用
    if(player->hospital_days>0){
        player->hospital_days -= 1;
        if(player->hospital_days == 0){
            player->in_hospital = false;
        }
    }
}

void PrisonDayReduceOne(Player *player){//每回合检测是否在监狱,请勿直接调用
    if(player->prison_days>0){
        player->prison_days -= 1;
        if(player->prison_days == 0){
            player->prison_days = false;
        }
    }
}