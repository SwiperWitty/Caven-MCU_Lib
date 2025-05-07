#ifndef _MODE_BYJ_H__
#define _MODE_BYJ_H__

#include "Base.h"
#include "Caven_Type.h"

/*
    28BJY48(28表示电机直径28毫米，B表示步进电机，Y表示永磁，J表示带减速箱48表示可以四拍和八拍运行)
    
*/
#define Reduction 512 

void Motor_BYJ_Init(int Set);
char Motor_BYJ_Drive(char Rotation,char Mode,int Code);


#endif
