#ifndef _BOOTLOAD_103_H_
#define _BOOTLOAD_103_H_

#include "stdint.h"
#include "user_config.h"
#include "flash.h"

typedef void (* pFunction)(void);

#define VERSION_LEN   4
#define MD5_LEN       32

typedef struct {
  volatile uint32_t pos;
  UART_HandleTypeDef  *uart;    //串口
  uint16_t dma_rbuff_size;      //dma接收缓冲区的大小
  uint8_t *dma_rbuff;           //指向dma接收缓冲区
} UartModule;

#pragma pack(1)
typedef struct {
  uint8_t u8Version;
  uint8_t u8NumOfPacket;  //一共多少个包
  uint8_t u8MD5[MD5_LEN];
  uint32_t u32FWSize;     //整体的大小
} FirmWare;

//向服务器发送的查询是否有升级固件的命令
typedef struct {
  uint8_t   u8Head;
  uint8_t   u8Cmd;
  uint8_t   u8Version;
  uint16_t  u16CRC;
  uint8_t   u8Tail;
} DeviceReqUpData;

//从服务器接收到的升级固件信息的命令格式
typedef struct {
  uint8_t u8Head;
  uint8_t u8Cmd;
  uint8_t u8Update;
  uint8_t u8Version;
  uint8_t u8TotalNumOfPacket;
  uint32_t u32TotalSize;
//  uint8_t u8MD5[MD5_LEN];
  uint16_t u16CRC;
  uint8_t u8Tail;
} RespUpdate;

//从服务器获取固件内容
typedef struct {
  uint8_t u8Head;
  uint8_t u8Cmd;
  uint8_t u8PacketNum;
  uint16_t u16CRC;
  uint8_t u8Tail;
} DeviceReqFW;

//接收到服务器发送来的固件内容
typedef struct {
  uint8_t u8Head;
  uint8_t u8Cmd;
  uint8_t u8CurNum;         //当前包号
  uint32_t u32PacketSize;   //当前包大小
  uint8_t u8FW[SECTOR_SIZE];       //包最大1024
  uint16_t u16CRC;
  uint8_t u8Tail;
} RespFW;

typedef struct 
{
  uint8_t ucBootType;
  uint8_t ucTryTimes;
  uint8_t ucBootSeccessFlg;
  uint8_t ucRev[1];
  
  uint8_t ucFWVer[VERSION_LEN];
  uint8_t ucNewFWVer[VERSION_LEN];
  
}T_BootConfig;

#pragma pack()

/*
*   初始化与F405通信的串口相关结构体
*/
void BootLoad_UartInit(UART_HandleTypeDef *huart);

/*
*     向服务器问询是否有更新版本
*     返回值：0 服务器没有应答
*             1 有更新版本
*             2 无更新版本
*/
uint8_t CheckUpdata(uint8_t ver);

bool GetFileBin(void);

void iap_load_app(void);

uint8_t getFWVersion(void);

#endif



