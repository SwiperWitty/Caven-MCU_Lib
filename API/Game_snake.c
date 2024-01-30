#include "Game_snake.h"

Game_snake_Type Now_Game_snake;

int check_point_coordinates (void *check_data,void *check_source,int size_data,int size_source);

void Game_snake_Init (int Set)
{
    if (Set)
    {
        memset(Now_Game_snake,0,sizeof(Game_snake_Type));
        Now_Game_snake.window_max_coordinates[0] = 240;
        Now_Game_snake.window_max_coordinates[1] = 240;
    }
    
}

void Game_snake_Runing_Fun (char grade,char advance_way,int random)
{
    uint16_t head_point_coordinates[2];
    char temp_num;
    temp_num = Now_Game_snake.head_run_num;
    memcpy(head_point_coordinates,Now_Game_snake.snake_bady[temp_num],sizeof(head_point_coordinates));
    if (advance_way != 0)
    {
        Now_Game_snake.advance_way == advance_way;      // 1左，2右，3上，4下 
    }
    if(Now_Game_snake.eat_falg == 1)
    {
        check_point_coordinates (void *check_data,void *check_source,int size_data,int size_source);
        Now_Game_snake.food_point_coordinates[0] = random & 0xffff;
        Now_Game_snake.food_point_coordinates[1] = (random >> 16) & 0xffff;
    }
    
}

int check_point_coordinates (void *check_data,void *check_source,int size_data,int size_source)
{
    int retval = 0;
    if (size_source < size_data)
    {
        return;
    }
    for (int i = 0; i < size_source; i += size_data)
    {
        if (memcmp(check_data,(check_source + i),size_data) == 0)
        {
            retval = i;
        }
    }
    
    return retval;
}

