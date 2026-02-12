#include "stm32h5xx_hal.h"

#define FLASH_USER_START_ADDR   0x08060000U // 扇区7起始地址
#define FLASH_USER_SECTOR     FLASH_SECTOR_7
#define FLASH_USER_SIZE  (128 * 1024) // 128KB
HAL_StatusTypeDef Flash_Write_Array(const uint8_t *data, uint32_t length);
void Flash_Read_Array(uint8_t *data, uint32_t length);
