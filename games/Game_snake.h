#ifndef _GAME_SNAKE__H_
#define _GAME_SNAKE__H_

#include "caven_type.h"

typedef struct 
{
    uint16_t Coordinates_x;
    uint16_t Coordinates_y;
    uint16_t Color;
    uint16_t Stats;
}Game_snake_point_Type;

typedef struct 
{
    int Length;                    
    int speed;
    char advance_way;                   // 运动方向

    Game_snake_point_Type food;         // 食物坐标
    Game_snake_point_Type window_max;   // 窗口最大坐标
    Game_snake_point_Type bady[100];    // 蛇的全部坐标
    char eat_falg;

}Game_snake_Type;

void Game_snake_Init (int Set);
void Game_snake_Runing_Fun (char grade,char advance_way,int random);


#endif // !_GAME_SNAKE__H_
