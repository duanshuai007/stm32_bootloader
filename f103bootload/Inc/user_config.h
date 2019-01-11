#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#include "stdint.h"
#include "stm32f1xx.h"
#include "lora_paramter.h"

typedef enum {
  false,
  true,
}bool;

/*
*   ±£¥Êfw version–≈œ¢
*/
#define FLASH_SAVE_SECTION    0x08003C00

#define APPLICATION_ADDR      0x08004000


#endif
