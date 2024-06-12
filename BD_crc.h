#ifndef __crc_H_dim
#define __crc_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"
#include "osl_general.h"

void CalulateTable_CRC8(void);
uint8_t Compute_CRC8(uint8_t bytes[], uint8_t Lenth);
char *MakeCRC(char *BitString);	

uint16_t CRC16(uint8_t *p, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif
