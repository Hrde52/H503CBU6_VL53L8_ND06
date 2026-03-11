///*******************************************************************************
//* Copyright (c) 2020, STMicroelectronics - All Rights Reserved
//*
//* This file, part of the VL53L8CX Ultra Lite Driver,
//* is licensed under terms that can be found in the LICENSE file
//* in the root directory of this software component.
//* If no LICENSE file comes with this software, it is provided AS-IS.
//*
//*******************************************************************************/

///***********************************/
///*   VL53L8CX ULD basic example    */
///***********************************/
///*
//* This example is the most basic. It initializes the VL53L8CX ULD, and starts
//* a ranging to capture 10 frames.
//*
//* By default, ULD is configured to have the following settings :
//* - Resolution 4x4
//* - Ranging period 1Hz
//*
//* In this example, we also suppose that the number of target per zone is
//* set to 1 , and all output are enabled (see file platform.h).
//*/

//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>
//#include "vl53l8cx_api.h"
//#include "flash_storage.h"

//extern volatile int IntCount; //add Interrupt
//#define is_interrupt 1 /*is_interrupt = 1 => get data by interrupt, = 0 => get data by polling */

//uint8_t WaitForL8CXInterrupt(VL53L8CX_Configuration * pDev){

//	__WFI();
//	if(IntCount !=0 ){
//		IntCount=0;
//		return 1;
//	}else{
//		return 0;
//	}
//}

//VL53L8CX_ResultsData 	Results;
//int example1(void)
//{

//	/*********************************/
//	/*   VL53L8CX ranging variables  */
//	/*********************************/
//	uint8_t p_data_ready;
//	static VL53L8CX_Configuration 	Dev;
//	
//	uint8_t status, resolution, isAlive, isReady;

//	/* USER CODE END PV */

//	/*********************************/
//	/*      Customer platform        */
//	/*********************************/

//	/* Fill the platform structure with customer's implementation. For this
//	* example, only the I2C address is used.
//	*/
//	Dev.platform.address = VL53L8CX_DEFAULT_I2C_ADDRESS;

//	/* (Optional) Reset sensor toggling PINs (see platform, not in API) */
//	VL53L8CX_Reset_Sensor(&(Dev.platform));

//	/* (Optional) Set a new I2C address if the wanted address is different
//	* from the default one (filled with 0x20 for this example).
//	*/
//	//status = vl53l8cx_set_i2c_address(&Dev, 0x20);


//	/*********************************/
//	/*   Power on sensor and init    */
//	/*********************************/

//	/* (Optional) Check if there is a VL53L8CX sensor connected */
//	status = vl53l8cx_is_alive(&Dev, &isAlive);
////	printf("status = %d, isAlive = %d\n", status,isAlive);
//	if(!isAlive || status)
//	{
////		printf("VL53L8CX not detected at requested address\n");
//		return status;
//	}

//	/* (Mandatory) Init VL53L8CX sensor */
//	status = vl53l8cx_init(&Dev);
//	if(status)
//	{
////		printf("VL53L8CX ULD Loading failed\n");
//		return status;
//	}

//	status = vl53l8cx_set_resolution(&Dev, VL53L8CX_RESOLUTION_8X8);

//	status = vl53l8cx_set_ranging_frequency_hz(&Dev, 5);// Set 30Hz ranging frequency
//#if 1
//	status = vl53l8cx_set_ranging_mode(&Dev, VL53L8CX_RANGING_MODE_CONTINUOUS);  // Set mode continuous
//#else
//	status = vl53l8cx_set_ranging_mode(&Dev, VL53L8CX_RANGING_MODE_AUTONOMOUS);  // Set mode autonomous
//	status = vl53l8cx_set_integration_time_ms(&Dev,2);
//#endif
////	printf("VL53L8CX ULD ready ! (Version : %s)\n",
////			VL53L8CX_API_REVISION);

//	/*********************************/
//	/*         Ranging loop          */
//	/*********************************/

//	status = vl53l8cx_start_ranging(&Dev);

//	if (is_interrupt) {
//		do
//			{
//				isReady = WaitForL8CXInterrupt(&Dev);
//				if(isReady)
//				{
//					status = vl53l8cx_get_resolution(&Dev, &resolution);
//					status = vl53l8cx_get_ranging_data(&Dev, &Results);
//				}
//			}while(1);
//	}
//	else {
//		do
//			{
//				status = vl53l8cx_check_data_ready(&Dev, &p_data_ready);
//				if(p_data_ready){
//					status = vl53l8cx_get_resolution(&Dev, &resolution);
//					status = vl53l8cx_get_ranging_data(&Dev, &Results);

//				}else{
//					HAL_Delay(5);
//				}
//			}
//			while(1);
//	}
//}
