
#include <stdio.h>
#include "platform.h"
#include "nd06av1c_comm.h"
#include "nd06av1c_dev.h"
#include "nd06av1c_data.h"
#include "nd06av1c_def.h"

/* 单次数据获取 */
void ND06AV1C_RangingTest(ND06AV1C_Dev_t *pNxDevice);

/* ND06AV1C模组设备结构体 */
ND06AV1C_Dev_t g_nd06av1c_device = {
    .i2c_dev_addr = ND06AV1C_DEFAULT_SLAVE_ADDR, 
    .SetXShutPinLevelFunc = set_xshut_pin_level
};

/* 函数指针结构体 */
ND06AV1C_Func_Ptr_t dev_op = {NULL, NULL, NULL, NULL}; 


int main(void)
{
    dev_op.Delay10usFunc = delay_10us;
	dev_op.Delay1msFunc = delay_1ms;
    dev_op.GetNowTimeUsFunc = get_now_time_us;     /* Note: 如果不进行频率标定则可以不实现get_now_time_us函数，此函数仅在对ND06AV1C模组频率进行标定时才使用 */
	dev_op.I2C_ReadNBytesFunc = i2c_read_nbytes;
	dev_op.I2C_WriteNBytesFunc = i2c_write_nbytes;

	ND06AV1C_RegisteredPlatformApi(dev_op);
    
    /* 循环等待设备启动, 若模组或者IIC读写函数有问题则会报错 */
    if(ND06AV1C_ERROR_NONE != ND06AV1C_WaitDeviceBootUp(&g_nd06av1c_device,ND06AV1C_BOOTFLAG_RUN))
	{
		NX_PRINTF("ND06AV1C_WaitDeviceBootUp error\r\n");
		return -1;
	}

	/* 初始化模组设备 */
	if(ND06AV1C_ERROR_NONE != ND06AV1C_InitDevice(&g_nd06av1c_device))
	{
		NX_PRINTF("ND06AV1C_InitDevice error!!\r\n");
		return -1;
	}


    //循环获取测量信息
    while(1)
    {
		NX_PRINTF("\033[0;0H");
        ND06AV1C_RangingTest(&g_nd06av1c_device);
    }

    return 0;
}


/**
 * @brief 单次数据获取
 * 
 * @param pNxDevice 
 */
void ND06AV1C_RangingTest(ND06AV1C_Dev_t *pNxDevice)
{
    int32_t	 ret = ND06AV1C_ERROR_NONE;
    uint16_t amp[16];
    uint16_t dep[16];
	uint8_t i,j;

    ret = ND06AV1C_GetDepthAndAmpData(pNxDevice,(uint16_t *)amp,(uint16_t *)dep);

    if(ret == ND06AV1C_ERROR_NONE)
    {
        NX_PRINTF("amp:\r\n");
        for( i = 0;i < 4;i ++)
        {
            for( j = 0;j < 4;j ++)
            {
                NX_PRINTF("%4d\t",amp[i*4+j]);
            }

            NX_PRINTF("\r\n");
        }
        
        NX_PRINTF("\r\ndep:\r\n");
        for( i = 0;i < 4;i ++)
        {
            for( j = 0;j < 4;j ++)
            {
                NX_PRINTF("%5d\t",dep[i*4+j]);
            }
            
            NX_PRINTF("\r\n");
        }
    }
    else
    {
        LOG_ERR("nd06av1c get depinfo fail!\r\n");
    }
	
}