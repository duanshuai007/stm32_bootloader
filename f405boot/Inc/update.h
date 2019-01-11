#ifndef _UPDATE_H_
#define _UPDATE_H_

#define F103_FW_SAVE_IN     0x08004000
#define F103_FW_SIZE        22368
#define FWVersion           0x13 

#include "stdint.h"

uint32_t getFWTotalSize(void);
uint8_t getFWPacketNum(void);
uint32_t getFWLastPacketSize(void);

uint8_t *getFW(uint8_t num, uint32_t len);

void SendFWToF103(uint8_t packetnum, uint32_t len);

#endif
