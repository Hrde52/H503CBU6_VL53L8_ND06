/**
 * @file nd06av1c_data.c
 * @author huiyang.jiang
 * @brief ND06AV1C get depth data functions
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
#include "stm32h5xx_hal.h"

static uint8_t config_flag = 0;
static uint8_t config_addr[20] = {0};
static uint8_t config_type[20] = {0};
static uint32_t config_user[sizeof(config_addr)];


/**
 * @brief ND06AV1C Wait Depth And Amp Buf Ready
 *        等待ND06AV1C测距完成
 * @param   pNxDevice 模组设备
 * @return  int32_t
 * @retval  函数执行结果
 * - 0：正常返回
 * - ::ND06AV1C_ERROR_TIME_OUT:获取数据超时
 */
uint32_t readDataWaitCnt = 2000;
int32_t ND06AV1C_WaitDepthAndAmpBufReady(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    buf_valid_flag = 0x0;

    //ND06AV1C_Delay1ms(1);
    //while(i)
    //{
    ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, &buf_valid_flag);
    if(ND06AV1C_DEPTH_DATA_VAL_MASK == (buf_valid_flag & ND06AV1C_DEPTH_DATA_VAL_MASK))
    {
			ret = ND06AV1C_DATA_READY;
      readDataWaitCnt = 2000;
    }

        //break;
    //ND06AV1C_Delay10us(50);
//        i--;
    //}

    if(readDataWaitCnt-- == 0)
    {
        readDataWaitCnt = 2000;
        return ND06AV1C_ERROR_TIME_OUT;
    }

    return ret;
}


int32_t ND06AV1C_WaitNd06Ready(ND06AV1C_Dev_t *pNxDevice,uint32_t req_val)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    buf_valid_flag = 0x0;
    uint32_t    i = 2000;


    ND06AV1C_Delay1ms(1);
    while(i)
    {
        ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, &buf_valid_flag);
        if(req_val == (buf_valid_flag & req_val))
            break;
        ND06AV1C_Delay10us(50);
        i--;
    }

    if(i == 0)
    {
        ND06AV1C_WriteWord(pNxDevice,ND06AV1C_REG_DATA_VAL_REQ,0);
        return ND06AV1C_ERROR_TIME_OUT;
    }

    ND06AV1C_WriteWord(pNxDevice,ND06AV1C_REG_DATA_VAL_REQ,0);
    return ret;
}

/**
 * @brief 获取配置地址和数据类型
 *
 * @param pNxDevice
 * @return int32_t
 */
static int32_t ND06AV1C_GetConfigAddrAndType(ND06AV1C_Dev_t *pNxDevice)
{
    uint32_t i;
    int32_t ret = ND06AV1C_ERROR_NONE;

    ret = ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_CONFIG); //切换到标定模式固件

    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_GET_CONFIG_ARRD_REQ_MASK); //发送触发信号
    ret |= ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_GET_CONFIG_ARRD_VAL_MASK);                     //等待模组响应请求
    ret |= ND06AV1C_ReadNWords(pNxDevice,0xA640 + ND06AV1C_REG_OFS,(uint32_t *)config_addr,sizeof(config_addr)/4);
    ret |= ND06AV1C_ReadNWords(pNxDevice,0xA640 + ND06AV1C_REG_OFS + 40,(uint32_t *)config_type,sizeof(config_type)/4);

    if(ret == ND06AV1C_ERROR_NONE)
    {
        config_flag = 1;
        for(i = 0;i < sizeof(config_type);i ++)
            config_user[i] = 0xffffffff;
    }

    ret |= ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_RUN); //切换到运行模式

    return ret;
}



/**
 * @brief ND06AV1C Start Measurement
 *        发送开始测量信号
 * @param   pNxDevice 模组设备
 * @return  int32_t
 */
int32_t ND06AV1C_StartMeasurement(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    data_req_flag = 0x0;

    ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, &data_req_flag);
    data_req_flag = data_req_flag | ND06AV1C_DEPTH_DATA_REQ_MASK;
    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, data_req_flag); //发送触发信号

    return ret;
}


/**
 * @brief ND06AV1C Clear Data Valid Flag
 *        清除ND06AV1C测量数据的有效位，取完一次数
 *        据做的操作，通知ND06AV1C数据已读取
 * @param   pNxDevice 模组设备
 * @return  int32_t
 */
int32_t ND06AV1C_ClearDataValidFlag(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    data_valid_flag = 0;

    ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, &data_valid_flag);
    data_valid_flag = data_valid_flag & (~ND06AV1C_DEPTH_DATA_VAL_MASK);
    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, data_valid_flag);
    return ret;
}

/**
 * @brief ND06AV1C获取深度幅度
 *
 * @param pNxDevice ND06AV1C模组设备
 * @param amp 幅度值
 * @param dep 深度值
 * @return int32_t
 */
//uint16_t test_i = 0;

uint8_t position65300[16] = {0};
uint8_t lastPosition65300[16] = {0};
//uint32_t begin65300Time = 0;
static uint32_t startTime65300[16] = {0};
static uint8_t was65300[16] = {0};
uint8_t over30s = 0;
uint16_t ND06AV1C_WaitDataReadyFlag = 0;
int32_t ND06AV1C_GetDepthAndAmpData(ND06AV1C_Dev_t *pNxDevice,uint16_t *amp,uint16_t *dep)
{
	uint32_t ret = 0;
	uint16_t i = 0;
	uint16_t j = 0;

	uint16_t amp_temp[4][4];
	uint16_t dep_temp[4][4];

	if(ND06AV1C_WaitDataReadyFlag == 0)
	{
		ret = ND06AV1C_StartMeasurement(pNxDevice);
		if(ret == ND06AV1C_ERROR_NONE)
		{
		  //printf("Measurement in progress...\r\n");
			ND06AV1C_WaitDataReadyFlag = 1;
		}
	}
  else // ND06AV1C_WaitDataReadyFlag == 1
	{
	    
	    ret = ND06AV1C_WaitDepthAndAmpBufReady(pNxDevice);
			if(ret == ND06AV1C_DATA_READY)
			{
				ND06AV1C_ReadNWords(pNxDevice,0xA620 + ND06AV1C_REG_OFS,(uint32_t *)dep_temp,8);
				ND06AV1C_ReadNWords(pNxDevice,0xA6E0 + ND06AV1C_REG_OFS,(uint32_t *)amp_temp,8);
				for(i = 0;i < 4;i++)
				{
						for(j = 0;j < 4;j++)
						{
								*(amp + (3 - i) * 4 + j) = amp_temp[i][j];
								*(dep + (3 - i) * 4 + j) = dep_temp[i][j];
							
							// add E002 65300
							uint8_t idx = (3 - i) * 4 + j;
        
							if(dep_temp[i][j] == 65300)
							{
									if(was65300[idx] == 0)
									{
											startTime65300[idx] = HAL_GetTick();
											was65300[idx] = 1;
									}
									else if(HAL_GetTick() - startTime65300[idx] >= 3000)
									{
											over30s = 1;  // 
									}
							}
							else
							{
									was65300[idx] = 0;
									startTime65300[idx] = 0;
							}
							if(over30s == 1)
							{
									uint8_t anyStillOver30s = 0;
									for(uint8_t k = 0; k < 16; k++)
									{
											if(was65300[k] == 1 && HAL_GetTick() - startTime65300[k] >= 30000)
											{
													anyStillOver30s = 1;
													break;
											}
									}
									
									if(anyStillOver30s == 0)
									{
											over30s = 0;
									}
							}

						}
				 }
					/* 清除数据有效标志位 */
//			    ret |= ND06AV1C_ClearDataValidFlag(pNxDevice);
					ND06AV1C_ClearDataValidFlag(pNxDevice);
					ret = ND06AV1C_GET_DATA_SUCCESS;
					ND06AV1C_WaitDataReadyFlag = 0;
				 // printf("test_i = %d\r\n",test_i);
				//  test_i = 0;
				}

    }

    return ret;
}



/**
 * @brief ND06AV1Citof数据恢复用户配置的数据
 *
 * @param pNxDevice
 * @return int32_t
 */
int32_t ND06AV1C_DataRecovery(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t ret = ND06AV1C_ERROR_NONE;
    uint32_t addr_base;
    uint32_t data;
    uint32_t i;

    if(config_flag == 0)
        return 0;

    for(i = 0;i < sizeof(config_type);i ++)
    {
        if(config_user[i] != 0xffffffff)
        {
            ret |= ND06AV1C_ReadWord(pNxDevice,ND06AV1C_REG_ITOFDATA,&addr_base);
            addr_base = (addr_base & 0xffff) | 0x8000;
            addr_base += config_addr[i];

            ret |= ND06AV1C_ReadWord(pNxDevice,(addr_base / 4) * 4,&data);
            if(config_type[i] == 4)
            {
                data = config_user[i];
            }
            else if(config_type[i] == 2)
            {
                data &= ~(0xffff << ((addr_base % 4) * 8));
                data |= config_user[i] << ((addr_base % 4) * 8);
            }
            else if(config_type[i] == 1)
            {
                data &= ~(0xff << ((addr_base % 4) * 8));
                data |= config_user[i] << ((addr_base % 4) * 8);
            }

            ret |= ND06AV1C_WriteWord(pNxDevice,(addr_base / 4) * 4,data);

        }
    }

    return ret;
}


/**
 * @brief ND06AV1C保存模组内部的itof数据到flash
 *
 * @param pNxDevice
 * @return int32_t
 */
int32_t ND06AV1C_SaveItofData(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t ret = ND06AV1C_ERROR_NONE;
    ret = ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_CONFIG);
    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_SAVE_CONFIG_ARRD_REQ_MASK);
    ret |= ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_SAVE_CONFIG_ARRD_VAL_MASK);
    ret |= ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_RUN);

    return ret;
}


/**
 * @brief ND06AV1C设置配置数据
 *
 * @param pNxDevice ND06AV1C模组设备
 * @param config 配置项目
 * @param addr_offset 地址偏移量
 * @param data 数据
 * @return int32_t
 */
int32_t ND06AV1C_SetConfigData(ND06AV1C_Dev_t *pNxDevice,ND06AV1C_Config_Options config,uint32_t addr_offset,uint32_t data)
{
    int32_t ret = ND06AV1C_ERROR_NONE;
    uint32_t addr_base;
    uint32_t data_temp;

    if(config_flag == 0)
    {
        ret |= ND06AV1C_GetConfigAddrAndType(pNxDevice);
    }

    ret |= ND06AV1C_ReadWord(pNxDevice,ND06AV1C_REG_ITOFDATA,&addr_base);
    addr_base = (addr_base & 0xffff) | 0x8000;
    addr_base += config_addr[config] + addr_offset;

    ret |= ND06AV1C_ReadWord(pNxDevice,(addr_base / 4) * 4,&config_user[config]);
    if(config_type[config] == 4)
    {
        config_user[config] = data;
    }
    else if(config_type[config] == 2)
    {
        config_user[config] &= ~(0xffff << ((addr_base % 4) * 8));
        config_user[config] |= data << ((addr_base % 4) * 8);
    }
    else if(config_type[config] == 1)
    {
        config_user[config] &= ~(0xff << ((addr_base % 4) * 8));
        config_user[config] |= data << ((addr_base % 4) * 8);
    }

    ret |= ND06AV1C_WriteWord(pNxDevice,(addr_base / 4) * 4,config_user[config]);
    config_user[config] = data;
    ND06AV1C_Delay10us(10);
    if(ret == ND06AV1C_ERROR_NONE)
    {
        ND06AV1C_GetConfigData(pNxDevice,config,addr_offset,&data_temp);
        if(data_temp == data)
            return ret;
        else
            return ND06AV1C_ERROR_I2C;
    }

    return ret;
}


/**
 * @brief ND06AV1C获取配置数据
 *
 * @param pNxDevice ND06AV1C模组设备
 * @param config 配置项目
 * @param addr_offset 地址偏移量
 * @param data 数据
 * @return int32_t
 */
int32_t ND06AV1C_GetConfigData(ND06AV1C_Dev_t *pNxDevice,ND06AV1C_Config_Options config,uint32_t addr_offset, uint32_t *data)
{

    int32_t ret = ND06AV1C_ERROR_NONE;
    uint32_t addr_base;
    uint32_t addr_data;
    uint8_t  *uint8 = (uint8_t *)&addr_data;
    uint16_t *uint16 = (uint16_t *)&addr_data;
    uint32_t *uint32 = (uint32_t *)&addr_data;

    if(config_flag == 0)
    {
        ret |= ND06AV1C_GetConfigAddrAndType(pNxDevice);
    }

    ret |= ND06AV1C_ReadWord(pNxDevice,ND06AV1C_REG_ITOFDATA,&addr_base);
    addr_base = (addr_base & 0xffff) | 0x8000;
    addr_base += config_addr[config] + addr_offset;

    if(addr_base % 4 != 0)
    {
        uint16 += 1;
        uint8 += addr_base % 4;
        addr_base = (addr_base / 4) * 4;
    }
    ret |= ND06AV1C_ReadWord(pNxDevice,addr_base,&addr_data);

    if(config_type[config] == 1)
    {
        *data = *uint8;
    }
    else if(config_type[config] == 2)
    {
        *data = *uint16;
    }
    else if(config_type[config] == 4)
    {
        *data = *uint32;
    }
    else
        return -1;


    return ret;
}




/**
 * @brief 恢复标定数据以及配置数据为出厂数据
 *
 * @return int32_t
 */
int32_t ND06AV1C_ResetTofData(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t ret = ND06AV1C_ERROR_NONE;
    uint32_t ret_data;

    ret = ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_CONFIG);

    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_CALIB_RESET_REQ_MASK);
    ret |= ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_CALIB_RESET_VAL_MASK);
    ret |= ND06AV1C_ReadWord(pNxDevice,ND06AV1C_REG_UNIVERSAL_DATA,&ret_data);

    ret = ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_RUN);

    if(ret == ND06AV1C_ERROR_NONE)
    {
        return ret_data;
    }

    return ret;
}
