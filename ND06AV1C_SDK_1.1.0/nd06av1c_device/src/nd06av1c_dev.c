/**
 * @file nd06av1c_dev.c
 * @author huiyang.jiang
 * @brief ND06AV1C device setting functions
 * @version 1.x.x
 * @date 2023-5
 *
 * @copyright Copyright (c) 2023, Shenzhen Nephotonics Inc.
 *
 */

#include "nd06av1c_comm.h"
#include "nd06av1c_dev.h"
#include "nd06av1c_data.h"
#include "nd06av1c_def.h"



/** SDK主版本 */
static uint8_t sdk_version_major = 1;
/** SDK次版本 */
static uint8_t sdk_version_minor = 1;
/** SDK小版本 */
static uint8_t sdk_version_patch = 0;


//Function ptr
/** 延时1ms函数指针 */
void(*Delay1msLib)(uint32_t );
/** 延时10us函数指针 */
void(*Delay10usLib)(uint32_t );
/** 获取当前系统时间 */
uint64_t(*GetNowTimeUsLib)(void );


/**
 * @brief ND06AV1C Get SDK Version
 *        获取当前SDK的软件版本号
 * @return  uint32_t
 * @retval  软件版本号
 */
uint32_t ND06AV1C_GetSdkVersion(void)
{
    return (uint32_t)sdk_version_major * 10000 + (uint32_t)sdk_version_minor * 100 + (uint32_t)sdk_version_patch;
}


/**
 * @brief ND06AV1C Registered Platform Api
 *        初始化ND06AV1C库的API
 * @param   dev_op   ND06AV1C库的API入口地址
 * @return  void
 */
void ND06AV1C_RegisteredPlatformApi(ND06AV1C_Func_Ptr_t dev_op)
{
    ND06AV1C_RegisteredIICApi(dev_op.I2C_WriteNBytesFunc, dev_op.I2C_ReadNBytesFunc);
    Delay1msLib = dev_op.Delay1msFunc;
    Delay10usLib = dev_op.Delay10usFunc;
    GetNowTimeUsLib = dev_op.GetNowTimeUsFunc;
}

/**
 * @brief ND06AV1C Set Slave Address
 *        设置ND06AV1C的设备地址
 * @param pNxDevice: ND06AV1C模组设备信息结构体指针
 * @param addr: 配置ND06AV1C模组的i2c设备地址
 * @return int32_t
*/
int32_t ND06AV1C_SetSlaveAddr(ND06AV1C_Dev_t *pNxDevice, uint8_t addr)
{
	int32_t ret = ND06AV1C_ERROR_NONE;

	ret = ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_SLAVE_ADDR, (uint32_t)addr);

    if(ret == ND06AV1C_ERROR_NONE)
	    pNxDevice->i2c_dev_addr = addr;

    return ret;
}

/**
 * @brief ND06AV1C Get Slave Address
 *        获取ND06AV1C的设备地址
 * @param pNxDevice: ND06AV1C模组设备信息结构体指针
 * @param addr: 获取的ND06AV1C模组i2c设备地址
 * @return int32_t
*/
int32_t ND06AV1C_GetSlaveAddr(ND06AV1C_Dev_t *pNxDevice, uint8_t *addr)
{
    int32_t ret = ND06AV1C_ERROR_NONE;

    *addr = pNxDevice->i2c_dev_addr;

    return ret;
}

/**
 * @brief ND06AV1C Set Current Exposure
 *        设置当前ND06AV1C的积分时间
 * @param   pNxDevice   模组设备
 * @param   current_exp 当前积分时间/us
 * @return  uint32_t
 */
int32_t ND06AV1C_SetCurrentExp(ND06AV1C_Dev_t *pNxDevice, uint16_t current_exp)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    rbuf;
    ret = ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_EXP_THERM, &rbuf);

    if(ND06AV1C_ERROR_NONE == ret){
        rbuf &= 0x0000FFFF;
        rbuf |= ((uint32_t)current_exp << 16);
        ret = ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_EXP_THERM, rbuf);
    }
	return current_exp;
}

/**
 * @brief ND06AV1C Get Current Exposure
 *        获取当前ND06AV1C的积分时间
 * @param   pNxDevice   模组设备
 * @return  uint32_t    当前积分时间/us
 */
uint16_t ND06AV1C_GetCurrentExp(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    rbuf;
    uint16_t    current_exp = 0;

    ret = ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_EXP_THERM, &rbuf);
    if(ND06AV1C_ERROR_NONE == ret){
        current_exp = (rbuf >> 16) & 0xFFFF;
    }

	return current_exp;
}


/**
 * @brief ND06AV1C Get Current Therm
 *        获取当前ND06AV1C的温度
 * @param   pNxDevice   模组设备
 * @return  int32_t     当前温度/0.1度
 */
int16_t ND06AV1C_GetCurrentTherm(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    rbuf;
    int16_t     current_therm = 0;

    ret = ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_EXP_THERM, &rbuf);
    if(ND06AV1C_ERROR_NONE == ret){
        current_therm = rbuf & 0xFFFF;
    }

	return current_therm;
}


/**
 * @brief ND06AV1C Get Current Therm
 *        获取当前ND06AV1C的驱动温度
 * @param   pNxDevice   模组设备
 * @return  int32_t     当前温度
 */
int16_t ND06AV1C_GetDriverTherm(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    rbuf;
    int16_t     current_therm = 0;

    ret = ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_DRIVER_THERM, &rbuf);
    if(ND06AV1C_ERROR_NONE == ret){
        current_therm = rbuf & 0xFFFF;
    }

	return current_therm;
}

/**
 * @brief ND06AV1C Delay 1ms
 * @param   ms 延时时间
 * @return  void
 */
void ND06AV1C_Delay1ms(uint32_t ms)
{
    Delay1msLib(ms);
}

/**
 * @brief ND06AV1C Delay 10us
 * @param   us 延时时间
 * @return  void
 */
void ND06AV1C_Delay10us(uint32_t us)
{
    Delay10usLib(us);
}

/**
 * @brief ND06AV1C Set XShut Pin Level
 *        设置xshut引脚的电平
 * @param   pNxDevice   模组设备
 * @param   level    xshut引脚电平，0为低电平，1为高电平
 * @return  void
 */
void ND06AV1C_SetXShutPinLevel(ND06AV1C_Dev_t *pNxDevice, uint32_t level)
{
    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        pNxDevice->SetXShutPinLevelFunc(level);
}

/**
 * @brief ND06AV1C Hardware Sleep
 *        硬件待机
 * @param   pNxDevice   模组设备
 * @return  void
 */
void ND06AV1C_HardwareSleep(ND06AV1C_Dev_t *pNxDevice)
{
    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        pNxDevice->SetXShutPinLevelFunc(0);
}

/**
 * @brief ND06AV1C Hardware Wakeup
 *        从硬件待机中唤醒
 * @param   pNxDevice   模组设备
 * @return  void
 */
void ND06AV1C_HardwareWakeup(ND06AV1C_Dev_t *pNxDevice)
{
    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        pNxDevice->SetXShutPinLevelFunc(1);
    ND06AV1C_REG_OFS = 0;
}


/**
 * @brief ND06AV1C InitDevice
 *        初始化设备
 * @param   pNxDevice   模组设备
 * @return  int32_t
 */
int32_t ND06AV1C_InitDevice(ND06AV1C_Dev_t *pNxDevice)
{
	int32_t     ret = ND06AV1C_ERROR_NONE;
	#if(DebugPrintfFlag==1)
    NX_PRINTF("ND06AV1C_InitDevice Start!\r\n");
	#endif
    /** 获取设备信息 */
	ret |= ND06AV1C_GetDevInfo(pNxDevice);
	#if(DebugPrintfFlag==1)
    NX_PRINTF("ND06AV1C_InitDevice End!\r\n");
	#endif

	return ret;
}

/**
 * @brief ND06AV1C Get Firmware Version
 *        获取ND06AV1C模组固件版本号
 * @param   pNxDevice   模组设备
 * @param   pFirmwareDataBuf  固件版本数据指针
 * @return  int32_t
 */
int32_t ND06AV1C_GetFirmwareVersion(ND06AV1C_Dev_t *pNxDevice, uint32_t* pFirmwareDataBuf)
{
	int32_t     ret = ND06AV1C_ERROR_NONE;

    ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_FW_VERSION, pFirmwareDataBuf);

	return ret;
}


/**
 * @brief ND06AV1C Get Device Information
 *        获取设备当前信息
 * @param   pNxDevice   模组设备
 * @return  void
 */
int32_t ND06AV1C_GetDevInfo(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;

    /** 读取ND06AV1C模组固件版本号 */
	ret |= ND06AV1C_GetFirmwareVersion(pNxDevice, &pNxDevice->chip_info.nd06av1c_fw_version);
	#if(DebugPrintfFlag==1)
	NX_PRINTF("FwVersion: V%d.%d.%d-mode%d\r\n", (pNxDevice->chip_info.nd06av1c_fw_version & 0x00ffffff)/10000,
                (pNxDevice->chip_info.nd06av1c_fw_version & 0x00ffffff)/100%100,(pNxDevice->chip_info.nd06av1c_fw_version & 0x00ffffff)%100,(pNxDevice->chip_info.nd06av1c_fw_version >> 24));
    #endif
	return ret;
}

/**
 * @brief ND06AV1C Wait for Device Boot Up
 *        等待ND06AV1C模组启动
 * @param   pNxDevice   模组设备
 * @return  ND06AV1C_ERROR_NONE:成功
 *          ND06AV1C_ERROR_BOOT:启动失败--请检测模组是否焊接好，还有i2c地址与读写函数是否错误。
 *          ND06AV1C_ERROR_FW:固件不兼容--请与FAE联系，是否模组的固件与SDK不兼容。
 */
int32_t ND06AV1C_WaitDeviceBootUp(ND06AV1C_Dev_t *pNxDevice,uint32_t boot_flag)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    int32_t     try_times = 2000;
    uint32_t    rbuf;
    uint32_t    state;
	#if(DebugPrintfFlag==1)
    NX_PRINTF("%s Start!\r\n", __func__);
	#endif

    /* 重启 20250619修改，增加复位时间 */
    ND06AV1C_HardwareWakeup(pNxDevice);
	ND06AV1C_Delay10us(50*100);
    ND06AV1C_HardwareSleep(pNxDevice);
	ND06AV1C_Delay10us(2*1000);
    ND06AV1C_HardwareWakeup(pNxDevice);
	ND06AV1C_Delay10us(50*100);

    /* 判断是否完成启动项配置 */
    ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_DEV_ID, &rbuf);
	#if(DebugPrintfFlag==1)
    NX_PRINTF("ND06AV1C_REG_DEVICE_ID: 0x%08x\r\n", rbuf);
	#endif


    /* 进行启动项配置 */
    if(rbuf != ND06AV1C_REG_DEV_ID_VALUE)
    {
        ret = ND06AV1C_WriteByte(pNxDevice, ND06AV1C_REG_BOOT1, ND06AV1C_REG_BOOT1_VALUE);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_BOOT2, ND06AV1C_REG_BOOT2_VALUE);
        ND06AV1C_Delay10us(30);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_BOOT3, ND06AV1C_REG_BOOT3_VALUE);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_BOOT4, ND06AV1C_REG_BOOT4_VALUE);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_BOOT_FLAG_REG, boot_flag); //启动位置为0x004000位置的固件，init固件
		ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_STATE, 0);
		ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_STATE + 0x80,0);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_BOOT5, ND06AV1C_REG_BOOT5_VALUE);
        if(ND06AV1C_ERROR_NONE != ret)
            return ND06AV1C_ERROR_I2C;
    }

    /* 等待进入工作状态 */
    do{
		ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_STATE, &pNxDevice->dev_pwr_state);
		ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_STATE + 0x80, &state);
        if(state == ND06AV1C_STATE_SOFTWARE_READY || pNxDevice->dev_pwr_state == ND06AV1C_STATE_SOFTWARE_READY)
        {
            if(state == ND06AV1C_STATE_SOFTWARE_READY)
            {
                pNxDevice->dev_pwr_state = state;
                ND06AV1C_REG_OFS = 0x80;
            }
            break;
        }
        // printf("pNxDevice->dev_pwr_state : %x\r\n",pNxDevice->dev_pwr_state);
        ND06AV1C_Delay10us(1);
	}while(--try_times);
	if((ND06AV1C_ERROR_NONE != ret) || (0 == try_times))
    {	
		#if(DebugPrintfFlag==1)
        NX_PRINTF("ND06AV1C boot error\r\n");
		#endif
        return ND06AV1C_ERROR_BOOT;
    }

    ND06AV1C_Delay1ms(50);
	#if(DebugPrintfFlag==1)
    NX_PRINTF("%s End!\r\n", __func__);
	#endif

    return ND06AV1C_ERROR_NONE;
}


/**
 * @brief ND06AV1C切换固件
 *
 * @param pNxDevice
 * @param bootflag
 * @return int32_t
 */
int32_t ND06AV1C_SwitchMode(ND06AV1C_Dev_t *pNxDevice, uint32_t bootflag)
{
    uint32_t try_times = 2000;
    int32_t ret;

    ret = ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_UNIVERSAL_DATA, bootflag);
    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_FW_SW_REQ_MASK);
    ret |= ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_FW_SW_VAL_MASK);

    /* 等待进入工作状态 */
    do{
		ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_STATE, &pNxDevice->dev_pwr_state);
        ND06AV1C_Delay10us(1);
	}while((pNxDevice->dev_pwr_state != ND06AV1C_STATE_SOFTWARE_READY) && (pNxDevice->dev_pwr_state != ND06AV1C_STATE_CALIB)   \
            && --try_times);

	if((ND06AV1C_ERROR_NONE != ret) || (0 == try_times))
    {
		#if(DebugPrintfFlag==1)
        NX_PRINTF("ND06AV1C boot error\r\n");
		#endif
        return ND06AV1C_ERROR_BOOT;
    }
    else
    {
        ND06AV1C_Delay1ms(400);
        ND06AV1C_DataRecovery(pNxDevice);
    }

    return ND06AV1C_ERROR_NONE;
}


/**
 * @brief   ND06_SetOscTrim
 *          ND06设置主时钟电流
 * @param   pNxDevice   模组设备
 * @param   freq        当前时钟频率
 * @param   trim        当前时钟电流
 * @return  int32_t     0: success
 *                     !0: fail
 */
int32_t ND06AV1C_SetOscTrim(ND06AV1C_Dev_t *pNxDevice, uint32_t freq, uint32_t trim)
{

    int32_t     ret = ND06AV1C_ERROR_NONE;
    ret |= ND06AV1C_WriteWord(pNxDevice,ND06AV1C_REG_UNIVERSAL_DATA,trim);
    ret |= ND06AV1C_WriteWord(pNxDevice,ND06AV1C_REG_UNIVERSAL_ADDR,freq);
    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_OSC_DATA_WR_REQ_MASK);
    ret |= ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_OSC_DATA_WR_VAL_MASK);
    return ret;
}



void ND06AV1C_BubbleSort(uint32_t arr[], int n)
{
    int i, j,temp;
    for (i = 0; i < n-1; i++)
    {
        for (j = 0; j < n-i-1; j++)
        {
            if (arr[j] > arr[j+1])
            {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}



/**
 * @brief   ND06_Get Clk
 *          使用ND06内部计数器与外部时间计算出ND06的时钟频率
 * @param   pNxDevice   模组设备
 * @param   run_ms      测试运行时间(>1000ms)
 * @param   p_clk       计算的时钟
 * @return  int32_t     0: success
 *                     !0: fail
 */
static int32_t ND06AV1C_GetClkFreq(ND06AV1C_Dev_t *pNxDevice, uint32_t run_ms, uint32_t *p_clk)
{
    int32_t     ret = ND06AV1C_ERROR_NONE;
    uint32_t    time_start, time_end, mcu_time;
    uint32_t    timer_data[3] = {1, 0, 0};

    if(GetNowTimeUsLib == NULL)
        return -1;

    ret |= ND06AV1C_ReadWord(pNxDevice, 0x3950, &timer_data[2]);
    if(timer_data[2] != 0)
    {
        ret |= ND06AV1C_ReadNWords(pNxDevice, 0x3A1C, &timer_data[0], 2);
        timer_data[0] &= 0x7F;    timer_data[0] += 2;
        timer_data[1] &= 0x3F;    timer_data[1] += 2;
        timer_data[0] = timer_data[0] / (timer_data[2] * 2 * timer_data[1]);
    }
    timer_data[0] = ((1<<6) | ((timer_data[0]-1)<<8));
    ret |= ND06AV1C_WriteWord(pNxDevice, 0x7004, timer_data[0]);
    ret |= ND06AV1C_WriteWord(pNxDevice, 0x700C, 0x00000000);
    ret |= ND06AV1C_WriteWord(pNxDevice, 0x701C, 0x00000001);

    time_start = GetNowTimeUsLib();
    ND06AV1C_Delay1ms(run_ms);
    ret |= ND06AV1C_WriteWord(pNxDevice, 0x701C, 0x00000000);
    time_end = GetNowTimeUsLib();
    ret |= ND06AV1C_ReadWord(pNxDevice, 0x700C, &mcu_time);

    *p_clk = (uint32_t)((float)mcu_time*1000.0/(float)(time_end-time_start));

    return ret;
}



/**
 * @brief   ND06_FreqCalib
 *          ND06频率矫正
 * @param   pNxDevice   模组设备
 * @return  int32_t     0: success
 *                     !0: fail
 */
int32_t ND06AV1C_FreqCalib(ND06AV1C_Dev_t *pNxDevice)
{
	uint32_t last_freq = 4333;
	uint32_t last_trim = 30;
	uint32_t freq[5] = {4000};
	uint32_t osctrim = 26;
    int32_t ret = 0;
    int32_t i;

    ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_CONFIG);
	do
	{
		ret |= ND06AV1C_SetOscTrim(pNxDevice,freq[0],osctrim | 0xffff0000);
		ND06AV1C_Delay1ms(100);
		ret |= ND06AV1C_GetClkFreq(pNxDevice,200,&freq[0]);
        freq[0] /= 100;
		#if(DebugPrintfFlag==1)
        NX_PRINTF("freq: %d, osctrim: %d\r\n",freq[0],osctrim);
		#endif

		if(osctrim > last_trim)
		{
			if((4000 - last_freq) < (freq[0] - 4000))
			{
				osctrim = last_trim;
				freq[0] = last_freq;
			}
            ret |= ND06AV1C_SetOscTrim(pNxDevice,freq[0],osctrim | 0xffff0000);
			break;
		}
		last_trim = osctrim;
		last_freq = freq[0];

		if(freq[0] >= 4000)
			osctrim--;
		if(freq[0] < 4000)
			osctrim++;

        if(osctrim > 30 || osctrim < 15)
            return -1;
	}
	while(1);

    for(i = 0;i < 5;i ++)
    {
        ND06AV1C_WriteWord(pNxDevice,ND06AV1C_REG_UNIVERSAL_ADDR,0xFF00FF00);
        ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_OSC_DATA_WR_REQ_MASK);
        ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_OSC_DATA_WR_VAL_MASK);
        ND06AV1C_Delay1ms(1);
        ND06AV1C_GetClkFreq(pNxDevice,500,&freq[i]);
		#if(DebugPrintfFlag==1)
        NX_PRINTF("freq_work: %d, osctrim: %d\r\n",freq[i],osctrim);
		#endif
        ND06AV1C_Delay1ms(3000);
    }
    ND06AV1C_BubbleSort(freq,5);
	ND06AV1C_WaitDeviceBootUp(pNxDevice,ND06AV1C_BOOTFLAG_CONFIG);
    ret |= ND06AV1C_SetOscTrim(pNxDevice,freq[2],osctrim);
	#if(DebugPrintfFlag==1)
	NX_PRINTF("freq_end: %d,%d  ret:%d\r\n",freq[2],osctrim,ret);
	#endif

    ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_RUN);
    return 0;
}


/**
 * @brief I2C读写错误回调函数
 *
 * @param pNxDevice ND06模组设备
 * @return int32_t
 */
int32_t ND06AV1C_I2CErrCallback(ND06AV1C_Dev_t *pNxDevice)
{
    uint32_t i = 16;
    uint32_t rbuf = 0xffffffff;
    uint8_t data;
    extern int32_t(*I2C_ReadNBytesLib)(uint8_t, uint16_t,uint8_t *, uint8_t);

    while(rbuf != 0x002F0012 && i--)
    {
        I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, ND06AV1C_REG_DEV_ID, &data, 1);
			
//        ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_DEV_ID, &rbuf);   // 
				I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, ND06AV1C_REG_DEV_ID, (uint8_t *)&rbuf, 4);
    }

    if(rbuf != 0x002F0012)
        return ND06AV1C_ERROR_BOOT;
    else
        return ND06AV1C_ERROR_NONE;
}




/**
 * @brief 发送升级固件
 *
 * @param pNxDevice 模组设备
 * @param fw 固件
 * @param fw_len 固件长度
 * @param fw_mode 固件模式
 * @return int32_t
 */
int32_t ND06AV1C_FirmwareSend(ND06AV1C_Dev_t *pNxDevice, uint8_t *fw,uint32_t fw_len,uint32_t fw_mode,uint32_t run_mode)
{
    int32_t ret;
    uint32_t data_load_addr;
    uint32_t sram_sum = 0;
    uint32_t i;
    uint32_t fw_sum = 0;
    uint32_t flashaddr = 0;

    ret = ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_UNIVERSAL_ADDR, 0xFF00FF00);
    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_FW_UP_REQ_MASK);
    ret |= ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_FW_UP_VAL_MASK);
    ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_UNIVERSAL_DATA, &data_load_addr);
	#if(DebugPrintfFlag==1) 
	printf("data_load_addr: 0x%x\r\n",data_load_addr);
	#endif

    while (fw_len >= 256)  //发送固件
    {
        ret |= ND06AV1C_WriteNWords(pNxDevice,data_load_addr + 0 ,(uint32_t *)fw,32);
        fw += 128;
        ret |= ND06AV1C_WriteNWords(pNxDevice,data_load_addr + 128,(uint32_t *)fw,32);
        fw -= 128;

        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_UNIVERSAL_ADDR, 0xFF00FFAA);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_UNIVERSAL_DATA, fw_mode);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_FW_UP_REQ_MASK);
        ret |= ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_FW_UP_VAL_MASK);
        ret |= ND06AV1C_ReadWord(pNxDevice,ND06AV1C_REG_UNIVERSAL_DATA,&sram_sum);

        for(i = 0;i < 256;i ++)
        {
            fw_sum += *(uint8_t *)fw;
            fw ++;
        }
        if(fw_sum != sram_sum)
        {	
			#if(DebugPrintfFlag==1)
            NX_PRINTF("check err!\r\n");
			#endif
            return ND06AV1C_ERROR_FW;
        }

        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_UNIVERSAL_ADDR, flashaddr);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_UNIVERSAL_DATA, fw_mode);
        ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, ND06AV1C_FW_UP_REQ_MASK);
        ret |= ND06AV1C_WaitNd06Ready(pNxDevice,ND06AV1C_FW_UP_VAL_MASK);

        fw_sum = 0;
        fw_len -= 256;
        flashaddr += 256;
		#if(DebugPrintfFlag==1)
        NX_PRINTF("flashaddr: 0x%x fw_len: 0x%x\r\n",flashaddr,fw_len);
		#endif
    }

    if(ret == ND06AV1C_ERROR_NONE)
    {
        ret  = ND06AV1C_WaitDeviceBootUp(pNxDevice,run_mode);
        ret |= ND06AV1C_InitDevice(pNxDevice);
        if(ret == ND06AV1C_ERROR_NONE)
        {
            return ND06AV1C_ERROR_NONE;
        }
        else
        {
			#if(DebugPrintfFlag==1)
            NX_PRINTF("User firmware upgrade Fail!\r\n");
			#endif
            return ND06AV1C_ERROR_NONE;
        }
    }


    return ND06AV1C_ERROR_NONE;
}




/**
 * @brief ND06AV1C固件升级
 *
 * @param pNxDevice 模组设备
 * @param fw_user    用户固件
 * @param fw_config  配置固件
 * @param fw_calib   标定固件
 * @param fw_calib2  标定固件
 * @param fw_init    初始化固件
 * @param fw_user_len
 * @param fw_config_len
 * @param fw_calib_len
 * @param fw_calib2_len
 * @param fw_init_len
 * @return
 */
int32_t ND06AV1C_FirmwareUpdate(ND06AV1C_Dev_t *pNxDevice,ND06AV1C_Fw_t *fw)
{
    uint32_t fw_now_mode;
    int32_t ret = ND06AV1C_ERROR_NONE;
    uint8_t *fw_user    = fw->fw_user;
    uint8_t *fw_config  = fw->fw_config;
    uint8_t *fw_calib   = fw->fw_calib;
    uint8_t *fw_calib2  = fw->fw_calib2;
    uint8_t *fw_init    = fw->fw_init;

    uint32_t fw_user_len   = fw->fw_len[0];
    uint32_t fw_config_len = fw->fw_len[1];
    uint32_t fw_calib_len  = fw->fw_len[2];
    uint32_t fw_calib2_len = fw->fw_len[3];
    uint32_t fw_init_len   = fw->fw_len[4];

    ret  = ND06AV1C_WaitDeviceBootUp(pNxDevice,ND06AV1C_BOOTFLAG_CONFIG);
    ret |= ND06AV1C_InitDevice(pNxDevice);
    if(ret == ND06AV1C_ERROR_NONE && ((pNxDevice->chip_info.nd06av1c_fw_version >> 24) == 255))
    {
        fw_now_mode = ND06AV1C_BOOTFLAG_CONFIG;
        goto ok;
    }

    ret  = ND06AV1C_WaitDeviceBootUp(pNxDevice,ND06AV1C_BOOTFLAG_RUN);  //config_bak
    ret |= ND06AV1C_InitDevice(pNxDevice);
    if(ret == ND06AV1C_ERROR_NONE && ((pNxDevice->chip_info.nd06av1c_fw_version >> 24) == 255))
    {
        fw_now_mode = ND06AV1C_BOOTFLAG_RUN;
        goto ok;
    }
    else
    {
        return ND06AV1C_ERROR_FW;
    }


    ok:; //开始更新固件

    if(fw_now_mode == ND06AV1C_BOOTFLAG_CONFIG)
    {
        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_config,fw_config_len,1,ND06AV1C_BOOTFLAG_RUN);  //备份配置模式，防止升级配置模式变砖
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;
        if((pNxDevice->chip_info.nd06av1c_fw_version >> 24) != 255)  //确认为配置模式
        {
			#if(DebugPrintfFlag==1)
            NX_PRINTF("fw error!\r\n");
			#endif
            return ND06AV1C_ERROR_FW;
        }

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_config,fw_config_len,3,ND06AV1C_BOOTFLAG_RUN);  //升级配置固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_user,fw_user_len,2,ND06AV1C_BOOTFLAG_RUN);  //升级工作固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_calib,fw_calib_len,4,ND06AV1C_BOOTFLAG_CONFIG); //升级标定固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_calib2,fw_calib2_len,5,ND06AV1C_BOOTFLAG_CONFIG); //升级标定2固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_init,fw_init_len,1,ND06AV1C_BOOTFLAG_RUN); //升级初始化固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        return ND06AV1C_ERROR_NONE;
    }
    else //无法正常启动配置模式，可能是上一次升级未完成
    {
		#if(DebugPrintfFlag==1)
        NX_PRINTF("BAK\r\n");
		#endif

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_config,fw_config_len,3,ND06AV1C_BOOTFLAG_CONFIG);  //升级配置模式
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;
        if((pNxDevice->chip_info.nd06av1c_fw_version >> 24) != 255)  //确认为配置模式
        {	
			#if (DebugPrintfFlag==1)
			NX_PRINTF("fw error!\r\n");
			#endif
            return ND06AV1C_ERROR_FW;
        }

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_user,fw_user_len,2,ND06AV1C_BOOTFLAG_RUN);  //升级工作固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_calib,fw_calib_len,4,ND06AV1C_BOOTFLAG_CONFIG); //升级标定固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_calib2,fw_calib2_len,5,ND06AV1C_BOOTFLAG_CONFIG); //升级标定2固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        ret = ND06AV1C_FirmwareSend(pNxDevice,fw_init,fw_init_len,1,ND06AV1C_BOOTFLAG_RUN); //升级初始化固件
        if(ret != ND06AV1C_ERROR_NONE)return ND06AV1C_ERROR_FW;

        return ND06AV1C_ERROR_NONE;

    }


    return ND06AV1C_ERROR_NONE;
}


/**
 * @brief 设置offset标定距离
 *
 * @param pNxDevice 模组设备
 * @param calib_dis 标定距离
 * @return
 */
int32_t ND06AV1C_SetCalibDis(ND06AV1C_Dev_t *pNxDevice,uint32_t calib_dis)
{
    return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_CALIB_DIS,0,calib_dis);
}


/**
 * @brief 设置 or 获取 自动曝光状态
 *
 * @param pNxDevice 模组设备
 * @param status 自动曝光状态 0关闭 1打开 2hdr模式
 * @param set_flag 1 设置 0 获取
 * @return
 */
int32_t ND06AV1C_SetOrGetAutoExpStatus(ND06AV1C_Dev_t *pNxDevice,uint32_t *status,uint8_t set_flag)
{
    if(set_flag)
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_AUTO_EXP,0,*status);
    else
        return ND06AV1C_GetConfigData(pNxDevice,ND06AV1C_CONFIG_AUTO_EXP,0,status);
}

/**
 * @brief 设置 or 获取 HDR目标幅度
 *
 * @param pNxDevice 模组设备
 * @param hdr_amp_tag HDR目标幅度值
 * @param set_flag 1 设置 0 获取
 * @return
 */
int32_t ND06AV1C_SetOrGetHDRAmpTag(ND06AV1C_Dev_t *pNxDevice,uint32_t *hdr_amp_tag,uint8_t set_flag)
{
    if(set_flag)
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_HDR_AMP,0,*hdr_amp_tag);
    else
        return ND06AV1C_GetConfigData(pNxDevice,ND06AV1C_CONFIG_HDR_AMP,0,hdr_amp_tag);
}



/**
 * @brief 设置 or 获取 最大积分时间
 *
 * @param pNxDevice 模组设备
 * @param max_exp 最大积分时间
 * @param set_flag 1 设置 0 获取
 * @return
 */
int32_t ND06AV1C_SetOrGetMaxExp(ND06AV1C_Dev_t *pNxDevice,uint32_t *max_exp,uint8_t set_flag)
{
    if(set_flag)
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_MAX_EXP,0,*max_exp);
    else
        return ND06AV1C_GetConfigData(pNxDevice,ND06AV1C_CONFIG_MAX_EXP,0,max_exp);
}


/**
 * @brief 设置 or 获取 自动曝光目标幅度值
 *
 * @param pNxDevice 模组设备
 * @param amp_tag 自动曝光目标幅度值
 * @param set_flag 1 设置 0 获取
 * @return
 */
int32_t ND06AV1C_SetOrGetAmpTarget(ND06AV1C_Dev_t *pNxDevice,uint32_t *amp_tag,uint8_t set_flag)
{
    if(set_flag)
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_PHS_TAG,0,*amp_tag);
    else
        return ND06AV1C_GetConfigData(pNxDevice,ND06AV1C_CONFIG_PHS_TAG,0,amp_tag);
}


/**
 * @brief 设置 or 获取 采样次数,默认值为1，设置多次采样则会把一次积分拆分为多次
 *
 * @param pNxDevice 模组设备
 * @param Sampling 采样次数
 * @param set_flag 1 设置 0 获取
 * @return
 */
int32_t ND06AV1C_SetOrGetSampling(ND06AV1C_Dev_t *pNxDevice,uint32_t *Sampling,uint8_t set_flag)
{
    if(set_flag)
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_SAMPLING,0,*Sampling);
    else
        return ND06AV1C_GetConfigData(pNxDevice,ND06AV1C_CONFIG_SAMPLING,0,Sampling);
}

/**
 * @brief 设置 or 获取 扩展测距模式状态
 *
 * @param pNxDevice 模组设备
 * @param status   状态
 * @param set_flag 1 设置 0 获取
 * @return
 */
int32_t ND06AV1C_SetOrGetExtendedRangingModeStatus(ND06AV1C_Dev_t *pNxDevice,uint32_t *status,uint8_t set_flag)
{
    if(set_flag)
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_DIS_UNCY,0,*status);
    else
        return ND06AV1C_GetConfigData(pNxDevice,ND06AV1C_CONFIG_DIS_UNCY,0,status);
}


/**
 * @brief 设置高环境光场景配置
 *
 * @param pNxDevice 模组设备
 * @param mode 0 开启检测环境光，自动开启积分时间补偿
 *             1 关闭环境光检测，关闭积分时间补偿
 *             2 关闭检测，始终开启积分时间补偿
 * @note       仅支持1.4.2版本固件，后续弃用
 * @return
 */
int32_t ND06AV1C_SetLightConfig(ND06AV1C_Dev_t *pNxDevice,uint32_t mode)
{
    if(mode == 0)
    {
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_LIGHT,0,0);
    }
    else if(mode == 1)
    {
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_LIGHT,0,(uint32_t)-1);
    }
    else if(mode == 2)
    {
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_LIGHT,0,(uint32_t)-2);
    }
    return ND06AV1C_ERROR_PARAMETER;
}

/**
 * @brief 设置滤波器触发时间阈值，两次测距间隔小于此时间则会经过滤波器输出，设置为0关闭此功能
 * @param pNxDevice 模组设备
 * @param time 最大间隔时间
 * @param set_flag 1 设置 0 获取
 * @return
 */
int32_t ND06AV1C_SetOrGetFilterTime(ND06AV1C_Dev_t *pNxDevice,uint32_t *time,uint8_t set_flag)
{
    if(set_flag)
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_FILTER_TIME,0,*time);
    else
        return ND06AV1C_GetConfigData(pNxDevice,ND06AV1C_CONFIG_FILTER_TIME,0,time);
}


/**
 * @brief 设置环境光场景积分时间增益，在开启环境光增益之后模组会在高环境光场景提高积分时间
 * @param pNxDevice 模组设备
 * @param param 环境光增益 默认开启为30
 * @param set_flag 1 设置 0 获取
 * @note       仅支持1.4.2版本固件，后续弃用
 * @return
 */
int32_t ND06AV1C_SetOrGetExpPro(ND06AV1C_Dev_t *pNxDevice,uint32_t *param,uint8_t set_flag)
{
    if(set_flag)
        return ND06AV1C_SetConfigData(pNxDevice,ND06AV1C_CONFIG_LIGHT_EXP_PRO,0,*param);
    else
        return ND06AV1C_GetConfigData(pNxDevice,ND06AV1C_CONFIG_LIGHT_EXP_PRO,0,param);
}




/**
 * @brief ND06AV1C标定
 *
 * @param pNxDevice  模组设备
 * @param err_flag   错误标志
 * @param calib_flag 标定项目
 * @param ret_val    返回数据
 * @param val        用户参数
 * @return int32_t
 */
int32_t ND06AV1C_Calib(ND06AV1C_Dev_t *pNxDevice,uint32_t calib_flag,int32_t val,uint32_t *ret_val,int32_t *err_flag)
{
    int32_t ret = ND06AV1C_ERROR_NONE;
    int32_t try_times = 200000;
    uint32_t wait_flag = ((calib_flag << 16) & 0xffff0000);

    if(calib_flag == ND06AV1C_CALIB_REQ_MASK)
        ret = ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_CALIB1);
    else
        ret = ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_CALIB2);
    ND06AV1C_WriteWord(pNxDevice,ND06AV1C_REG_UNIVERSAL_DATA,val);

    ret |= ND06AV1C_WriteWord(pNxDevice, ND06AV1C_REG_DATA_VAL_REQ, calib_flag); //发送触发信号
    ret |= ND06AV1C_WaitNd06Ready(pNxDevice,wait_flag);    //等待模组响应请求
    do
    {
        ret |= ND06AV1C_ReadWord(pNxDevice, ND06AV1C_REG_STATE, &pNxDevice->dev_pwr_state);
    } while ((pNxDevice->dev_pwr_state == ND06AV1C_STATE_CALIB) && --try_times);

    ret |= ND06AV1C_ReadWord(pNxDevice,ND06AV1C_REG_UNIVERSAL_DATA,(uint32_t *)ret_val);
    ret |= ND06AV1C_ReadWord(pNxDevice,ND06AV1C_REG_ERROR,(uint32_t *)err_flag);
    ret |= ND06AV1C_SwitchMode(pNxDevice,ND06AV1C_BOOTFLAG_RUN); //切换到运行模式

    return ret;
}


/**
 * @brief ND06AV1C offset标定
 *
 * @param pNxDevice ND06AV1C设备
 * @param err_flag 错误标志位
 * @return int32_t
 */
int32_t ND06AV1C_OffsetCalib(ND06AV1C_Dev_t *pNxDevice,int32_t *err_flag)
{
    int32_t ret = ND06AV1C_ERROR_NONE;
    uint32_t temp;

	ret = ND06AV1C_Calib(pNxDevice,ND06AV1C_CALIB_REQ_MASK,ND06AV1C_OFFSET_ONLY_FLAG, &temp, err_flag);
    return ret;
}
