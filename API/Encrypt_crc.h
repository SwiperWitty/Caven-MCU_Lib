#ifndef _ENCRYPT_CRC_H_
#define _ENCRYPT_CRC_H_


#ifdef CAVEN
#include "Caven_Type.h"
#else
#include "stdint.h"
#endif

/*
    ModBus_CRC16用的
*/

int Encrypt_ModBus_CRC16_Fun (uint8_t *data, int len);
uint16_t CRC16_XMODEM_Table_Byte(uint8_t CheckByte, uint16_t LastCRC);
uint16_t Encrypt_XMODEM_CRC16_Fun(uint8_t *data, int len);
uint64_t Encrypt_ARX_Fun(const uint8_t *key, const uint8_t *data, uint32_t len);

#ifdef AES
void Encrypt_AES_set_key (char *key_str);
uint8_t Encrypt_AES_decrypt_Fun (uint8_t *data, uint16_t data_length,uint8_t *ret_data,uint16_t *ret_length);
int Encrypt_AES_unpack_Fun (uint8_t *source, uint16_t len,uint8_t *ret_data,uint8_t *pIV);
#endif

#endif
