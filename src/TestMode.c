#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TestMode.h"

#define CELL_NUMS 100
int  g_ended = 0, g_winner = -1;
//入口函数，开始测试模式，设置玩家，地图
//使用BeginTestMode( dir , &playerManager, map);读取dir下的preset.json文件
//解析到playerManager和map中
//使用SetGameEnded(ended,winner);设置游戏结束状态
//使用output_game_json( dir , &playerManager, map);将当前游戏状态输出到dir下的dump.json文件
int BeginTestMode(const char* filename,PlayerManager* pm,Structure* structures) {
    char json_path[300];
    snprintf(json_path, sizeof(json_path), "%s/preset.json", filename);
    load_game_from_json(json_path,pm,structures);
    return 0;
}
void SetGameEnded(int ended,int winner){
    g_ended = ended;
    g_winner = winner;
}

//根据名字获取角色枚举
PlayerCharacter get_character_enum(const char* name) {
    if (strcmp(name, "Q") == 0) return QIAN_FUREN;
    else if (strcmp(name, "A") == 0) return ATURBER;
    else if (strcmp(name, "S") == 0) return SUN_XIAOMEI;
    else if (strcmp(name, "J") == 0) return JIN_BEIBEI;
    else return QIAN_FUREN; //默认钱夫人
}

Player* get_map_owner(const char* name,PlayerManager* pm) {
    for(int i=0;i<pm->playerCount;i++){
        if(pm->players[i].character == get_character_enum(name)){
            return &pm->players[i];
        }
        else return NULL;
    }
    return NULL;
}

const char* get_name_from_enum(PlayerCharacter character) {
    switch (character) {
        case QIAN_FUREN:
            return "Q";
        case ATURBER:
            return "A";
        case SUN_XIAOMEI:
            return "S";
        case JIN_BEIBEI:
            return "J";
        default:
            return "?";
    }
}
int load_game_from_json(const char* filename, PlayerManager* pm, Structure* structures) {

    FILE* file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "无法打开文件 %s\n", filename);
		return -1;
	}
	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = (char*)malloc(len + 1);
	fread(buffer, 1, len, file);
	buffer[len] = '\0';
	fclose(file);

	cJSON* root = cJSON_Parse(buffer);
	free(buffer);
	if (!root) {
		fprintf(stderr, "JSON解析失败\n");
		return -1;
	}//解析JSON文件，解析结果-根节点 root
    
    //初始化玩家信息

    cJSON *players_json = cJSON_GetObjectItem(root, "players");  //players_json为玩家数组
    if (players_json && cJSON_IsArray(players_json)){
        pm->playerCount = cJSON_GetArraySize(players_json); // 设置总玩家数
        printf("加载玩家中：玩家总数设置为：%d\n",pm->playerCount);
        for (int i =0; i < pm->playerCount; i++){
            cJSON *player_json = cJSON_GetArrayItem(players_json, i);  // player_json为单个玩家对象
            printf("加载玩家 %d 信息中...\n", i + 1);
            if (player_json){

                pm->players[i].character = get_character_enum(cJSON_GetObjectItem(player_json, "name")->valuestring);
                //加载基础信息
                pm->players[i].money = cJSON_GetObjectItem(player_json, "fund")->valueint;
                printf("%d---money:%d\n",i,cJSON_GetObjectItem(player_json, "fund")->valueint);
                pm->players[i].bankruptcy = 1 - cJSON_GetObjectItem(player_json, "alive")->valueint;
                pm->players[i].position = cJSON_GetObjectItem(player_json, "location")->valueint;
                pm->players[i].points = cJSON_GetObjectItem(player_json, "credit")->valueint;
                //加载道具
                cJSON *prop_json = cJSON_GetObjectItem(player_json, "prop");
                if (prop_json){
                    pm->players[i].tool.roadblock = cJSON_GetObjectItem(prop_json, "barrier")->valueint;
                    pm->players[i].tool.doll = cJSON_GetObjectItem(prop_json, "robot")->valueint;
                    pm->players[i].tool.bomb = cJSON_GetObjectItem(prop_json, "bomb")->valueint;
                    pm->players[i].tool.total = pm->players[i].tool.roadblock + pm->players[i].tool.doll + pm->players[i].tool.bomb;
                }
                //加载buff
                cJSON *buff_json = cJSON_GetObjectItem(player_json, "buff");
                if (buff_json){
                    pm->players[i].god_bless_days = cJSON_GetObjectItem(buff_json, "god")->valueint;
                    pm->players[i].prison_days = cJSON_GetObjectItem(buff_json, "prison")->valueint;
                    pm->players[i].hospital_days = cJSON_GetObjectItem(buff_json, "hospital")->valueint;
                    if (pm->players[i].god_bless_days > 0)
                    {
                        pm->players[i].god = true;
                    }
                    if (pm->players[i].prison_days > 0)
                    {
                        pm->players[i].in_prison = true;
                    }
                    if (pm->players[i].hospital_days > 0)
                    {
                        pm->players[i].in_hospital = true;
                    }
                    
                }
            }
        }

    }
    printf("玩家信息加载完成\n");

    //初始化房产信息
    cJSON *houses_json = cJSON_GetObjectItem(root, "houses"); // houses_json为房产对象
    if (houses_json && cJSON_IsObject(houses_json)){
        cJSON *house_jSON = NULL;
        cJSON_ArrayForEach(house_jSON, houses_json) { // 遍历 houses_json 中的每个键值对
            int position = atoi(house_jSON->string); 
            if (position >= 0 && position < CELL_NUMS) { // 确保位置合法
                cJSON *house_data = house_jSON; // 值是房产的详细信息
                if (house_data && cJSON_IsObject(house_data)) {
                    // 加载房产的所有者
                    Player* owner_ptr = get_map_owner(cJSON_GetObjectItem(house_data, "owner")->valuestring,pm);
                    if (owner_ptr) {
                        structures[position].owner = owner_ptr;
                        owner_ptr->house[position] = 1;
                    }
                    // 加载房产的等级
                    int rank = cJSON_GetObjectItem(house_data, "level")->valueint;
                    structures[position].level = rank;
                }
            }
        }
    }
    printf("房产信息加载完成\n");

       //加载道具信息
    cJSON *placed_prop_json = cJSON_GetObjectItem(root, "placed_prop"); // placed_prop_json为道具对象
    if (placed_prop_json) {
        // 加载炸弹位置
        cJSON *bombs_json = cJSON_GetObjectItem(placed_prop_json, "bomb");
        if (bombs_json && cJSON_IsArray(bombs_json)) {
            for (int i = 0; i < cJSON_GetArraySize(bombs_json); i++) {
                int bomb_position = cJSON_GetArrayItem(bombs_json, i)->valueint;
                structures[bomb_position].tool = TOOL_BOMB;
            }
        }
    printf("炸弹信息加载完成\n");
        // 加载路障位置
        cJSON *barriers_json = cJSON_GetObjectItem(placed_prop_json, "barrier");
        if (barriers_json && cJSON_IsArray(barriers_json)) {
            for (int i = 0; i < cJSON_GetArraySize(barriers_json); i++) {
                int barrier_position = cJSON_GetArrayItem(barriers_json, i)->valueint;
                structures[barrier_position].tool = TOOL_BLOCK;
            }
        }
    }
    printf("路障信息加载完成\n");

    //加载游戏当前状态
    cJSON *game_json = cJSON_GetObjectItem(root, "game");
    if (game_json) {
        pm->currentPlayerIndex = cJSON_GetObjectItem(game_json, "now_player")->valueint + 1; 
    }
    printf("current_player:%d\n", pm->currentPlayerIndex);

    if(cJSON_GetObjectItem(game_json, "ended")->valueint == 1){
        printf("游戏已结束，获胜者为玩家 %d\n", cJSON_GetObjectItem(game_json, "winner")->valueint + 1);

        //游戏终止逻辑
        //...
    }

    //释放 JSON 对象
    cJSON_Delete(root);
    printf("预设文件加载完成\n");
    return 0;
}

cJSON* player_to_json(const Player* p, int idx) {
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(obj, "index", idx);
	// 输出基本信息
	cJSON_AddStringToObject(obj, "name", get_name_from_enum(p->character));
	cJSON_AddNumberToObject(obj, "fund", p->money);
	cJSON_AddNumberToObject(obj, "credit", p->points);
	cJSON_AddNumberToObject(obj, "location", p->position);
	cJSON_AddBoolToObject(obj, "alive", p->bankruptcy ? 0 : 1);
	// 输出道具信息
	cJSON* prop_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(prop_obj, "bomb", p->tool.bomb);
	cJSON_AddNumberToObject(prop_obj, "barrier", p->tool.roadblock);
	cJSON_AddNumberToObject(prop_obj, "robot", p->tool.doll);
	cJSON_AddNumberToObject(prop_obj, "total", p->tool.total);
	cJSON_AddItemToObject(obj, "prop", prop_obj);
	// 输出buff信息
	cJSON* buff_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(buff_obj, "god", p->god_bless_days);
	cJSON_AddNumberToObject(buff_obj, "prison", p->prison_days);
	cJSON_AddNumberToObject(buff_obj, "hospital", p->hospital_days);
	cJSON_AddItemToObject(obj, "buff", buff_obj);
	return obj;
}

void output_game_json(const char* filename,PlayerManager *pm, Structure* structures) {
	cJSON* root = cJSON_CreateObject();
	// players

    printf("正在输出玩家信息...\n");

	cJSON* players_arr = cJSON_CreateArray();
	for (int i = 0; i < pm->playerCount; ++i) {
		cJSON* player_obj = player_to_json(&pm->players[i], i);
		cJSON_AddItemToArray(players_arr, player_obj);
	}
	cJSON_AddItemToObject(root, "players", players_arr);
	// houses

    printf("正在输出房产信息...\n");

	cJSON* houses_obj = cJSON_CreateObject();
    for(int i=0;i<CELL_NUMS;i++){
        Player* owner_ptr = structures[i].owner;
        // 只有 owner_ptr 非 NULL 且角色合法才输出
        if(owner_ptr != NULL && (owner_ptr->character == QIAN_FUREN || owner_ptr->character == ATURBER || owner_ptr->character == SUN_XIAOMEI || owner_ptr->character == JIN_BEIBEI)){
            cJSON* house_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(house_obj, "owner", get_name_from_enum(owner_ptr->character));
            cJSON_AddNumberToObject(house_obj, "level", structures[i].level);
            char key[8];
            snprintf(key, sizeof(key), "%d", i);
            cJSON_AddItemToObject(houses_obj, key, house_obj);
            printf("房产 %d 信息已输出\n", i);
        }
    }
	// 如果输入没有房产，输出空对象
	cJSON_AddItemToObject(root, "houses", houses_obj);
	// placed_prop

    printf("正在输出道具信息...\n");

	cJSON* bomb_arr = cJSON_CreateArray();
	cJSON* barrier_arr = cJSON_CreateArray();
    for(int i=0;i<CELL_NUMS;i++){
        if(structures[i].tool == TOOL_BOMB){
            cJSON_AddItemToArray(bomb_arr, cJSON_CreateNumber(i));
        } else if(structures[i].tool == TOOL_BLOCK){
            cJSON_AddItemToArray(barrier_arr, cJSON_CreateNumber(i));
        }
    }
	cJSON* placed_prop_obj = cJSON_CreateObject();
	cJSON_AddItemToObject(placed_prop_obj, "bomb", bomb_arr);
	cJSON_AddItemToObject(placed_prop_obj, "barrier", barrier_arr);
	cJSON_AddItemToObject(root, "placed_prop", placed_prop_obj);
	// game

    printf("正在输出游戏状态信息...\n");

	cJSON* game_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(game_obj, "now_player", pm->currentPlayerIndex - 1);
    playerManager_nextPlayer(pm);
	cJSON_AddNumberToObject(game_obj, "next_player", pm->currentPlayerIndex - 1);
	cJSON_AddBoolToObject(game_obj, "ended", g_ended);
	cJSON_AddNumberToObject(game_obj, "winner", g_winner);
	cJSON_AddItemToObject(root, "game", game_obj);
	// 输出到文件
	char* json_str = cJSON_Print(root);

    char json_path[300];
    snprintf(json_path, sizeof(json_path), "%s/dump.json", filename);
	FILE* fp = fopen(json_path, "w");
	if (fp) {
		fputs(json_str, fp);
		fclose(fp);
		printf("已保存到 %s\n", json_path);
	} else {
		printf("无法写入 %s\n", json_path);
	}
	free(json_str);
	cJSON_Delete(root);
}