#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vl53l8cx_api.h"
#include "flash_storage.h"
#include "vl53l8cx_app.h"
#include "sensorParaTable.h"


extern volatile int IntCount; //add Interrupt
#define is_interrupt 1 /*is_interrupt = 1 => get data by interrupt, = 0 => get data by polling */

uint8_t WaitForL8CXInterrupt(VL53L8CX_Configuration * pDev){

	__WFI();
	if(IntCount !=0 ){
		IntCount=0;
		return 1;
	}else{
		return 0;
	}
}


//VL53L8CX_ResultsData 	Results;
uint8_t vl53cx8_device_init(VL53L8CX_Configuration *Dev, uint8_t Resolution, uint8_t freq)
{
	if(freq <= 0 )
	{
		return 0;
	}
	if(Resolution == 4 && freq >30)
	{
		return 0;
	}
	if(Resolution == 8 && freq >15)
	{
		return 0;
	}
	
	/*********************************/
	/*   VL53L8CX ranging variables  */
	/*********************************/
//	static VL53L8CX_Configuration 	Dev;
	
	uint8_t status, isAlive;

	/* USER CODE END PV */

	/*********************************/
	/*      Customer platform        */
	/*********************************/

	/* Fill the platform structure with customer's implementation. For this
	* example, only the I2C address is used.
	*/
	Dev->platform.address = VL53L8CX_DEFAULT_I2C_ADDRESS;

	/* (Optional) Reset sensor toggling PINs (see platform, not in API) */
	VL53L8CX_Reset_Sensor(&(Dev->platform));


	/*********************************/
	/*   Power on sensor and init    */
	/*********************************/

	/* (Optional) Check if there is a VL53L8CX sensor connected */
	status = vl53l8cx_is_alive(Dev, &isAlive);
//	printf("status = %d, isAlive = %d\n", status,isAlive);
	if(!isAlive || status)
	{
//		printf("VL53L8CX not detected at requested address\n");
		return status;
	}

	/* (Mandatory) Init VL53L8CX sensor */
	status = vl53l8cx_init(Dev);
	if(status)
	{
//		printf("VL53L8CX ULD Loading failed\n");
		return status;
	}
#if (DebugPrintfFlag == 1)
//	printf("VL53L8CX ULD Loading succeed\n");
#endif
	if(Resolution == 4)
	{
		status = vl53l8cx_set_resolution(Dev, VL53L8CX_RESOLUTION_4X4);
	}
	else if(Resolution == 8)
	{
		status = vl53l8cx_set_resolution(Dev, VL53L8CX_RESOLUTION_8X8);
	}
	else
	{
		return 0;
	}

	status = vl53l8cx_set_ranging_frequency_hz(Dev, freq);// Set 30Hz ranging frequency
#if 1
	status = vl53l8cx_set_ranging_mode(Dev, VL53L8CX_RANGING_MODE_CONTINUOUS);  // Set mode continuous
#else
	status = vl53l8cx_set_ranging_mode(&Dev, VL53L8CX_RANGING_MODE_AUTONOMOUS);  // Set mode autonomous
	status = vl53l8cx_set_integration_time_ms(&Dev,2);
#endif
//	printf("VL53L8CX ULD ready ! (Version : %s)\n",
//			VL53L8CX_API_REVISION);
	status = vl53l8cx_start_ranging(Dev);
	
	return status;
}

uint8_t vl53cx8_ranging_data(VL53L8CX_Configuration *Dev, VL53L8CX_ResultsData *Results)
{
	uint8_t p_data_ready;
	uint8_t  status, resolution, isAlive, isReady;
		/*********************************/
	/*         Ranging          */
	/*********************************/
	if (is_interrupt) {
		isReady = WaitForL8CXInterrupt(Dev);
		if(isReady)
		{
			status = vl53l8cx_get_resolution(Dev, &resolution);
			status = vl53l8cx_get_ranging_data(Dev, Results);
			#if (DebugPrintfFlag == 1)
//			printf("Print data no : %3u\n", Dev->streamcount);
//			for(uint8_t i = 0; i < 16; i++)
//			{
//				printf("Zone : %3d,  Distance : %4d mm\n",
//					i,
//					Results->target_status[VL53L8CX_NB_TARGET_PER_ZONE*i],
//					Results->distance_mm[VL53L8CX_NB_TARGET_PER_ZONE*i]);
//			}
//			printf("\n");
			#endif
		}		
	}
	else {
		status = vl53l8cx_check_data_ready(Dev, &p_data_ready);
		if(p_data_ready){
			status = vl53l8cx_get_resolution(Dev, &resolution);
			status = vl53l8cx_get_ranging_data(Dev, Results);

		}else{
			HAL_Delay(5);
		}
	}
	
	return status;	
}

void process_second_targets(VL53L8CX_ResultsData *results,  uint32_t study_Dsitance, uint8_t thresh, uint8_t *detect_flag)
{
	uint8_t ZONE_COUNT;
	if(current_vl_resl == 8)
      ZONE_COUNT = 64;                     // 8x8 分辨率
	else if(current_vl_resl == 4)
		  ZONE_COUNT = 16; 
	uint8_t TARGET_INDEX = VL53L8CX_NB_TARGET_PER_ZONE - 1;                    // 第二层目标索引 (0-based)

	uint32_t pixel_dist;
	for (uint8_t i = 0; i < current_vl_resl; i++)
	{
		for (uint8_t j = 0; j < current_vl_resl; j++)
		{
			if(VL53L8CX_NB_TARGET_PER_ZONE == 2)
			{
				uint8_t idx = 2*(i * 4 + j) + 1;
//					pixel_dist = Results.distance_mm[2*(i * 4 + j) + 1];
				int32_t distance = results->distance_mm[idx];
				uint8_t status   = results->target_status[idx];

		// 检查距离条件（>50mm）和目标状态有效（通常为5或9）
				if (distance > 50 && (status == 5 || status == 9))
				{
					// 这里可以对有效数据进行处理，例如保存、平均等
					// 注意：若需补偿滤光片厚度，可减去4mm得到实际物理距离
					int32_t actual_distance = distance - 4; // 减去滤光片厚度
					// 处理数据
					if((actual_distance > 200)&&
						(actual_distance < (study_Dsitance -thresh)))
					{
						*detect_flag = 1;
					}
					else
					{
						*detect_flag = 0;
					}
				}
			}
		}
	}
	/*
	for (uint8_t zone = 0; zone < ZONE_COUNT; zone++)
	{
		// 计算第二层目标在数组中的偏移
		uint16_t idx = zone * VL53L8CX_NB_TARGET_PER_ZONE + TARGET_INDEX;

		// 检查该区域是否检测到至少 2 个目标
		if (results->nb_target_detected[zone] >= 2)
		{
			int32_t distance = results->distance_mm[idx];
			uint8_t status   = results->target_status[idx];

			// 检查距离条件（>50mm）和目标状态有效（通常为5或9）
			if (distance > 50 && (status == 5 || status == 9))
			{
				// 这里可以对有效数据进行处理，例如保存、平均等
				// 注意：若需补偿滤光片厚度，可减去4mm得到实际物理距离
				int32_t actual_distance = distance - 4; // 减去滤光片厚度
				// 处理数据
				if((actual_distance > 200)&&
					(actual_distance < (study_Dsitance -thresh)))
				{
					*detect_flag = 1;
				}
				else
				{
					*detect_flag = 0;
				}
			}
		}
//		else
//		{
//			*detect_flag = 0;
//		}
	}
	*/
}
