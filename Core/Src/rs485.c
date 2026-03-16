#include "rs485.h"
#include "PDA_Communication.h"
#include "main.h"
//#include "sensorAppLogic.h"
//#include "sensorParaTable.h"
#include "string.h"
//#include "toolingtest.h"
//#include "elevatorSignalMonitor.h"

unsigned char gLookupTableCRC8[256] = {
    /* 0x00 */ 0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,
    0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,
    0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,
    0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,
    0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
    /* 0x40 */ 0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,
    0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,
    0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,
    0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,
    0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
    /* 0x80 */ 0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,
    0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,
    0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,
    0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,
    0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
    /* 0xC0 */ 0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,
    0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,
    0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,
    0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,
    0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF};

uint16_t RxElevatorCnt = 0;
uint16_t headerIsFoundFlag = 0;
uint16_t rxSize = 6;
bool isSixBytes = true; // true 6 false HCB 4 
bool isFixBytes = false;

ControlSystemPara CSpara = {0, 0, 0, 0, 0};

uint8_t rxDataBuffElevator[1] = {0};
uint8_t rx_data_Elevator[17] = {0, 0, 0, 0, 0, 0,0,0,0,0,
0, 0, 0, 0, 0, 0,0};
uint8_t RxDataElevator[6] = {0, 0, 0, 0, 0, 0};
uint8_t rxDataBuffPDA[4] = {0, 0, 0, 0};
uint8_t rs485ElevatorRxCpltFlag = 0;
uint8_t rs485PDARxCpltFlag = 0;

uint8_t testArr[100];
uint16_t testii = 0;
uint8_t testArr2[100];
uint16_t testii2 = 0;

uint16_t rxIndex = 0;
uint16_t total_len = 0;
uint8_t rxDBuffPDA[MAX_RX_LEN];
uint8_t iindex = 0;
uint8_t rx_DBuffPDA[MAX_RX_LEN];
SensorProtocol *global_pkt = NULL;

uint16_t rxIndexTooling = 0;
uint16_t rxIndexTooling2 = 0;
uint8_t rxDBuffTooling1[10];
uint8_t rxDBuffTooling2[10];
uint8_t gotToolingHandshake = 0;

uint8_t CalcCRC8(uint8_t *inBuffer, int inSize)
{
    uint8_t aCRC;
    uint8_t i;
    aCRC = 0xFF; /* Start CRC Remain*/
    for (i = 0; i < (int)inSize; i++)
    {
        aCRC = (aCRC ^ *(inBuffer + i));
        aCRC = gLookupTableCRC8[aCRC];
    }
    aCRC = ~aCRC;
    return (uint8_t)aCRC;
}

uint8_t xor_checkSum(uint8_t *data, uint8_t length)
{
    uint8_t checkSum = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        checkSum ^= data[i];
    }
    return checkSum;
}

void RS485_Elevator_Init()
{
	RS485_Elevator_RX_ENABLE();
	HAL_UART_Receive_IT(&huart1, rxDataBuffElevator, 1);
	RS485_Elevator_RX_ENABLE();
}

void RS485_PDA_Init()
{
    HAL_UART_Receive_IT(&huart2, rxDataBuffPDA, 4);
    RS485_PDA_RX_ENABLE();
}


int RS485_Elevator_Transmit(uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;

    RS485_Elevator_TX_ENABLE();

    status = HAL_UART_Transmit(&RS485_Elevator_USART, data, len, 1);

    // HAL_Delay(1);

    RS485_Elevator_RX_ENABLE();
    rs485ElevatorRxCpltFlag = 0;

    return status == HAL_OK ? 0 : -1;
}

int RS485_PDA_Transmit(uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;

    RS485_PDA_TX_ENABLE();

    status = HAL_UART_Transmit(&RS485_PDA_USART, data, len, 1);

    // HAL_Delay(1);

    RS485_PDA_RX_ENABLE();
    rs485PDARxCpltFlag = 0;

    return status == HAL_OK ? 0 : -1;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t crcValue = 0;

    if (huart->Instance == USART1)
    {
			testArr[testii] = rxDataBuffElevator[0];
			testii++;
			if (testii >= 100)
			{
				testii = 0;
			}

			if (rxDataBuffElevator[0] == 0x87)
			{
				headerIsFoundFlag = 1;
				RxElevatorCnt = 0;
			}

			if (headerIsFoundFlag == 1)
			{
				rx_data_Elevator[RxElevatorCnt++] = rxDataBuffElevator[0];

				if (RxElevatorCnt >= 17)
				{
					ProcessElevatorData();
					headerIsFoundFlag = 0;
				}
			}
			
			rxDataBuffElevator[0] = 0;
			HAL_UART_Receive_IT(&RS485_Elevator_USART, rxDataBuffElevator, 1);
    }

    else if (huart->Instance == USART2)
    {
			rx_DBuffPDA[iindex++] = rxDBuffPDA[rxIndex];
			if (iindex >= 128)
			{
					iindex = 0;
			}

			//  Header
			if (rxIndex == 0 && rxDBuffPDA[0] != 0XAA) //(HEADER & 0xFF)
			{
					rxIndex = 0;
					HAL_UART_Receive_IT(&RS485_PDA_USART, &rxDBuffPDA[rxIndex], 1);
					return;
			}

			if (rxIndex >= sizeof(SensorProtocol))
			{
				SensorProtocol *pkt = (SensorProtocol *)rxDBuffPDA;
				global_pkt = pkt;

				uint8_t sizeSP = sizeof(SensorProtocol);
				total_len = sizeSP - 3 + pkt->length; // /*cmd+l1 = 3*/ DATA1
				if (total_len >= MAX_RX_LEN)
				{
						rxIndex = 0;
						total_len = 0;
						memset(rxDBuffPDA, 0, sizeof(rxDBuffPDA)); //

						RS485_PDA_RX_ENABLE();
						HAL_UART_Receive_IT(&RS485_PDA_USART, &rxDBuffPDA[rxIndex], 1);
						return;
				}

				if (rxIndex >= total_len - 1)
				{
						ProcessPacket(pkt);

						rxIndex = 0;
						total_len = 0;
						memset(rxDBuffPDA, 0, sizeof(rxDBuffPDA));
						RS485_PDA_RX_ENABLE();
						HAL_UART_Receive_IT(&RS485_PDA_USART, &rxDBuffPDA[rxIndex], 1);
						return;
				}
			}

			rxIndex++;
			if (rxIndex >= MAX_RX_LEN)
			{
				rxIndex = 0;
			}

			HAL_UART_Receive_IT(&RS485_PDA_USART, &rxDBuffPDA[rxIndex], 1); 
    }
}



uint8_t connectEleFlag = 0;
uint8_t masterElevator_LevelingSignal = 0;
uint8_t tx_data_Door[16] = {0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0,0,0};
uint8_t rxDataEleHeander = 0;
uint32_t lastReceivedELETime = 0;
uint32_t cls_cmd_Time =0;
uint32_t cls_Time = 0;
//uint8_t  ObjectIsDetectedFlag;

void ProcessElevatorData(void)
{
	//recEleMsgFLG = 1;
	lastReceivedELETime = HAL_GetTick();

	connectEleFlag = 1;
		
	uint8_t checkXor = xor_checkSum(rx_data_Elevator, 16);
	if (checkXor == rx_data_Elevator[16])
	{
		CSpara.LevelingSignal = 1;

		masterElevator_LevelingSignal = (rx_data_Elevator[1] >> 0) & 0x01; 
		if ((rx_data_Elevator[1] >> 0 & 0X01))
		{
			
				CSpara.openDoorCmd = 1;
		}
		else
		{
				CSpara.openDoorCmd = 0;
		}

		if ((rx_data_Elevator[1] >> 1 & 0X01))
		{
			if(CSpara.closeDoorCmd == 0)
			{
				cls_cmd_Time = HAL_GetTick();
				
			}
			if(CSpara.OLS == 1)
			{
				work_j = 0;
			}
			CSpara.closeDoorCmd = 1;
		}
		else
		{
				CSpara.closeDoorCmd = 0;
		}
		
		if ((rx_data_Elevator[1] >> 4 & 0X01))
		{
				CSpara.OLS = 1;
		}
		else
		{
				CSpara.OLS = 0;
		}
		
		if ((rx_data_Elevator[1] >> 5 & 0X01))
		{
			cls_Time = HAL_GetTick() - cls_cmd_Time;
			CSpara.CLS = 1;
			cls_cmd_Time = 0;
		}
		else
		{
			CSpara.CLS = 0;
//				work_j = 2;
		}

		tx_data_Door[0] = 0x86;

		ByteBits *byte_bits = (ByteBits *)&tx_data_Door[1]; // ??? packet[1]
		if((ObjectIsDetectedFlag == 1 ) || (nd06AV1C_objDetectFlag == 1))
		{
//				byte_bits->bits.bit0 = 1;
			if(HAL_GetTick() - cls_cmd_Time < 0xBB8 )   //0x7d0 2
			{
				if(HAL_GetTick() - cls_cmd_Time < 0x7d0 )
				{
					work_j = 0;
				}
				else if(HAL_GetTick() - cls_cmd_Time < 0x898)  // 0xBB8 3  0x9c4 2.5  0x898 2.2 0x7d0 2
				{
					work_j = 1;
					
				}
				else
				{
					work_j = 2;
				}
				
				if((work_j == 0)||(work_j == 1))
					tx_data_Door[1] = 0x01;
			}
			if(CSpara.OLS == 1)
			{
				tx_data_Door[1] = 0x01;
				cls_cmd_Time = 0;
				work_j = 0;
			}
		}
		else
		{				
//				byte_bits->bits.bit0 = 0;
			tx_data_Door[1] = 0x00;
		}

		tx_data_Door[2] = 0x01;
		
		tx_data_Door[8] = 0x01;
		tx_data_Door[9] = 0x01;
		tx_data_Door[10] = 0x01;
		tx_data_Door[11] = 0x40;

		// byte16
		tx_data_Door[15] = xor_checkSum(tx_data_Door, 16);

		delay_10us(29);
		HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);
		delay_10us(1);
		HAL_UART_Transmit_IT(&RS485_Elevator_USART, tx_data_Door, 16);
	}

	headerIsFoundFlag = 0;
	RxElevatorCnt = 0;
	memset(rx_data_Elevator, 0, sizeof(rx_data_Elevator));
	return;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);
        delay_10us(1);
        HAL_UART_Receive_IT(&RS485_Elevator_USART, rxDataBuffElevator, 1);
    }
}
// END
