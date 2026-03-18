#include "nd06av1c_app.h"
#include "nd06av1c_comm.h"
#include "nd06av1c_dev.h"
#include "nd06av1c_data.h"
#include "nd06av1c_def.h"
#include "platform.h"


/* ND06AV1C */
ND06AV1C_Dev_t g_nd06av1c_device = {
    .i2c_dev_addr = ND06AV1C_DEFAULT_SLAVE_ADDR,
    .SetXShutPinLevelFunc = set_xshut_pin_level
};

void ND06AV1C_OFF(void)
{
	HAL_GPIO_WritePin(ND06_EN_GPIO_Port, ND06_EN_Pin, GPIO_PIN_RESET);       
  HAL_Delay(500);
}

void ND06AV1C_ON(void)
{
	HAL_GPIO_WritePin(ND06_EN_GPIO_Port, ND06_EN_Pin, GPIO_PIN_SET);       
  HAL_Delay(500);
}

void ND06_Reset(void)
{
    HAL_GPIO_WritePin(ND06_EN_GPIO_Port, ND06_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);

    HAL_GPIO_WritePin(ND06_EN_GPIO_Port, ND06_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(500);
}
/* */
ND06AV1C_Func_Ptr_t dev_op = {NULL, NULL, NULL, NULL};

void ND06AV1C_Init(void)
{
	dev_op.Delay10usFunc = delay_10us;
	dev_op.Delay1msFunc = delay_1ms;
	dev_op.GetNowTimeUsFunc = get_now_time_us;     
	dev_op.I2C_ReadNBytesFunc = i2c_read_nbytes;
	dev_op.I2C_WriteNBytesFunc = i2c_write_nbytes;
	ND06AV1C_RegisteredPlatformApi(dev_op);

	if(ND06AV1C_ERROR_NONE != ND06AV1C_WaitDeviceBootUp(&g_nd06av1c_device,ND06AV1C_BOOTFLAG_RUN))
	{
		;
//		printf("ND06AV1C_WaitDeviceBootUp error\r\n");
		#if(DebugPrintfFlag == 1)
//	    printf("ND06AV1C_WaitDeviceBootUp error\r\n");
		#endif
	    //return -1;
	}

	if(ND06AV1C_ERROR_NONE != ND06AV1C_InitDevice(&g_nd06av1c_device))
	{
		;
//		printf("ND06AV1C_WaitDeviceBootUp error\r\n");
		#if(DebugPrintfFlag == 1)
//	    printf("ND06AV1C_InitDevice error!!\r\n");
		#endif
	    //return -1;
	}
}

//int32_t ND06AV1C_Ranging(struct ND06_DATA nd06_data)
int32_t ND06AV1C_Ranging(ND06_DATA *nd06_data, uint8_t used_i, uint8_t used_j, uint8_t *nd06objDetectFlag)
{
  int32_t	 ret = ND06AV1C_ERROR_NONE;
	uint8_t i, j, timesND06Failed;
	uint16_t objDetectCnt = 0;
	ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, nd06_data->amp, nd06_data->dep);

	if(ret == ND06AV1C_GET_DATA_SUCCESS)
	{
		timesND06Failed = 0;
		nd06OutputEN = 1;
		num_of_pixel_occluded = 0;
		for (uint8_t i = 0; i < 4; i++) // i = 2;i < 4;i ++
		{
			for (uint8_t j = work_j; j < 4-work_j; j++) // j = 0;j < 4;j ++
			{
				if ((nd06_data->dep[i * 4 + j] >= 100) &&
						(nd06_data->dep[i * 4 + j] <= 1000))
						num_of_pixel_occluded++;
			}
		}
		if ((num_of_pixel_occluded >= 1) && (nd06OutputEN == 1))
		{
			nd06AV1C_objDetectFlag = 1;
		}
		else
		{
			nd06AV1C_objDetectFlag = 0;
		}

	}
	else if(ret == ND06AV1C_ERROR_NONE)
	{
		ND06AV1C_ON();
//		printf("do nothing!\r\n");
	}
	else
	{
		if (timesND06Failed++ > 3)
		{
//		            E001 = 1;
			timesND06Failed = 0;
			ND06_Reset();
			ND06AV1C_Init();
		}
//		printf("nd06av1c get dataInfo fail!\r\n");
	}
	return ret;
}








































