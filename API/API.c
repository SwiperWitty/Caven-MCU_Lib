#include "API.h"

struct API_Data_ API;


void API_Index(void)
{
    API.Get_CRC16 = CRC16_CCITT_CalculateBuf;


    
    API.Data.Find_Data = Find_Data;

}
