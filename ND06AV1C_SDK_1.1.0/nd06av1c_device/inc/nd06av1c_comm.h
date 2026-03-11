/**
 * @file nd06av1c_comm.h
 * @author huiyang.jiang
 * @brief ND06AV1C communication functions
 * @version 1.x.x
 * @date 2023-5
 * 
 * @copyright Copyright (c) 2023, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND06AV1C_COMM_H__
#define __ND06AV1C_COMM_H__

#include "nd06av1c_def.h"

extern uint16_t ND06AV1C_REG_OFS;

/** @defgroup ND06AV1C_Communication_Group ND06AV1C Communication Funtions
 *  @brief ND06AV1C Communication Funtions
 *  @{
 */

/** IIC注册函数，即将实现的iic接口函数传输注册到本SDK中 */
void ND06AV1C_RegisteredIICApi(
        int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t len),
        int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t len)
    );
    
/** 对ND06AV1C寄存器写1个字节 */
int32_t ND06AV1C_WriteByte(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint8_t wdata);

/** 对ND06AV1C寄存器写1个字 */
int32_t ND06AV1C_WriteWord(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint32_t wdata);

/** 对ND06AV1C寄存器读1个字 */
int32_t ND06AV1C_ReadWord(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata);

/** 对ND06AV1C寄存器写N个字 */
int32_t ND06AV1C_WriteNWords(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint32_t *wdata, uint32_t len);

/** 对ND06AV1C寄存器读N个字 */
int32_t ND06AV1C_ReadNWords(ND06AV1C_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata, uint32_t len);



/** @} ND06AV1C_Communication_Group */

#endif

