// GameLogic.c
#include "GameLogic.h"
#include "Player.h"
#include "Structure.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Tool.h"

// 处理地块事件
void game_handle_cell_event(Player* player, Structure* cell, PlayerManager* player_manager) {
    if (player == NULL || cell == NULL) return;

    printf("停留在了%c\n", cell->type);

    switch (cell->type) {
        case 'P':
            if (cell->owner == NULL) {
                // 空地，可以购买
                printf("此处为空地，价值%d元，是否购买？(Y/N): ", cell->money);
                char response = getchar();
                getchar(); // 消耗换行符
                if (response == 'Y' || response == 'y') {
                    game_buy_property(player, cell);
                }
            } else if (cell->owner == player) {
                // 自己的房产，可以升级
                printf("是否升级该房产？升级费用为%d元 (Y/N): ", cell->money);
                char response = getchar();
                getchar(); // 消耗换行符
                if (response == 'Y' || response == 'y') {
                    game_upgrade_property(player, cell);
                }
            } else {
                // 他人的房产，需要支付过路费
                game_pay_toll(player, cell, player_manager);
            }
            break;

        case 'T':
            game_handle_tool_shop(player);
            break;

        case 'G':
            game_handle_gift_house(player);
            break;

        case 'M':
            game_handle_magic_house(player, player_manager, NULL); // 需要传递map参数
            break;

        case 'H':
            game_handle_hospital(player);
            break;

        case 'J':
            game_handle_jail(player);
            break;

        default:
            break;
    }

    // 检查地块上是否有道具
    if (cell->tool.total > 0 ) {
        if (cell->tool.bomb > 0) {
            printf("触发了炸弹！");
            player->in_hospital = true;
            player->hospital_days = 3;
            cell->tool.bomb--;
            printf("被炸伤，住院3天！\n");
        } else if (cell->tool.roadblock > 0) {
            printf("遇到了路障！\n");
            cell->tool.roadblock--;
        }
    }
}

// 购买房产


// 升级房产


// 出售房产


// 支付过路费


// 处理道具屋事件
void game_handle_tool_shop(Player* player) {
    if (player == NULL) return;

    printf("欢迎光临道具屋，请选择您所需要的道具：\n");
    printf("1. 路障（200点）\n");
    printf("2. 机器娃娃（300点）\n");
    printf("3. 炸弹（400点）\n");
    printf("输入道具编号购买，或输入f退出：");

    char choice;
    int result;

    while (1) {
        result = scanf(" %c", &choice);
        if (result != 1) {
            while (getchar() != '\n');
            printf("输入无效，请重试。\n");
            printf("输入道具编号购买，或输入f退出：");
            continue;
        }

        if (choice == 'f' || choice == 'F') {
            printf("退出道具屋。\n");
            break;
        } else if (choice == '1') {
            if (player->points >= 200) {
                player->tool.roadblock++;
                player->points -= 200;
                player->tool.total++;
                printf("购买路障成功，剩余点数：%d\n", player->points);
            } else {
                printf("点数不足，无法购买路障。\n");
            }
        } else if (choice == '2') {
            if (player->points >= 300) {
                player->tool.doll++;
                player->points -= 300;
                player->tool.total++;
                printf("购买机器娃娃成功，剩余点数：%d\n", player->points);
            } else {
                printf("点数不足，无法购买机器娃娃。\n");
            }
        } else if (choice == '3') {
            if (player->points >= 400) {
                player->tool.bomb++;
                player->points -= 400;
                player->tool.total++;
                printf("购买炸弹成功，剩余点数：%d\n", player->points);
            } else {
                printf("点数不足，无法购买炸弹。\n");
            }
        } else {
            printf("无效选择，请重新输入。\n");
        }
        printf("输入道具编号购买，或输入f退出：");
    }
}

// 处理礼品屋事件
void game_handle_gift_house(Player* player) {
    if (player == NULL) return;

    printf("欢迎光临礼品屋，请选择一件您喜欢的礼品：\n");
    printf("1. 奖金 - 2000元\n");
    printf("2. 点数卡 - 200点\n");
    printf("3. 财神 - 5轮内免过路费\n");

    char choice;
    scanf("%c", &choice);
    getchar(); // 消耗换行符

    switch (choice) {
        case '1':
            player->money += 2000;
            printf("获得2000元奖金！\n");
            break;

        case '2':
            player->points += 200;
            printf("获得200点！\n");
            break;

        case '3':
            player->god = true;
            player->multiplier_days = 5;
            printf("获得财神附身，5轮内免过路费！\n");
            break;

        default:
            printf("无效选择！\n");
            break;
    }
}

// 处理魔法屋事件
void game_handle_magic_house(Player* player, PlayerManager* player_manager, void* map) {
    if (player == NULL) return;

    printf("欢迎光临魔法屋，魔法施展中...\n");
    // 可根据实际需求实现魔法效果
}

// 处理医院事件
void game_handle_hospital(Player* player) {
    if (player == NULL) return;

    if (player->in_hospital) {
        printf("住院中，还剩%d轮\n", player->hospital_days);
        player->hospital_days--;
        if (player->hospital_days <= 0) {
            player->in_hospital = false;
            printf("出院了！\n");
        }
    } else {
        printf("路过医院\n");
    }
}

// 处理监狱事件
void game_handle_jail(Player* player) {
    if (player == NULL) return;

    if (player->in_prison) {
        printf("服刑中，还剩%d轮\n", player->prison_days);
        player->prison_days--;
        if (player->prison_days <= 0) {
            player->in_prison = false;
            printf("刑满释放！\n");
        }
    } else {
        printf("路过监狱\n");
    }
}