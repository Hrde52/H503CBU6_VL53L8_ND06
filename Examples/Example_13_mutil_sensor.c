#include "main.h"
#include <stdio.h>
#include "vl53l8cx_api.h"
#include "vl53l8cx_plugin_motion_indicator.h"
int status;
	/* USER CODE BEGIN PV */

uint8_t p_data_ready, isReady;
uint8_t resolution, isAlive;
uint16_t idx;

VL53L8CX_Configuration 	Dev;
VL53L8CX_Configuration 	Dev_Center;
VL53L8CX_Configuration 	Dev_Left;
VL53L8CX_Configuration 	Dev_Right;

VL53L8CX_ResultsData 	Results;
VL53L8CX_ResultsData 	Results_Left;
VL53L8CX_ResultsData 	Results_Center;
VL53L8CX_ResultsData 	Results_Right;
/* USER CODE END PV */

//void get_data_by_polling(VL53L8CX_Configuration *p_dev);
//void get_data_by_interrupt(VL53L8CX_Configuration *p_dev);

extern uint8_t WaitForL8CXInterrupt(VL53L8CX_Configuration * pDev);//defined in example1
void get_data_by_polling(void);
void get_data_by_interrupt(void);

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif

#define is_interrupt 0 /*is_interrupt = 1 => get data by interrupt, = 0 => get data by polling */

/**
 * @brief  The application entry point.
 * @retval int
 */
int example13(void)
{

	Dev_Center.platform.address = VL53L8CX_DEFAULT_I2C_ADDRESS;
	Dev_Left.platform.address = VL53L8CX_DEFAULT_I2C_ADDRESS;
	Dev_Right.platform.address = VL53L8CX_DEFAULT_I2C_ADDRESS;

	VL53L8CX_Reset_Sensor(&(Dev_Center.platform));
	VL53L8CX_Reset_Sensor(&(Dev_Left.platform));
	VL53L8CX_Reset_Sensor(&(Dev_Right.platform));// Dev.platform not used inside this function

/*Center sensor init setting start*/
	HAL_GPIO_WritePin(PWR_EN_C_GPIO_Port, PWR_EN_C_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LPn_C_GPIO_Port, LPn_C_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

	status = vl53l8cx_is_alive(&Dev_Center, &isAlive);
	if(!isAlive)
	{
		printf("VL53L8CX not detected at requested address (0x%x)\n", Dev_Center.platform.address);
				return 255;
			}
	status = vl53l8cx_set_i2c_address(&Dev_Center,0x56);
	printf("Center sensor initializing, please wait few seconds\n");

	status = vl53l8cx_init(&Dev_Center);
	status = vl53l8cx_set_ranging_frequency_hz(&Dev_Center, 2);				// Set 2Hz ranging frequency
	status = vl53l8cx_set_ranging_mode(&Dev_Center, VL53L8CX_RANGING_MODE_CONTINUOUS);  // Set mode continuous
/*Center sensor init setting end*/

/*Left sensor init setting start*/
//	HAL_GPIO_WritePin(PWR_EN_L_GPIO_Port, PWR_EN_L_Pin, GPIO_PIN_SET);
//	HAL_Delay(100);
//	HAL_GPIO_WritePin(LPn_L_GPIO_Port, LPn_L_Pin, GPIO_PIN_SET);
//	HAL_Delay(100);

	status = vl53l8cx_is_alive(&Dev_Left, &isAlive);
	if(!isAlive)
	{
		printf("VL53L8CX not detected at requested address (0x%x)\n", Dev_Left.platform.address);
				return 255;
			}
    status = vl53l8cx_set_i2c_address(&Dev_Left,0x54);
	printf("Left sensor initializing, please wait few seconds\n");

	status = vl53l8cx_init(&Dev_Left);
	status = vl53l8cx_set_ranging_frequency_hz(&Dev_Left, 2);				// Set 2Hz ranging frequency
	status = vl53l8cx_set_ranging_mode(&Dev_Left, VL53L8CX_RANGING_MODE_CONTINUOUS);  // Set mode continuous
/*Left sensor init setting end*/

/*Right sensor init setting start*/
//	HAL_GPIO_WritePin(PWR_EN_R_GPIO_Port, PWR_EN_R_Pin, GPIO_PIN_SET);
//	HAL_Delay(100);
//	HAL_GPIO_WritePin(LPn_R_GPIO_Port, LPn_R_Pin, GPIO_PIN_SET);
//	HAL_Delay(100);

	status = vl53l8cx_is_alive(&Dev_Right, &isAlive);
	if(!isAlive)
	{
		printf("VL53L8CX not detected at requested address (0x%x)\n", Dev_Right.platform.address);
				return 255;
			}
    status = vl53l8cx_set_i2c_address(&Dev_Right,0x58);
	printf("Right sensor initializing, please wait few seconds\n");

	status = vl53l8cx_init(&Dev_Right);
	status = vl53l8cx_set_ranging_frequency_hz(&Dev_Right, 2);				// Set 2Hz ranging frequency
	status = vl53l8cx_set_ranging_mode(&Dev_Right, VL53L8CX_RANGING_MODE_CONTINUOUS);  // Set mode continuous
/*Right sensor init setting end*/

/*Start all sensor*/
	status = vl53l8cx_start_ranging(&Dev_Left);
	printf("Left sensor ranging starts\n" );
	status = vl53l8cx_start_ranging(&Dev_Center);
	printf("Center sensor ranging starts\n" );
	status = vl53l8cx_start_ranging(&Dev_Right);
	printf("Right sensor ranging starts\n" );

/*Start all sensor*/
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	if (is_interrupt) {
		get_data_by_interrupt();
	}
	else {
		get_data_by_polling();
	}

	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
	/* USER CODE END 3 */
	return status;
}

/* USER CODE BEGIN 4 */

void get_data_by_interrupt(void){
	do
	{
		isReady = WaitForL8CXInterrupt(&Dev_Left);
		if(isReady)
		{
			status = vl53l8cx_get_resolution(&Dev_Left, &resolution);
			status = vl53l8cx_get_ranging_data(&Dev_Left, &Results_Left);

			printf("left Ranging result:\n" );
			for(int i = 0; i < resolution;i++){
				/* Print per zone results */
				printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
						i,
						Results_Left.nb_target_detected[i],
						Results_Left.ambient_per_spad[i]);

				/* Print per target results */
				if(Results_Left.nb_target_detected[i] > 0){
					printf("Target status : %3u, Distance : %4d mm\n",
							Results_Left.target_status[VL53L8CX_NB_TARGET_PER_ZONE * i],
							Results_Left.distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * i]);
				}else{
					printf("Target status : 255, Distance : No target\n");
				}
			}
			printf("\n");
		}


		isReady = WaitForL8CXInterrupt(&Dev_Center);
		if(isReady)
		{
			status = vl53l8cx_get_resolution(&Dev_Center, &resolution);
			status = vl53l8cx_get_ranging_data(&Dev_Center, &Results_Center);

			printf("center Ranging result:\n" );
			for(int i = 0; i < resolution;i++){
				/* Print per zone results */
				printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
						i,
						Results_Center.nb_target_detected[i],
						Results_Center.ambient_per_spad[i]);

				/* Print per target results */
				if(Results_Center.nb_target_detected[i] > 0){
					printf("Target status : %3u, Distance : %4d mm\n",
							Results_Center.target_status[VL53L8CX_NB_TARGET_PER_ZONE * i],
							Results_Center.distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * i]);
				}else{
					printf("Target status : 255, Distance : No target\n");
				}
			}
			printf("\n");
		}


		isReady = WaitForL8CXInterrupt(&Dev_Right);
		if(isReady)
		{
			status = vl53l8cx_get_resolution(&Dev_Right, &resolution);
			status = vl53l8cx_get_ranging_data(&Dev_Right, &Results_Right);

			printf("right Ranging result:\n" );
			for(int i = 0; i < resolution;i++){
				/* Print per zone results */
				printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
						i,
						Results_Right.nb_target_detected[i],
						Results_Right.ambient_per_spad[i]);

				/* Print per target results */
				if(Results_Right.nb_target_detected[i] > 0){
					printf("Target status : %3u, Distance : %4d mm\n",
							Results_Right.target_status[VL53L8CX_NB_TARGET_PER_ZONE * i],
							Results_Right.distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * i]);
				}else{
					printf("Target status : 255, Distance : No target\n");
				}
			}
			printf("\n");
		}

	}while(1);
}

void get_data_by_polling(void){
	do
	{

		status = vl53l8cx_check_data_ready(&Dev_Left, &p_data_ready);
		if(p_data_ready){
			status = vl53l8cx_get_resolution(&Dev_Left, &resolution);// Time consuming function,
			status = vl53l8cx_get_ranging_data(&Dev_Left, &Results_Left);


			printf("left Ranging result:\n" );

			for(int i = 0; i < resolution;i++){
				/* Print per zone results */
				printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
						i,
						Results_Left.nb_target_detected[i],
						Results_Left.ambient_per_spad[i]);

				/* Print per target results */
				if(Results_Left.nb_target_detected[i] > 0){
					printf("Target status : %3u, Distance : %4d mm\n",
							Results_Left.target_status[VL53L8CX_NB_TARGET_PER_ZONE * i],
							Results_Left.distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * i]);
				}else{
					printf("Target status : 255, Distance : No target\n");
				}
			}

			printf("\n");

			}else{
				printf("left Ranging not ready\n" );
				HAL_Delay(1000);
			}




		status = vl53l8cx_check_data_ready(&Dev_Center, &p_data_ready);
		if(p_data_ready){
			status = vl53l8cx_get_resolution(&Dev_Center, &resolution);// Time consuming function,
			status = vl53l8cx_get_ranging_data(&Dev_Center, &Results_Center);


			printf("center Ranging result:\n" );

			for(int i = 0; i < resolution;i++){
				/* Print per zone results */
				printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
						i,
						Results_Center.nb_target_detected[i],
						Results_Center.ambient_per_spad[i]);

				/* Print per target results */
				if(Results_Center.nb_target_detected[i] > 0){
					printf("Target status : %3u, Distance : %4d mm\n",
							Results_Center.target_status[VL53L8CX_NB_TARGET_PER_ZONE * i],
							Results_Center.distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * i]);
				}else{
					printf("Target status : 255, Distance : No target\n");
				}
			}

			printf("\n");

			}else{
				printf("Center Ranging not ready\n" );
				HAL_Delay(1000);
			}



		status = vl53l8cx_check_data_ready(&Dev_Right, &p_data_ready);
		if(p_data_ready){
			status = vl53l8cx_get_resolution(&Dev_Right, &resolution);// Time consuming function,
			status = vl53l8cx_get_ranging_data(&Dev_Right, &Results_Right);


			printf("right Ranging result:\n" );

			for(int i = 0; i < resolution;i++){
				/* Print per zone results */
				printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
						i,
						Results_Right.nb_target_detected[i],
						Results_Right.ambient_per_spad[i]);

				/* Print per target results */
				if(Results_Right.nb_target_detected[i] > 0){
					printf("Target status : %3u, Distance : %4d mm\n",
							Results_Right.target_status[VL53L8CX_NB_TARGET_PER_ZONE * i],
							Results_Right.distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * i]);
				}else{
					printf("Target status : 255, Distance : No target\n");
				}
			}

			printf("\n");

			}else{
				printf("Right Ranging not ready\n" );
				HAL_Delay(1000);
			}
		HAL_Delay(1000);
	}
	while(1);
}

/* USER CODE END 4 */
