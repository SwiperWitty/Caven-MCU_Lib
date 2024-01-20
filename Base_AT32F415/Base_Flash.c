#include "Base_Flash.h"

#include "Caven_Type.h"              //MAX,MIN


/*
    符合地址大小 ≥ 0,且返回扇区
    不符合 (-1)
*/

int Addr_Get_Area(int Addr)
{
    int retval = FLASH_OPERATE_DONE;
#ifdef Exist_FLASH 
    if((Addr >= FLASH_END) || (Addr < FLASH_START))
    { 
//        printf("S-ERROR\r\n"); 
        retval = (-1);
        return retval;
    }
    retval = (Addr - FLASH_START) / FLASH_AREA_SIZE;

#endif
    return retval;
}

int Flash_Clear_Area(int Addr,int size)
{
    int retval = FLASH_OPERATE_DONE;
#ifdef Exist_FLASH 
    int StartSector, EndSector;
    StartSector = Addr_Get_Area(Addr);
    EndSector = Addr_Get_Area(Addr + size);
    if(StartSector == (-1) || EndSector == (-1))
    {
        retval = ERROR;
        return retval;
    }
    flash_unlock();
    for(int i = StartSector;i <= EndSector;i++)
    {
        /* wait for operation to be completed */
        retval = flash_operation_wait_for(ERASE_TIMEOUT);
        if((retval == FLASH_PROGRAM_ERROR) || (retval == FLASH_EPP_ERROR))
        {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
        else if(retval == FLASH_OPERATE_TIMEOUT)
        {return ERROR;}
        
        //Clear
        retval = flash_sector_erase(GET_Area_Addr(i));
        if(retval != FLASH_OPERATE_DONE)
        {
//            printf("p: Clr ERROR \r\n");
            return ERROR;
        }
        else
        {
//            printf("p: Clr secc %d \r\n",i);
        }
    }
    flash_lock();
#endif
    return retval;
}

int Flash_Save_Data (int Addr,void *Data,int size)
{
    int retval = SUCCESS;
#ifdef Exist_FLASH 
    char status;
    int array[0x200];         // 缓冲区(给写区域) 
    int size_array = sizeof(array);
    int temp;
    int StartSector, EndSector;
    StartSector = Addr_Get_Area(Addr);
    EndSector = Addr_Get_Area(Addr + size);
    if(StartSector == (-1) || EndSector == (-1) || Data == NULL)
    {
        return -1;
    }
    flash_unlock();
    for(int i = StartSector;i <= EndSector;i++)
    {
        /* wait for operation to be completed */
        status = flash_operation_wait_for(ERASE_TIMEOUT);
        if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
        {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
        else if(status == FLASH_OPERATE_TIMEOUT)
        {return -1;}
        //Clear
        status = flash_sector_erase(GET_Area_Addr(i));      
        if(status != FLASH_OPERATE_DONE)
        {
//            printf("p: Clr ERROR \r\n");
            return -1;
        }
    }
    /* write data_32 to the corresponding address */
    for(int i = 0; i < size;)
    {
        if(*(__IO int *)Addr != EMPTY_DATA)     //不能写
        {
            return -1;
        }
        if((size - i)/size_array)
        {temp = size_array;}
        else
        {temp = size - i;}                      //本次需要写入的数量
        
        memset(array,0,size_array);
        memcpy(array,(char *)Data + i,temp);
        int k = 0;
        for(int j = 0;j < temp;)
        {
            if(FLASH_OPERATE_DONE == flash_word_program(Addr, array[k++]))      //32bit
            {
                Addr = Addr + 4;    //more
                j += 4;
            }else
            {
                while(1);
            }
        }
        i += temp;
    }
    flash_lock();
#endif
    return retval;
}

//char Flash_Save_Area (void * Data,char Area_Start,int Lenght)
//{
//    char back = SUCCESS;
//#ifdef Exist_FLASH 
//    char Buff[FLASH_AREA_SIZE];         //缓冲区      而且要
//    char status = FLASH_OPERATE_DONE;
//    memcpy(Buff,Data,Lenght);           //向前对齐
//    status = Flash_Addr_Check(GET_Area_Addr(Area_Start),GET_Area_Addr(Area_Start));
//    if(status == FLASH_PROGRAM_ERROR) {return ERROR;}
//    
//    flash_unlock();
//    status = flash_operation_wait_for(ERASE_TIMEOUT);
//    if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
//    {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
//    else if(status == FLASH_OPERATE_TIMEOUT)
//    {return ERROR;}
//    
//    status = flash_sector_erase(GET_Area_Addr(Area_Start));         //清除一区
//    if(status != FLASH_OPERATE_DONE)
//    {return ERROR;}
//    
//    for(int i = 0;i < FLASH_AREA_SIZE;i++)                          //保存
//    {
//        status = flash_byte_program(GET_Area_Addr(Area_Start)+i, Buff[i]);
//        if(status !=  FLASH_OPERATE_DONE)
//        {return ERROR;}
//    }
//    
//    flash_lock();
//#endif
//    return back;
//}

//char Flash_Save_Data(int Addr,const uint16_t *Data,int Lenght)
//{
//	char back = SUCCESS;
//#ifdef Exist_FLASH 
//    uint16_t flash_buf[FLASH_AREA_SIZE];        //缓冲区      而且要
//    char status = FLASH_OPERATE_DONE;
//    uint32_t Temp;
//    
//    if(Addr%2)  Addr++;
//    int Addr_Start = Addr;          
//    int Addr_End = Addr + Lenght;   //
//    
//    status = Flash_Addr_Check(Addr_Start,Addr_End); //11
//    if(status == FLASH_PROGRAM_ERROR)
//    {return ERROR; }
//    
//    Temp = (Addr_Start - FLASH_START) / FLASH_AREA_SIZE;
//    char Area_Start = Temp;                     //首页 
//    
//    Temp = ((Addr_End - FLASH_START) / FLASH_AREA_SIZE);
//    if(Temp * FLASH_AREA_SIZE < Addr_End)
//    {Temp++;}
//    char Area_End = Temp;          //尾页 

//    int Sour_NUM = 0;               //源(Data)
//    int Targ_NUM = 0;               //缓冲区
//    int Cpy_num = 0;                //复制数
//    
//    for(int i = Area_Start;i < Area_End;i++)
//    {
//        //开始数据处理
//        Temp = GET_Area_Addr(i);
//        back = Flash_Read_Data (Temp,flash_buf,FLASH_AREA_SIZE);     //Flash备份
//        if(back == 0) 
//        {return ERROR;}
//        
//        Targ_NUM = 0;
//        if(Temp < Addr_Start)        //第一页  覆盖数据
//        {
//            Targ_NUM = (Addr_Start - Temp);
//            Cpy_num = FLASH_AREA_SIZE - Targ_NUM;
//            memcpy((flash_buf+Targ_NUM/2),(Data + Sour_NUM/2),Cpy_num);       //注意数据类型
////            printf("1-Move_Targ(u16): %x   Temp_source(u16): %x  num: %x\r\n",Targ_NUM/2,Sour_NUM/2,Cpy_num);

//            Sour_NUM += Cpy_num; 
//        }
//        else if(GET_Area_Addr(i+1) > Addr_End)        //最后一页
//        {
//            Cpy_num = Addr_End - Temp;
//            if(Cpy_num == 0)
//            {return SUCCESS;}              //结束
//            memcpy((flash_buf+Targ_NUM/2),(Data + Sour_NUM/2),Cpy_num);       //注意数据类型
////            printf("2-Move_Targ(u16): %x   Temp_source(u16): %x  num: %x\r\n",Targ_NUM/2,Sour_NUM/2,Cpy_num);
//            
////            Sour_NUM += Temp;
//        }
//        else
//        {
//            Cpy_num = FLASH_AREA_SIZE;
//            memcpy((flash_buf+Targ_NUM/2),(Data + Sour_NUM/2),Cpy_num);       //注意数据类型
////            printf("3-Move_Targ(u16): %x   Temp_source(u16): %x  num: %x\r\n",Targ_NUM/2,Sour_NUM/2,Cpy_num);
//            
//            Sour_NUM += Cpy_num;
//        }
//        status = flash_operation_wait_for(ERASE_TIMEOUT);
//        if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
//        {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
//        else if(status == FLASH_OPERATE_TIMEOUT)
//        {return ERROR; }
//        
//        back = Flash_Clear_Area(i,i + 1);                                   //擦一页
//        if(back == 0) 
//        {return ERROR; } 
////        printf("AREA : %d  Flash : %x ; wirte size : %x\r\n",i,GET_Area_Addr(i),FLASH_AREA_SIZE);
//        
//        flash_unlock();
//        
//        for(int i = 0;i < FLASH_AREA_SIZE/2;i++)        //覆盖保存数据
//        {
////            printf("%x ",flash_buf[i]);
//            status = flash_halfword_program(Temp, flash_buf[i]);    

//            if(status == FLASH_PROGRAM_ERROR)
//            {   
////                printf("W ERROR \r\n ");
//                return ERROR; 
//            }
//            
//            flash_buf[i] = 0;
//            Temp += 2;
//        }
//        flash_lock();

//    }
//#endif
//    return back;
//}


int Flash_Read_Data (int Addr,void *Data,int size)
{
	int retval = SUCCESS;
	
#ifdef Exist_FLASH 
	char temp;
    int StartSector, EndSector;
    StartSector = Addr_Get_Area(Addr);
    EndSector = Addr_Get_Area(Addr + size);
    if(StartSector == (-1) || EndSector == (-1) || Data == NULL)
    {
        retval = ERROR;
        return retval;
    }
    for(int i = 0; i < size; i++)
    {
        temp = *((__IO char*)Addr + i);
        *((char *)Data + i) = temp;
    }
#endif
    return retval;
}

