/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS485_GHP_TX_Pin GPIO_PIN_2
#define RS485_GHP_TX_GPIO_Port GPIOA
#define RS485_GHP_RX_Pin GPIO_PIN_3
#define RS485_GHP_RX_GPIO_Port GPIOA
#define VL53B_NCS_Pin GPIO_PIN_4
#define VL53B_NCS_GPIO_Port GPIOA
#define SPI_IIC_N2_Pin GPIO_PIN_5
#define SPI_IIC_N2_GPIO_Port GPIOA
#define VL53B_LPn_Pin GPIO_PIN_6
#define VL53B_LPn_GPIO_Port GPIOA
#define VL53B_SYNC_Pin GPIO_PIN_7
#define VL53B_SYNC_GPIO_Port GPIOA
#define VL53B_INT_Pin GPIO_PIN_0
#define VL53B_INT_GPIO_Port GPIOB
#define VL53B_INT_EXTI_IRQn EXTI0_IRQn
#define VL53A_NCS_Pin GPIO_PIN_1
#define VL53A_NCS_GPIO_Port GPIOB
#define VL53A_LPn_Pin GPIO_PIN_2
#define VL53A_LPn_GPIO_Port GPIOB
#define VL53B_SCL_Pin GPIO_PIN_10
#define VL53B_SCL_GPIO_Port GPIOB
#define IO_IN_Pin GPIO_PIN_12
#define IO_IN_GPIO_Port GPIOB
#define IO_OUT_Pin GPIO_PIN_13
#define IO_OUT_GPIO_Port GPIOB
#define RS485_TX_Pin GPIO_PIN_14
#define RS485_TX_GPIO_Port GPIOB
#define RS485_RX_Pin GPIO_PIN_15
#define RS485_RX_GPIO_Port GPIOB
#define ND06_EN_Pin GPIO_PIN_8
#define ND06_EN_GPIO_Port GPIOA
#define TOF_LED_Pin GPIO_PIN_9
#define TOF_LED_GPIO_Port GPIOA
#define VL53A_INT_Pin GPIO_PIN_10
#define VL53A_INT_GPIO_Port GPIOA
#define VL53A_INT_EXTI_IRQn EXTI10_IRQn
#define VL53A_SYNC_Pin GPIO_PIN_11
#define VL53A_SYNC_GPIO_Port GPIOA
#define SPI_IIC_N_Pin GPIO_PIN_12
#define SPI_IIC_N_GPIO_Port GPIOA
#define ND06_XSHUT_Pin GPIO_PIN_15
#define ND06_XSHUT_GPIO_Port GPIOA
#define RS485_GHP_EN_Pin GPIO_PIN_3
#define RS485_GHP_EN_GPIO_Port GPIOB
#define RS485_EN_Pin GPIO_PIN_5
#define RS485_EN_GPIO_Port GPIOB
#define VL53B_SDA_Pin GPIO_PIN_8
#define VL53B_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define DebugPrintfFlag 0
extern uint8_t num_of_pixel_occluded , work_j, nd06OutputEN, nd06AV1C_objDetectFlag;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
