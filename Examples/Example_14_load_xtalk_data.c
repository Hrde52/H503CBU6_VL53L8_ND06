/*******************************************************************************
* Copyright (c) 2020, STMicroelectronics - All Rights Reserved
*
* This file, part of the VL53L8CX Ultra Lite Driver,
* is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

/***********************************/
/*   VL53L8CX ULD Load xtalk example    */
/***********************************/
/*.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vl53l8cx_api.h"
#include "vl53l8cx_plugin_xtalk.h"
#include "flash_storage.h"

int example14(void)
{

	/*********************************/
	/*   VL53L8CX ranging variables  */
	/*********************************/
	uint8_t p_data_ready;
	VL53L8CX_Configuration 	Dev;
	VL53L8CX_ResultsData 	Results;
	uint8_t status, resolution, isAlive;
	uint8_t					xtalk_data[VL53L8CX_XTALK_BUFFER_SIZE];
	/* USER CODE END PV */

	/*********************************/
	/*      Customer platform        */
	/*********************************/

	/* Fill the platform structure with customer's implementation. For this
	* example, only the I2C address is used.
	*/
	Dev.platform.address = VL53L8CX_DEFAULT_I2C_ADDRESS;

	/* (Optional) Reset sensor toggling PINs (see platform, not in API) */
	VL53L8CX_Reset_Sensor(&(Dev.platform));

	/* (Optional) Set a new I2C address if the wanted address is different
	* from the default one (filled with 0x20 for this example).
	*/
	//status = vl53l8cx_set_i2c_address(&Dev, 0x20);


	/*********************************/
	/*   Power on sensor and init    */
	/*********************************/

	/* (Optional) Check if there is a VL53L8CX sensor connected */
	status = vl53l8cx_is_alive(&Dev, &isAlive);
	printf("status = %d, isAlive = %d\n", status,isAlive);
	if(!isAlive || status)
	{
		printf("VL53L8CX not detected at requested address\n");
		return status;
	}

	/* (Mandatory) Init VL53L8CX sensor */
	status = vl53l8cx_init(&Dev);
	if(status)
	{
		printf("VL53L8CX ULD Loading failed\n");
		return status;
	}

	printf("VL53L8CX ULD ready ! (Version : %s)\n",
			VL53L8CX_API_REVISION);
/*load xtalk*/

	uint32_t i, j;
	union Block_header *bh_ptr;
	uint32_t xtalk_signal_kcps_grid[VL53L8CX_RESOLUTION_8X8];
	uint16_t xtalk_shape_bins[144];

	Flash_Read_Array((uint8_t*)&xtalk_data, sizeof(xtalk_data));

	/* Re-Swap buffer (in case of re-using data later) */
	VL53L8CX_SwapBuffer(xtalk_data, VL53L8CX_XTALK_BUFFER_SIZE);

	/* Get data */
	for(i = 0; i < VL53L8CX_XTALK_BUFFER_SIZE; i = i + 4)
	{
		bh_ptr = (union Block_header *)&(xtalk_data[i]);
		if (bh_ptr->idx == 0xA128){
			printf("Xtalk shape bins located at position %#06x\n", (int)i);
			for (j = 0; j < 144; j++){
				memcpy(&(xtalk_shape_bins[j]), &(xtalk_data[i + 4 + j * 2]), 2);
				printf("xtalk_shape_bins[%d] = %d\n", (int)j, (int)xtalk_shape_bins[j]);
			}
		}
		if (bh_ptr->idx == 0x9FFC){
			printf("Xtalk signal kcps located at position %#06x\n", (int)i);
			for (j = 0; j < VL53L8CX_RESOLUTION_8X8; j++){
				memcpy(&(xtalk_signal_kcps_grid[j]), &(xtalk_data[i + 4 + j * 4]), 4);
				xtalk_signal_kcps_grid[j] /= 2048;
				printf("xtalk_signal_kcps_grid[%d] = %d\n", (int)j, (int)xtalk_signal_kcps_grid[j]);
			}
		}
	}

	/* Re-Swap buffer (in case of re-using data later) */
	VL53L8CX_SwapBuffer(xtalk_data, VL53L8CX_XTALK_BUFFER_SIZE);

	/* Set Xtalk calibration data */
	status = vl53l8cx_set_caldata_xtalk(&Dev, xtalk_data);

/*load xtalk*/

	/*********************************/
	/*         Ranging loop          */
	/*********************************/

	status = vl53l8cx_start_ranging(&Dev);

	do{
		status = vl53l8cx_check_data_ready(&Dev, &p_data_ready);
			if(p_data_ready){
					status = vl53l8cx_get_resolution(&Dev, &resolution);
					status = vl53l8cx_get_ranging_data(&Dev, &Results);

					for(int i = 0; i < resolution;i++){
						/* Print per zone results */
						printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
							i,
							Results.nb_target_detected[i],
							Results.ambient_per_spad[i]);

						/* Print per target results */
						if(Results.nb_target_detected[i] > 0){
							printf("Target status : %3u, Distance : %4d mm\n",
								Results.target_status[VL53L8CX_NB_TARGET_PER_ZONE * i],
								Results.distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * i]);
						}else{
							printf("Target status : 255, Distance : No target\n");
						}
					}
				printf("\n");
			}else{
				HAL_Delay(5);
			}
			
	}while(10);
}
