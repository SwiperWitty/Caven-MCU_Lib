#ifndef __MLX90614_H
#define __MLX90614_H

//PB10   SCL
//PB11   SDA

#include "stdint.h"
#include "Items.h"
#if Exist_MLX90614
	#include "Base.h"
#endif


#define	IIC_Speed 3
//
//#define IIC_Read(ack_nack, Speed)	IICs_ReadByte(ack_nack, Speed)
//#define IIC_Send(data, Speed)		IICs_SendByte(data, Speed)
//#define IIC_Init(Set)	IIC_Software_Init(Set)
//
//void Mlx90614_Init(void);
//float Mlx90614_ReadTemp(void);



#endif

