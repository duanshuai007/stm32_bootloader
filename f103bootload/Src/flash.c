#include "flash.h"
#include "stm32f1xx.h"
#include "stdlib.h"

uint8_t FlashGetLocalFWVersion(void)
{
  uint8_t buf[4] = {0};
  uint8_t version = 0;
  buf[0] = READ_FLASH_BYTE(FLASH_SAVE_SECTION  );
  buf[1] = READ_FLASH_BYTE(FLASH_SAVE_SECTION + 1);
  buf[2] = READ_FLASH_BYTE(FLASH_SAVE_SECTION + 2);
  buf[3] = READ_FLASH_BYTE(FLASH_SAVE_SECTION + 3);
  
  if ( buf[0] == 'v' || buf[0] == 'V') {
    if ( buf[2] == '.' ) {
      version |= (buf[1] - '0');
      version <<= 4;
      version |= (buf[3] - '0');
    }
  }

  return version;
}

void FlashUpdateFWVersion(uint8_t ver)
{
  FLASH_EraseInitTypeDef f;
  uint32_t pageerror = 0;
  uint8_t version[4];
  
  HAL_FLASH_Unlock();
 
  f.TypeErase = FLASH_TYPEERASE_PAGES;
  f.PageAddress = FLASH_SAVE_SECTION;
  f.NbPages = 1;
  
  HAL_FLASHEx_Erase(&f, &pageerror);

  version[0] = 'v';
  version[1] = (ver >> 4) | 0x30;
  version[2] = '.';
  version[3] = (ver & 0x0f) | 0x30;
  //将version地址指针转换为uint32_t类型指针，再用该指针取值
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_SAVE_SECTION, *((uint32_t *)version));
  
  HAL_FLASH_Lock();
}

void FlashWrite(uint32_t addr, uint8_t *buffer, uint32_t len)
{
  FLASH_EraseInitTypeDef f;
  uint32_t pageerror = 0;
  uint32_t i;
  
  HAL_FLASH_Unlock();
 
  f.TypeErase = FLASH_TYPEERASE_PAGES;
  f.PageAddress = addr;
  f.NbPages = 1;
  
  HAL_FLASHEx_Erase(&f, &pageerror);
  
  //恢复lora信息
  for(i = 0; i < len/4; i++) {
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (addr + (i*4)), *((uint32_t *)buffer + (i)));
  }
  
  HAL_FLASH_Lock();
}
