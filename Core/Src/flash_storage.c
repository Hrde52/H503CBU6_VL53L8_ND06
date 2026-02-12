#include "flash_storage.h"
HAL_StatusTypeDef Flash_Write_Array(const uint8_t *data, uint32_t length)
{
    if (data == NULL || length == 0 || length > FLASH_USER_SIZE)
        return HAL_ERROR; // 参数检查

    HAL_FLASH_Unlock();

    // 擦除扇区
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector        = FLASH_USER_SECTOR;
    EraseInitStruct.NbSectors     = 1;
//    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return HAL_ERROR;
    }

    uint32_t address = FLASH_USER_START_ADDR;
    uint32_t i = 0;

    // 以4字节为单位写入
    while (i < length)
    {
        uint32_t word = 0xFFFFFFFF;
        for (uint8_t j = 0; j < 4 && (i + j) < length; j++)
        {
            ((uint8_t*)&word)[j] = data[i + j];
        }
//        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, word) != HAL_OK)
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, address, word) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return HAL_ERROR;
        }
        address += 4;
        i += 4;
    }

    HAL_FLASH_Lock();
    return HAL_OK;
}

void Flash_Read_Array(uint8_t *data, uint32_t length)
{
    if (length > FLASH_USER_SIZE) return; // 边界检查

    uint32_t address = FLASH_USER_START_ADDR;
    for (uint32_t i = 0; i < length; i++)
    {
        data[i] = *(volatile uint8_t*)(address + i);
    }
}