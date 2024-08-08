#include "Encrypt_crc.h"
#include "string.h"
#include "stdio.h"

#define CRC_16_CCITT 0x1021

/*CRC校验表*/
static const uint16_t CRC_XMODEM_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

/******函数名:CRC16_XMODEM_Table_Byte() *********************
 *
 *   功能描述:
 *       这个函数为系统的CRC16的计算函数，将给定的CRC值和要
 *       计算的数据做CRC计算并将计算结果赋予前面给定的CRC值。
 *       计算公式：
 *       	 CRC_16= x^16 + x^12 + x^5 + 1
 *				查表
 *          多项式:1021,初始:0000,结果异或:0000,高字节在前。
 *   调用参数:
 *       CheckByte       要计算的数据
 *		LastCRC 上次计算的CRC值
 *   返回值:
 *       函数新计算的CRC值
 *   函数代码:
 */
uint16_t CRC16_XMODEM_Table_Byte(uint8_t CheckByte, uint16_t LastCRC)
{
    return (LastCRC << 8) ^ CRC_XMODEM_table[(LastCRC >> 8) ^ CheckByte];
}

/******函数名:CRC16_XMODEM_fast_Fun() *********************
 *
 *   功能描述:
 *       这个函数为系统的CRC16的计算函数，计算指定地址和个数
 *       的数据的CRC_16的校验值.
 *       计算公式：
 *       	 CRC_16= x^16 + x^15 + x^2 + 1
 *
 *   调用参数:
 *      ptr 计算的数据起始地址
 *		len 计算的数据长度
 *   返回值:
 *       函数计算的CRC值
 */
uint16_t Encrypt_XMODEM_CRC16_Fun(uint8_t *data, int len)
{
    uint16_t retval = 0;
    int temp_run = 0;
    while (len--)
    {
        retval = CRC16_XMODEM_Table_Byte(data[temp_run++], retval);
    }
    return (retval & 0xffff);
}

// x16+x15+x2+1
int Encrypt_ModBus_CRC16_Fun(uint8_t *data, int len)
{
    int i, j, temp, CRC16;

    CRC16 = 0xFFFF; /* CRC寄存器初始值 */
    for (i = 0; i < len; i++)
    {
        CRC16 ^= data[i];
        for (j = 0; j < 8; j++)
        {
            temp = (int)(CRC16 & 0x0001);
            CRC16 >>= 1;
            if (temp == 1)
            {
                CRC16 ^= 0xA001; /* 异或多项式 */
            }
        }
    }
    return CRC16;
}

#if AES

#define AES_KEY_LENGTH 128

// 加解密模式
#define AES_MODE_ECB 0        // 电子密码本模式
#define AES_MODE_CBC 1        // 密码分组链接模式
#define AES_MODE AES_MODE_ECB // 配置加密模式

#define Nk (AES_KEY_LENGTH / 32) // 以“字”（4字节）为单位的密钥长度
#define Nb 4                     // 以“字”（4字节）为单位的加解密数据块大小，固定为4

// Nr：加密的轮数
#if AES_KEY_LENGTH == 128
#define Nr 10
#elif AES_KEY_LENGTH == 192
#define Nr 12
#elif AES_KEY_LENGTH == 256
#define Nr 14
#else
#error AES_KEY_LENGTH must be 128, 192 or 256
#endif

// GF(28) 多项式
#define BPOLY 0x1B // Lower 8 BOOLs of (x^8 + x^4 + x^3 + x + 1), ie. (x^4 + x^3 + x + 1).

/*****************************************************************************
 *  Local variable
 *****************************************************************************/

// AES子密钥表，当密钥长度为128位时，占用176字节空间
static uint8_t g_roundKeyTable[4 * Nb * (Nr + 1)];
// AES密钥
static uint8_t AES_KeyTable[50] = "Z4T6U7RL08VJ5Y36";
// 加密用的SBox
static const uint8_t SBox[256] =
    {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

// 解密用的SBox
static const uint8_t InvSBox[256] =
    {
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

/*****************************************************************************
 *  Local Functions
 ******************************************************************************/

/*****************************************************************************
//	函数名：	RotationWord
//	描述：		对一个“字”数据进行循环右移。
//	输入参数：	pWord -- 要右移的4字节数据。
//	输出参数：	pWord -- 右移后的4字节数据。
//	返回值：      无。
*****************************************************************************/
static void RotationWord(uint8_t *pWord)
{
    uint8_t temp = pWord[0];
    pWord[0] = pWord[1];
    pWord[1] = pWord[2];
    pWord[2] = pWord[3];
    pWord[3] = temp;
}

/*****************************************************************************
//	函数名：	XorBytes
//	描述：		批量异或两组数据。
//	输入参数：	pData1 -- 要异或的第一组数据。
//				pData1 -- 要异或的第二组数据。
//				nCount -- 要异或的数据长度。
//	输出参数：	pData1 -- 异或后的结果。
//	返回值：    无。
*****************************************************************************/
static void XorBytes(uint8_t *pData1, const uint8_t *pData2, uint8_t nCount)
{
    uint8_t i;

    for (i = 0; i < nCount; i++)
    {
        pData1[i] ^= pData2[i];
    }
}

/*****************************************************************************
//	函数名：	AddRoundKey
//	描述：		把 中间状态数据 加上（异或）子密钥，数据长度为16字节。
//	输入参数：	pState	  -- 状态数据。
//				pRoundKey -- 子密钥数据。
//	输出参数：	pState	  -- 加上子密钥后的状态数据。
//	返回值：	无。
*****************************************************************************/
static void AddRoundKey(uint8_t *pState, const uint8_t *pRoundKey)
{
    XorBytes(pState, pRoundKey, 4 * Nb);
}

// AddRoundKey的宏形式，比函数形式可以节省4字节的data数据
// #define AddRoundKey(pState, pRoundKey) XorBytes((pState), (pRoundKey), 4*Nb)

/*****************************************************************************
//	函数名：	SubBytes
//	描述：		通过S盒子置换状态数据。
//	输入参数：	pState	-- 状态数据。
//				nCount  -- 状态数据长度。
//				bInvert	-- 是否使用反向S盒子（解密时使用）。
//	输出参数：	pState	-- 置换后的状态数据。
//	返回值：	无。
*****************************************************************************/
static void SubBytes(uint8_t *pState, uint8_t nCount, uint8_t bInvert)
{
    uint8_t i;
    const uint8_t *pSBox = bInvert ? InvSBox : SBox;

    for (i = 0; i < nCount; i++)
    {
        pState[i] = pSBox[pState[i]];
    }
}

/*****************************************************************************
//	函数名：	ShiftRows
//	描述：		把状态数据移行。
//	输入参数：	pState	-- 状态数据。
//				bInvert	-- 是否反向移行（解密时使用）。
//	输出参数：	pState	-- 移行后的状态数据。
//	返回值：	无。
*****************************************************************************/
static void ShiftRows(uint8_t *pState, uint8_t bInvert)
{
    // 注意：状态数据以列形式存放！

    uint8_t r; // row，   行
    uint8_t c; // column，列
    uint8_t temp;
    uint8_t rowData[4];

    for (r = 1; r < 4; r++)
    {
        // 备份一行数据
        for (c = 0; c < 4; c++)
        {
            rowData[c] = pState[r + 4 * c];
        }

        temp = bInvert ? (4 - r) : r;
        for (c = 0; c < 4; c++)
        {
            pState[r + 4 * c] = rowData[(c + temp) % 4];
        }
    }
}

/*****************************************************************************
//	函数名：	GfMultBy02
//	描述：		在GF(28)域的 乘2 运算。
//	输入参数：	num	-- 乘数。
//	输出参数：	无。
//	返回值：	num乘以2的结果。
*****************************************************************************/
static uint8_t GfMultBy02(uint8_t num)
{
    if ((num & 0x80) == 0)
    {
        num = num << 1;
    }
    else
    {
        num = (num << 1) ^ BPOLY;
    }

    return num;
}

/*****************************************************************************
//	函数名：	MixColumns
//	描述：		混合状态各列数据。
//	输入参数：	pState	-- 状态数据。
//				bInvert	-- 是否反向混合（解密时使用）。
//	输出参数：	pState	-- 混合列后的状态数据。
//	返回值：	无。
*****************************************************************************/
static void MixColumns(uint8_t *pState, uint8_t bInvert)
{
    uint8_t i;
    uint8_t temp;
    uint8_t a0Pa2_M4; // 4(a0 + a2)
    uint8_t a1Pa3_M4; // 4(a1 + a3)
    uint8_t result[4];

    for (i = 0; i < 4; i++, pState += 4)
    {
        temp = pState[0] ^ pState[1] ^ pState[2] ^ pState[3];
        result[0] = temp ^ pState[0] ^ GfMultBy02((uint8_t)(pState[0] ^ pState[1]));
        result[1] = temp ^ pState[1] ^ GfMultBy02((uint8_t)(pState[1] ^ pState[2]));
        result[2] = temp ^ pState[2] ^ GfMultBy02((uint8_t)(pState[2] ^ pState[3]));
        result[3] = temp ^ pState[3] ^ GfMultBy02((uint8_t)(pState[3] ^ pState[0]));

        if (bInvert)
        {
            a0Pa2_M4 = GfMultBy02(GfMultBy02((uint8_t)(pState[0] ^ pState[2])));
            a1Pa3_M4 = GfMultBy02(GfMultBy02((uint8_t)(pState[1] ^ pState[3])));
            temp = GfMultBy02((uint8_t)(a0Pa2_M4 ^ a1Pa3_M4));
            result[0] ^= temp ^ a0Pa2_M4;
            result[1] ^= temp ^ a1Pa3_M4;
            result[2] ^= temp ^ a0Pa2_M4;
            result[3] ^= temp ^ a1Pa3_M4;
        }

        memcpy(pState, result, 4);
    }
}

/*****************************************************************************
//	函数名：	BlockEncrypt
//	描述：		对单块数据加密。
//	输入参数：	pState -- 状态数据。
//	输出参数：	pState -- 加密后的状态数据。
//	返回值：	无。
*****************************************************************************/
static void BlockEncrypt(uint8_t *pState)
{
    uint8_t i;

    AddRoundKey(pState, g_roundKeyTable);

    for (i = 1; i <= Nr; i++) // i = [1, Nr]
    {
        SubBytes(pState, 4 * Nb, 0);
        ShiftRows(pState, 0);

        if (i != Nr)
        {
            MixColumns(pState, 0);
        }

        AddRoundKey(pState, &g_roundKeyTable[4 * Nb * i]);
    }
}

/*****************************************************************************
//	函数名：	BlockDecrypt
//	描述：		对单块数据解密。
//	输入参数：	pState -- 状态数据。
//	输出参数：	pState -- 解密后的状态数据。
//	返回值：	无。
*****************************************************************************/
static void BlockDecrypt(uint8_t *pState)
{
    uint8_t i;

    AddRoundKey(pState, &g_roundKeyTable[4 * Nb * Nr]);

    for (i = Nr; i > 0; i--) // i = [Nr, 1]
    {
        ShiftRows(pState, 1);
        SubBytes(pState, 4 * Nb, 1);
        AddRoundKey(pState, &g_roundKeyTable[4 * Nb * (i - 1)]);

        if (i != 1)
        {
            MixColumns(pState, 1);
        }
    }
}

/*****************************************************************************
 *  Global Functions
 ******************************************************************************/

/*****************************************************************************
//	函数名：	AES_Encrypt
//	描述：		加密数据
//	输入参数：	pPlainText	-- 明文，即需加密的数据，其长度为nDataLen字节。
//				nDataLen	-- 数据长度，以字节为单位，必须为AES_KEY_LENGTH/8的整倍数。
//				pIV			-- 初始化向量，如果使用ECB模式设为NULL。
//	输出参数：	pCipherText	-- 密文，即由明文加密后的数据，可以与pPlainText相同。
//	返回值：	无。
*****************************************************************************/
void AES_Encrypt(const uint8_t *pPlainText, uint8_t *pCipherText,
                 unsigned int nDataLen, const uint8_t *pIV)
{
    unsigned int i;

    if (pPlainText != pCipherText)
    {
        memcpy(pCipherText, pPlainText, nDataLen);
    }

    for (i = nDataLen / (4 * Nb); i > 0; i--, pCipherText += 4 * Nb)
    {
#if AES_MODE == AES_MODE_CBC
        XorBytes(pCipherText, pIV, 4 * Nb);
#endif

        BlockEncrypt(pCipherText);
        if (pIV == NULL)
            pIV = pCipherText;
        else
            pIV = pCipherText;
    }
}

/*****************************************************************************
//	函数名：	AES_Decrypt
//	描述：		解密数据
//	输入参数：	pCipherText -- 密文，即需解密的数据，其长度为nDataLen字节。
//				nDataLen	-- 数据长度，以字节为单位，必须为AES_KEY_LENGTH/8的整倍数。
//				pIV			-- 初始化向量，如果使用ECB模式设为NULL。
//	输出参数：	pPlainText  -- 明文，即由密文解密后的数据，可以与pCipherText相同。
//	返回值：	无。
*****************************************************************************/
void AES_Decrypt(uint8_t *pPlainText, const uint8_t *pCipherText,
                 unsigned int nDataLen, const uint8_t *pIV)
{
    unsigned int i;

    if (pPlainText != pCipherText)
    {
        memcpy(pPlainText, pCipherText, nDataLen);
    }

    // 从最后一块数据开始解密，这样不用开辟空间来保存IV
    pPlainText += nDataLen - 4 * Nb;
    for (i = nDataLen / (4 * Nb); i > 0; i--, pPlainText -= 4 * Nb)
    {
        BlockDecrypt(pPlainText);

#if AES_MODE == AES_MODE_CBC
        if (i == 1)
        {
            // 最后一块数据
            XorBytes(pPlainText, pIV, 4 * Nb);
        }
        else
        {
            XorBytes(pPlainText, pPlainText - 4 * Nb, 4 * Nb);
        }
#endif
    }
}

/*****************************************************************************
//	函数名：	AES_add_pkcs7Padding
//	描述：		PKCS7Padding填充补齐
//	输入参数：	input -- 后面最多预留16个字节空间用于存放填充值
//				len --  数据的长度
//	输出参数：	input  -- 添加填充码后的数据
//	返回值：	填充后的长度
*****************************************************************************/
unsigned int AES_add_pkcs7Padding(uint8_t *input, unsigned int len)
{
    unsigned int i, end, padd_len;
    unsigned int reminder = len % 16;
    unsigned int block = len / 16;
    unsigned int start = len; // 开始补码的地址

    padd_len = len;
    if (reminder != 0) // 需要补足为16的整数倍
    {
        start = len;
        end = (block + 1) * 16;

        for (i = start; i < end; i++)
        {
            input[i] = 16 - reminder;
            padd_len++;
        }
    }
    else
    {
        end = start + 16;
        for (i = start; i < end; i++)
        {
            input[i] = 16;
            padd_len++;
        }
    }

    return padd_len;
}

/*****************************************************************************
//	函数名：	AES_delete_pkcs7Padding
//	描述：		PKCS7Padding填充密文解密后剔除填充值
//	输入参数：	input -- 解密后的数据
//				len --  数据的长度
//	输出参数：	input  -- 删除填充码后的数据
//	返回值：	删除后的实际有效数据长度，为0表示传入的数据异常
*****************************************************************************/
unsigned int AES_delete_pkcs7Padding(uint8_t *input, unsigned int len)
{
    uint8_t i, last;

    if ((input == 0) || ((len & 0x0F) != 0))
    {
        return 0;
    }

    last = input[len - 1];

    for (i = len - last; i < len; i++)
    {
        if (input[i] != last)
        {
            return 0;
        }
    }

    return len - last;
}

void hex2str(uint8_t *hex, uint16_t hex_len, char *str, uint8_t low_or_up)
{
    int i, pos = 0;
    for (i = 0; i < hex_len; i++)
    {
        if (low_or_up == 1) // 字母大写
            sprintf(str + pos, "%02X", hex[i]);
        else
            sprintf(str + pos, "%02x", hex[i]); // 字母小写
        pos += 2;
    }
}

uint8_t str2hex(char a)
{
    uint8_t return_value = 0xff;
    if ((a >= 0x30) & (a <= 0x39))
        return_value = a - 0x30;
    else if ((a >= 0x61) & (a <= 0x66))
        return_value = a - 0x57;
    else if ((a >= 0x41) & (a <= 0x46))
        return_value = a - 0x37;

    return return_value;
}

uint16_t str2hex_decrypt(char *str, uint8_t *return_data)
{
    //
    int len = strlen(str);
    int offset = 0;
    uint8_t k, j = 0;
    for (int a = 0; a < len; a = a + 2)
    {
        k = str2hex(str[a]);
        j = str2hex(str[a + 1]);
        return_data[offset] = k;
        return_data[offset] = (return_data[offset] << 4) + j;
        offset++;
    }
    return offset;
}

/*****************************************************************************
//	函数名：	AES_Init
//	描述：		初始化，在此执行扩展密钥操作。
//	输入参数：	pKey -- 原始密钥，其长度必须为 AES_KEY_LENGTH/8 字节。
//	输出参数：	无。
//	返回值：	无。
*****************************************************************************/
void AES_Init(const void *pKey)
{
    // 扩展密钥
    uint8_t i;
    uint8_t *pRoundKey;
    uint8_t Rcon[4] = {0x01, 0x00, 0x00, 0x00};

    memcpy(g_roundKeyTable, pKey, 4 * Nk);

    pRoundKey = &g_roundKeyTable[4 * Nk];

    for (i = Nk; i < Nb * (Nr + 1); pRoundKey += 4, i++)
    {
        memcpy(pRoundKey, pRoundKey - 4, 4);

        if (i % Nk == 0)
        {
            RotationWord(pRoundKey);
            SubBytes(pRoundKey, 4, 0);
            XorBytes(pRoundKey, Rcon, 4);

            Rcon[0] = GfMultBy02(Rcon[0]);
        }
        else if (Nk > 6 && i % Nk == Nb)
        {
            SubBytes(pRoundKey, 4, 0);
        }

        XorBytes(pRoundKey, pRoundKey - 4 * Nk, 4);
    }
}
#endif //


void Encrypt_AES_set_key (char *key_str)
{
    int temp_num = strlen(key_str);
    if (key_str != NULL && temp_num < sizeof(AES_KeyTable))
    {
        memcpy(AES_KeyTable,key_str,temp_num);
    }
}

/*
//	函数名：	Encrypt_AES_decrypt
    @brief  aes 		数据解密
    @param data         接收到的数据
    @param data_length  接收到数据的长度
    @return
*/
uint8_t Encrypt_AES_decrypt_Fun(uint8_t *data, uint16_t data_length, uint8_t *ret_data, uint16_t *ret_length)
{
#if AES
//    uint16_t temp_len = data_length + 100;
    // uint8_t *decrypt_result = malloc(temp_len);
    // uint8_t *array = malloc((data_length / 2) + 2);
    uint8_t decrypt_result[300];
    uint8_t array[200];
    uint16_t decrypt_result_length = 0;
    if ((decrypt_result != NULL) & (array != NULL))
    {
        memset(decrypt_result, 0, sizeof(decrypt_result));

        AES_Init(AES_KeyTable);
        // 解密数据不需要转换为字符串,直接使用服务器端传过来的16进制数据进行解密; 加密的数据需要转换为字符串后送入加密函数
        AES_Decrypt(decrypt_result, &data[0], data_length, NULL);
        memset(array, 0, sizeof(array));
        decrypt_result_length = str2hex_decrypt((char *)decrypt_result, array);

        if (decrypt_result_length > 0)
        {
            *ret_length = decrypt_result_length;
            if (ret_data != NULL)
            {
                memcpy(ret_data, array, decrypt_result_length);
            }
        }
        else
        {
            // free(array);
        }
        // free(decrypt_result);
        return 1;
    }
    else
    {

    }
#endif //
    return 0;
}

/*
//	函数名：	Encrypt_AES_unpack
//	描述：		加密数据
//	输入参数：	pPlainText	-- 明文，即需加密的数据，其长度为nDataLen字节。
//				nDataLen	-- 数据长度，以字节为单位，必须为AES_KEY_LENGTH/8的整倍数。
//				pIV			-- 初始化向量，如果使用ECB模式设为NULL。
//	输出参数：	pCipherText	-- 密文，即由明文加密后的数据，可以与pPlainText相同。
//	返回值：	无
*/
int Encrypt_AES_unpack_Fun (uint8_t *source, uint16_t len,uint8_t *ret_data,uint8_t *pIV)
{
    int retval = 0;
#if AES
    int temp_num = 0;
    char temp_str[100];
    if (ret_data == NULL)
    {
        return retval;
    }
    hex2str(source, len, temp_str, 1);
    temp_num = strlen(temp_str);
    AES_Init(AES_KeyTable);
    AES_Encrypt((uint8_t *)temp_str, ret_data, temp_num, NULL);

    retval = temp_num;
    return retval;
#endif //
}


