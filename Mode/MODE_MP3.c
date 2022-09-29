#include "MODE_MP3.h"

U8 Array_mp3[8];

void Voice_Init(void)
{
	//Uart2_Init(Voice_Baud,ENABLE);
}

void Voice(char Model,char Num)
{
	if(Model == JQ8900)
	{
		Array_mp3[0] = 0xAA;
		Array_mp3[1] = 0x07;
		Array_mp3[2] = 0x02;
		Array_mp3[3] = 0x00;
		Array_mp3[4] = Num;
		Array_mp3[5] = 0xB3+Num;
        for(int n = 0;n < 6;n++)
        {
            //UART_TXD_Send(2,Array_mp3[n]);
        }
	}
	else if(Model == JQ6500)
	{
		Array_mp3[0] = 0x7E;
		Array_mp3[1] = 0x04;
		Array_mp3[2] = 0x03;
		Array_mp3[3] = 0x00;
		Array_mp3[4] = Num;
		Array_mp3[5] = 0xEF;
        for(int n = 0;n < 6;n++)
        {
            //UART_TXD_Send(2,Array_mp3[n]);
        }
	}
	else
		return;
}
