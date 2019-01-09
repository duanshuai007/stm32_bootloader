#ifndef _DATACOPY_H_
#define _DATACOPY_H_

#include <stm32f1xx.h>
#include "user_config.h"

bool CopyDataFromDMAHandler(UART_HandleTypeDef *huart);

#endif