/**
 * @file nd06av1c_dev.h
 * @author huiyang.jiang
 * @brief ND06AV1C device setting functions
 * @version 1.x.x
 * @date 2023-5
 * 
 * @copyright Copyright (c) 2023, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND06AV1C_DEV_H__
#define __ND06AV1C_DEV_H__

#include "nd06av1c_def.h"

/** @defgroup ND06AV1C_Dev_Group ND06AV1C Device Funtions
 *  @brief ND06AV1C Device Funtions
 *  @{
 */

/** 获取SDK版本号 */
uint32_t ND06AV1C_GetSdkVersion(void);

/** 初始化ND06AV1C库的API */
void ND06AV1C_RegisteredPlatformApi(ND06AV1C_Func_Ptr_t dev_op);

/** 设置ND06AV1C的设备地址 */
int32_t ND06AV1C_SetSlaveAddr(ND06AV1C_Dev_t *pNxDevice, uint8_t addr);

/** 获取ND06AV1C的设备地址 */
int32_t ND06AV1C_GetSlaveAddr(ND06AV1C_Dev_t *pNxDevice, uint8_t *addr);

/** 设置当前ND06AV1C的积分时间 */
int32_t ND06AV1C_SetCurrentExp(ND06AV1C_Dev_t *pNxDevice, uint16_t current_exp);

/** 获取当前ND06AV1C的积分时间 */
uint16_t ND06AV1C_GetCurrentExp(ND06AV1C_Dev_t *pNxDevice);

/** 获取当前ND06AV1C的温度 */
int16_t ND06AV1C_GetCurrentTherm(ND06AV1C_Dev_t *pNxDevice);

/** 获取当前ND06AV1C的驱动温度 */
int16_t ND06AV1C_GetDriverTherm(ND06AV1C_Dev_t *pNxDevice);

/** 频率标定 */
int32_t ND06AV1C_FreqCalib(ND06AV1C_Dev_t *pNxDevice);

/** 设置offset标定距离 */
int32_t ND06AV1C_SetCalibDis(ND06AV1C_Dev_t *pNxDevice,uint32_t calib_dis);

/** 设置or获取自动曝光状态 */
int32_t ND06AV1C_SetOrGetAutoExpStatus(ND06AV1C_Dev_t *pNxDevice,uint32_t *status,uint8_t set_flag);

/** 设置or获取自动曝光最大积分时间 */
int32_t ND06AV1C_SetOrGetMaxExp(ND06AV1C_Dev_t *pNxDevice,uint32_t *max_exp,uint8_t set_flag);

/** 设置or获取自动曝光目标幅度值 */
int32_t ND06AV1C_SetOrGetAmpTarget(ND06AV1C_Dev_t *pNxDevice,uint32_t *amp_tag,uint8_t set_flag);

/** 设置or获取HDR目标幅度值 */
int32_t ND06AV1C_SetOrGetHDRAmpTag(ND06AV1C_Dev_t *pNxDevice,uint32_t *hdr_amp_tag,uint8_t set_flag);

/** 设置or获取扩展测距模式状态 */
int32_t ND06AV1C_SetOrGetExtendedRangingModeStatus(ND06AV1C_Dev_t *pNxDevice,uint32_t *status,uint8_t set_flag);

/** 设置内部滤波器触发时间 */
int32_t ND06AV1C_SetOrGetFilterTime(ND06AV1C_Dev_t *pNxDevice,uint32_t *time,uint8_t set_flag);

/** ND06AV1C标定 */
int32_t ND06AV1C_OffsetCalib(ND06AV1C_Dev_t *pNxDevice,int32_t *err_flag);

/** 延时时间（ms） */
void ND06AV1C_Delay1ms(uint32_t ms);

/** 延时时间（10us） */
void ND06AV1C_Delay10us(uint32_t us);

/** 设置xshut引脚的电平 */
void ND06AV1C_SetXShutPinLevel(ND06AV1C_Dev_t *pNxDevice, uint32_t level);

/** 硬件待机 */
void ND06AV1C_HardwareSleep(ND06AV1C_Dev_t *pNxDevice);

/** 硬件待机唤醒 */
void ND06AV1C_HardwareWakeup(ND06AV1C_Dev_t *pNxDevice);

/** 初始化ND06AV1C设备 */
int32_t ND06AV1C_InitDevice(ND06AV1C_Dev_t *pNxDevice);

/** 获取ND06AV1C模组固件版本号 */
int32_t ND06AV1C_GetFirmwareVersion(ND06AV1C_Dev_t *pNxDevice, uint32_t* pFirmwareDataBuf);

/** 获取设备当前信息 */
int32_t ND06AV1C_GetDevInfo(ND06AV1C_Dev_t *pNxDevice);

/** 等待ND06AV1C模组启动 */
int32_t ND06AV1C_WaitDeviceBootUp(ND06AV1C_Dev_t *pNxDevice,uint32_t boot_flag);

/** ND06AV1C切换模组模式 */
int32_t ND06AV1C_SwitchMode(ND06AV1C_Dev_t *pNxDevice, uint32_t bootflag);

/** ND06AV1C i2c回调函数 */
int32_t ND06AV1C_I2CErrCallback(ND06AV1C_Dev_t *pNxDevice);

/** ND06AV1C 固件升级 */
int32_t ND06AV1C_FirmwareUpdate(ND06AV1C_Dev_t *pNxDevice,ND06AV1C_Fw_t *fw);


/** 以下函数仅支持1.4.2版本固件，后续固件将弃用 */

/** 设置or获取采样次数 */
int32_t ND06AV1C_SetOrGetSampling(ND06AV1C_Dev_t *pNxDevice,uint32_t *Sampling,uint8_t set_flag);

/** 设置环境光场景积分时间增益 */
int32_t ND06AV1C_SetOrGetExpPro(ND06AV1C_Dev_t *pNxDevice,uint32_t *param,uint8_t set_flag);

/** 设置高环境光场景配置 */
int32_t ND06AV1C_SetLightConfig(ND06AV1C_Dev_t *pNxDevice,uint32_t mode);

/** @} ND06AV1C_Dev_Group */

#endif

