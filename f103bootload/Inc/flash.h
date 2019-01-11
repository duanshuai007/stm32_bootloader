#ifndef _STM32_FLASH_H_
#define _STM32_FLASH_H_

#include "stdint.h"
#include "user_config.h"

#define SECTOR_SIZE       1024

#define READ_FLASH_WORD(addr) (*(volatile uint32_t *)(addr))
#define READ_FLASH_BYTE(addr) (*(volatile uint8_t *)(addr))

uint8_t FlashGetLocalFWVersion(void);

void FlashUpdateFWVersion(uint8_t ver);

void FlashWrite(uint32_t addr, uint8_t *buffer, uint32_t len);

#endif