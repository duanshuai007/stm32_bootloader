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


#if 0

#define INFO_DBG  0
#define WARN_DBG  1
#define ERROR_DBG 1

#define DEBUG_INFO(format, ...)   do{if(INFO_DBG){printf("[INFO]:"format, ##__VA_ARGS__);}}while(0)
#define DEBUG_WARN(format, ...)   do{if(WARN_DBG){printf("[WARN][%s]:"format, __FUNCTION__ , ##__VA_ARGS__);}}while(0)
#define DEBUG_ERROR(format, ...)  do{if(ERROR_DBG){printf("[ERROR][%s]:"format, __FUNCTION__ , ##__VA_ARGS__);}}while(0)
#else

#define DEBUG_INFO(format, ...) 
#define DEBUG_WARN(format, ...)
#define DEBUG_ERROR(format, ...)
#endif


#define APPLICATION_ADDR              0x08004000


#endif
