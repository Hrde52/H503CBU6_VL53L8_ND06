/**
 * @file nd06av1c_comm.c
 * @author huiyang.jiang
 * @brief ND06AV1C communication functions
 * @version 1.x.x
 * @date 2023-5
 *
 * @copyright Copyright (c) 2023, Shenzhen Nephotonics Inc.
 *
 */

#include "nd06av1c_comm.h"
#include "nd06av1c_dev.h"
#include "nd06av1c_data.h"
#include <stdio.h>
/** 寄存器偏移地址 */
uint16_t ND06AV1C_REG_OFS = 0;
/** iic单字节写函数指针 */
int32_t(*I2C_WriteNBytesLib)(uint8_t, uint16_t, uint8_t *, uint8_t);
/** iic单字节读函数指针 */
int32_t(*I2C_ReadNBytesLib)(uint8_t, uint16_t,uint8_t *, uint8_t);


/**
 * @brief 注册IIC API
 *
 * @param   I2C_WriteNBytesFunc iic 写N个bytes函数
 * @param   I2C_ReadNBytesFunc  iic 读N个bytes函数
 * @return  void
 */
void ND06AV1C_RegisteredIICApi(
        int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t),
        int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t)
    )
{
    I2C_ReadNBytesLib = I2C_ReadNBytesFunc;
    I2C_WriteNBytesLib = I2C_WriteNBytesFunc;
}


/**
 * @brief Write 1 Byte to ND06AV1C
 *        对ND06AV1C的寄存器写1个字节
 * @param pNxDevice: ND06AV1C模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 寄存器的值
 * @return int32_t
*/
int32_t ND06AV1C_WriteByte(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint8_t wdata)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;

    ret = I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, &wdata, 1);

	return ret;
}


/**
 * @brief Write 4 Bytes to ND06AV1C
 *        对ND06AV1C的寄存器写1个字
 * @param pNxDevice: ND06AV1C模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 寄存器的值
 * @return int32_t
*/
int32_t ND06AV1C_WriteWord(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint32_t wdata)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint8_t     data[4];
    uint32_t    try = 4;

    data[0] = (wdata >> 0) & 0xff;
    data[1] = (wdata >> 8) & 0xff;
    data[2] = (wdata >>16) & 0xff;
    data[3] = (wdata >>24) & 0xff;
    ret = I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, data, 4);
    while(ret != 0 && try--)
    {
        if(ND06AV1C_I2CErrCallback(pNxDevice) != 0)
            return ND06AV1C_ERROR_I2C;
        ret = I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, data, 4);
    }
	return ret;
}


/**
 * @brief Read 4 Bytes from ND06AV1C
 *        对ND06AV1C的寄存器读1个字
 * @param pNxDevice: ND06AV1C模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param rdata: 存放寄存器值的指针
 * @return int32_t
*/
int32_t ND06AV1C_ReadWord(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint8_t     data[4];
    uint32_t    try = 4;

    ret = I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, addr, data, 4);

    while(ret != 0 && try--)
    {
        if(ND06AV1C_I2CErrCallback(pNxDevice) != 0)
            return ND06AV1C_ERROR_I2C;
        ret = I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, addr, data, 4);
    }

    *rdata = ((uint32_t)data[3]<<24) | ((uint32_t)data[2]<<16) | ((uint32_t)data[1]<<8) | (uint32_t)data[0];

	return ret;
}


/**
 * @brief Write n Words to ND06AV1C
 *        对ND06AV1C的寄存器写N个字
 * @param pNxDevice: ND06AV1C模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 存放寄存器值的指针
 * @param len: 写数据的长度，按字个数计算
 * @return int32_t
*/
int32_t ND06AV1C_WriteNWords(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint32_t *wdata, uint32_t len)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    try = 4;

    ret |= I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, (uint8_t*)wdata, len*4);

    while(ret != 0 && try--)
    {
        if(ND06AV1C_I2CErrCallback(pNxDevice) != 0)
            return ND06AV1C_ERROR_I2C;
        ret = I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, (uint8_t*)wdata, len*4);
    }

	return ret;
}


/**
 * @brief Read n Words from ND06AV1C
 *        对ND06AV1C的寄存器读N个字
 * @param pNxDevice: ND06AV1C模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param rdata: 存放寄存器值的指针
 * @param len: 读数据的长度，按字个数计算
 * @return int32_t
*/
int32_t ND06AV1C_ReadNWords(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata, uint32_t len)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    try = 4;

    ret |= I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, addr, (uint8_t*)rdata, len*4);
    while(ret != 0 && try--)
    {
        if(ND06AV1C_I2CErrCallback(pNxDevice) != 0)
            return ND06AV1C_ERROR_I2C;
        ret = I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, addr, (uint8_t*)rdata, len*4);
    }

	return ret;
}
