/**
 * @file nd06av1c_data.h
 * @author huiyang.jiang
 * @brief ND06AV1C get depth data functions
 * @version 1.x.x
 * @date 2023-5
 * 
 * @copyright Copyright (c) 2023, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND06AV1C_DATA_H__
#define __ND06AV1C_DATA_H__

#include "nd06av1c_def.h"
 
 
extern uint32_t begin65300Time;
extern uint8_t over30s;
/** @defgroup ND06AV1C_Data_Group ND06AV1C Data Funtions
 *  @brief ND06AV1C Data Funtions
 *  @{
 */

/** 获取一次测量深度和幅度值 */
int32_t ND06AV1C_GetDepthAndAmpData(ND06AV1C_Dev_t *pNxDevice,uint16_t *amp,uint16_t *dep);

/** ND06AV1C获取配置数据*/
int32_t ND06AV1C_GetConfigData(ND06AV1C_Dev_t *pNxDevice,ND06AV1C_Config_Options config,uint32_t addr_offset, uint32_t *data);

/** ND06AV1C写入配置数据*/
int32_t ND06AV1C_SetConfigData(ND06AV1C_Dev_t *pNxDevice,ND06AV1C_Config_Options config,uint32_t addr_offset,uint32_t data);

/** ND06AV1C保存itof数据到flash */
int32_t ND06AV1C_SaveItofData(ND06AV1C_Dev_t *pNxDevice);

/** 恢复标定数据以及配置数据为出厂数据 */
int32_t ND06AV1C_ResetTofData(ND06AV1C_Dev_t *pNxDevice);





/** ND06AV1C恢复配置数据*/
int32_t ND06AV1C_DataRecovery(ND06AV1C_Dev_t *pNxDevice);
/*
  用户无需关心此函数
 */

/** 等待ND06AV1C操作完成 */
int32_t ND06AV1C_WaitNd06Ready(ND06AV1C_Dev_t *pNxDevice,uint32_t req_val);
/*
  用户无需关心此函数
 */


/** @} ND06AV1C_Data_Group */

#endif

