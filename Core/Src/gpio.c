/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, VL53B_NCS_Pin|SP1_IIC_N2_Pin|VL53B_LPn_Pin|VL53B_SYNC_Pin
                          |ND06_EN_Pin|VL53A_SYNC_Pin|SPI_IIC_N_Pin|ND06_XSHUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, VL53A_NCS_Pin|VL53A_LPn_Pin|RS485_GHP_EN_Pin|RS485_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TOF_LED_GPIO_Port, TOF_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : VL53B_NCS_Pin SP1_IIC_N2_Pin VL53B_LPn_Pin VL53B_SYNC_Pin
                           ND06_EN_Pin TOF_LED_Pin VL53A_SYNC_Pin SPI_IIC_N_Pin
                           ND06_XSHUT_Pin */
  GPIO_InitStruct.Pin = VL53B_NCS_Pin|SP1_IIC_N2_Pin|VL53B_LPn_Pin|VL53B_SYNC_Pin
                          |ND06_EN_Pin|TOF_LED_Pin|VL53A_SYNC_Pin|SPI_IIC_N_Pin
                          |ND06_XSHUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : VL53B_INT_Pin */
  GPIO_InitStruct.Pin = VL53B_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VL53B_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VL53A_NCS_Pin VL53A_LPn_Pin */
  GPIO_InitStruct.Pin = VL53A_NCS_Pin|VL53A_LPn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : IO_IN_Pin */
  GPIO_InitStruct.Pin = IO_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IO_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IO_OUT_Pin RS485_GHP_EN_Pin */
  GPIO_InitStruct.Pin = IO_OUT_Pin|RS485_GHP_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : VL53A_INT_Pin */
  GPIO_InitStruct.Pin = VL53A_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(VL53A_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_EN_Pin */
  GPIO_InitStruct.Pin = RS485_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RS485_EN_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
