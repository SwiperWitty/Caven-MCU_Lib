#include "Game_snake.h"

Game_snake_Type Now_Game_snake;

int check_point_coordinates (Game_snake_point_Type check_data,Game_snake_point_Type *check_source,int Length);
void Game_snake_Init (int Set)
{
    if (Set)
    {
        memset(Now_Game_snake,0,sizeof(Game_snake_Type));
        Now_Game_snake.window_max.Coordinates_x = 240;
        Now_Game_snake.window_max.Coordinates_y = 240;
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

    }
    
}

int make_food_point_coordinates (Game_snake_Type *Game_snake_data)
{
    int retval = 0;
    int random;
    int temp_data;
    Game_snake_point_Type temp_snake_data;
    if (Game_snake_data != NULL)
    {
        do
        {
            random = rand();
            temp_snake_data.Coordinates_x= (random & 0xffff) % Now_Game_snake.window_max.Coordinates_x;
            temp_snake_data.Coordinates_y = ((random >> 16) & 0xffff) % Now_Game_snake.window_max.Coordinates_y;
            temp_data = check_point_coordinates (temp_snake_data,Game_snake_data->snake_bady,Now_Game_snake.Length);
        } while (temp_data == 0);
        temp_snake_data.Color = 0x03;
        temp_snake_data.Stats = 0x02;
        Game_snake_data->food = temp_snake_data;
    }
    return retval;
}

int snake_runing_handle (Game_snake_Type *Game_snake_data)
{
    int retval = 0;
    int temp_data;
    Game_snake_point_Type temp_snake_data;
    if (Game_snake_data != NULL)
    {
        Game_snake_data->Length ++;
        if (Game_snake_data->Length > 30)
        {
            Game_snake_data->Length = 30;
        }
        
        temp_snake_data.Color = 0x01;
        temp_snake_data.Stats = 0x02;
        Game_snake_data->food = temp_snake_data;
    }
    return retval;
}

int check_point_coordinates (Game_snake_point_Type check_data,Game_snake_point_Type *check_source,int Length)
{
    int retval = 0;
    if (check_source != NULL)
    {
        return;
    }
    for (int i = 0; i < Length; i ++)
    {
        if (memcmp(&check_data,&check_source[i],size_data) == 0)
        {
            retval = i;
            break;
        }
    }
    
    return retval;
}

