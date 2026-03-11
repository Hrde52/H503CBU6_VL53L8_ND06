
#include "platform.h"
#include "stm32h5xx_hal.h"
#include "main.h"
#include "i2c.h"
#include <stdio.h>

int32_t i2c_read_nbytes(uint8_t i2c_addr, uint16_t i2c_read_addr, uint8_t *i2c_read_data, uint8_t len)
{
    //For user implement
    if(HAL_I2C_Mem_Read(&hi2c1, (i2c_addr<<1), i2c_read_addr,I2C_MEMADD_SIZE_16BIT, i2c_read_data, len, 100) != HAL_OK)
    //if(HAL_I2C_Mem_Read_IT(&hi2c1, (i2c_addr<<1), i2c_read_addr,I2C_MEMADD_SIZE_16BIT, i2c_read_data, len) != HAL_OK)
    {
		#if(DebugPrintfFlag == 1)
        printf("HAL_I2C_Mem_Read error\r\n");
		#endif
				return -1;
    }

    return 0;
}

int32_t i2c_write_nbytes(uint8_t i2c_addr, uint16_t i2c_write_addr, uint8_t *i2c_write_data, uint8_t len)
{
    //For user implement

    //if(HAL_I2C_Mem_Write_IT(&hi2c1, (i2c_addr<<1), i2c_write_addr,I2C_MEMADD_SIZE_16BIT, i2c_write_data, len) != HAL_OK)
		if(HAL_I2C_Mem_Write(&hi2c1, (i2c_addr<<1), i2c_write_addr,I2C_MEMADD_SIZE_16BIT, i2c_write_data, len, 100) != HAL_OK)
    {
		#if(DebugPrintfFlag == 1)
        printf("HAL_I2C_Mem_Write error\r\n");
		#endif
				return -1;
    }
    return 0;
}


void delay_1ms(uint32_t count)
{
    //For user implement
    HAL_Delay(count);
}

void delay_10us(uint32_t count)
{
    //For user implement
	delay_us((uint32_t) (count*10));

}

void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD; /* LOADֵ */
    ticks = nus * (SystemCoreClock / 1000000UL); /*  */
    told = SysTick->VAL; /*  */
    while (1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)
            {
                tcnt += told - tnow; /*  */
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if(tcnt >= ticks)
            {
                break; /*  */
            }
        }
    }
}


uint64_t get_now_time_us(void)
{

	return 0;

}


void set_xshut_pin_level(uint32_t level)
{
    //For user implement
    HAL_GPIO_WritePin(ND06_XSHUT_GPIO_Port, ND06_XSHUT_Pin, level);
}

int count1ms = 0;
int count5ms = 0;
int count10ms = 0;
int count100ms = 0;
int count1s = 0;
int TIME_1MS_FLAG = 0;
int TIME_5MS_FLAG = 0;
int TIME_10MS_FLAG = 0;
int TIME_100MS_FLAG = 0;
int TIME_1S_FLAG = 0;


void HAL_SYSTICK_Callback(void)
{
	count1ms++;
    count5ms++;
    count10ms++;
    count100ms++;
    count1s++;
    if(count1ms>=1)
    {
        TIME_1MS_FLAG = 1;
        count1ms = 0;
    }
    if(count5ms>=5)
    {
        TIME_5MS_FLAG = 1;
        count5ms = 0;
    }
    if(count10ms>=10)
    {
        TIME_10MS_FLAG = 1;
        count10ms = 0;
    }
    if(count100ms>=100)
    {
        TIME_100MS_FLAG = 1;
        count100ms = 0;
    }
    if(count1s>=1000)
    {
        TIME_1S_FLAG = 1;
        count1s = 0;
    }

}






