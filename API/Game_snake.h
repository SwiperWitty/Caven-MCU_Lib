#ifndef _GAME_SNAKE__H_
#define _GAME_SNAKE__H_

#include "caven_type.h"

typedef struct 
{
    int Length;                         // 蛇长
    int speed;
    uint16_t food_point_coordinates[2]; // 食物坐标
    uint16_t window_max_coordinates[2]; // 窗口最大坐标
    char advance_way;                   // 运动方向
    uint16_t snake_bady[100][2];        // 蛇的全部坐标
    char head_run_num;                  // 头在坐标的位置
    char eat_falg;

}Game_snake_Type;

void Game_snake_Init (int Set);
void Game_snake_Runing_Fun (char grade,char advance_way,int random);


#endif // !_GAME_SNAKE__H_
