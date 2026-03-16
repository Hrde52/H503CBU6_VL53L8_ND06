#include "PDA_Communication.h"
//#include "elevatorSignalMonitor.h"
#include "rs485.h"
//#include "sensorAppLogic.h"
#include "sensorParaTable.h"
#include "string.h"
#include "vl53cx8_app.h"

// #define uint8_t unsigned int

uint32_t DistanceThresholdLearningReqFlg = 0;
uint32_t ClosingTimeLearningReqFlg = 0;

const unsigned char auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

const unsigned char auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

uint16_t show_pktcrc;
uint16_t show_crc;
uint8_t m_sensorType111;

uint8_t txBuffer1[50] = {0};
uint8_t txBuffer2[144] = {0};
uint8_t STOP_DistanceThresholdLearning_ReqFlg = 0;
uint8_t STOP_ClosingTimeLearningReqFlg = 0;
uint8_t txBuffer3[116] = {0};
uint8_t setBeginDebug;
uint8_t txBuffer4[162] = {0};
uint8_t txBuffer5[24] = {0};


/*temp 2026/3/12*/
uint8_t sensor_status = 1;


unsigned short CalcCRC16(unsigned char *puchMsg, unsigned short usDataLen)
{
    unsigned char uchCRCHi = 0xFF; 
    unsigned char uchCRCLo = 0xFF; 
    unsigned int uIndex;           
    while (usDataLen--)
    { 
        uIndex = uchCRCLo ^ *puchMsg++;
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
        uchCRCHi = auchCRCLo[uIndex];
    }
    return (uchCRCHi << 8 | uchCRCLo);
}

void WriteU32LittleEndian(uint8_t *buf, uint32_t value)
{
    buf[0] = (value >> 0) & 0xFF;
    buf[1] = (value >> 8) & 0xFF;
    buf[2] = (value >> 16) & 0xFF;
    buf[3] = (value >> 24) & 0xFF;
}

void WriteU16LittleEndian(uint8_t *buf, uint32_t value)
{
    buf[0] = (value >> 0) & 0xFF;
    buf[1] = (value >> 8) & 0xFF;
}

uint8_t find_bit_position(uint32_t hex_value) 
{
    if (hex_value == 0) {
        return 0;  // 没有有效位1
    }
    
    for (uint8_t i = 0; i < 8; i++) 
		{
        if (hex_value & 0x01) {
            return i + 1;  // 返回位置（从1开始）
        }
        hex_value >>= 1;
    }
		
    return 0;
}


uint8_t get_error_count(uint32_t hex_value) {
    uint8_t count = 0;
    
  /*  
	// 使用位运算快速计算1的个数
	uint32_t value = hex_value;     // ErrData.data
	
	// Brian Kernighan算法
	while (value) {
			value &= (value - 1);
			count++;
	}
*/	
	return count;
	
}


void ProcessPacket(SensorProtocol *pkt)
{

    if (pkt->head != HEADER || pkt->version != 0x01)
    {
        return;
    }

    uint16_t calc_crc = CalcCRC16((uint8_t *)&pkt->cmd, pkt->length); // 0x0129 0x01
    show_pktcrc = pkt->crc;                                           // 0x005d
    show_crc = calc_crc;
    // CRC
    if (calc_crc != pkt->crc)
    {
    }

    switch (PARA_TABLE_USE.data.functionChioce)
    {
    case 0x11:
        m_sensorType111 = 1;
        break;
    case 0x22:
        m_sensorType111 = 2;
        break;
    case 0x33:
        m_sensorType111 = 3;
        break;
    default:
        break;
    }

    switch (pkt->cmd)
    {
    case HANDSHAKE_CMD: //
        HandleHandshake(pkt);
        // AA 55 20 80 01 09 00 78 5E 01 00 06 19 05 16 0F 18 2E  // PDA HAND
        break;

    case HEARTBEAT_CMD:       // 
        HandleHeartbeat(pkt); // PDA HEART
        // AA 55 1D A3 01 0F 00 F6 57 02 00 0C 01 02 03 04 05 06 07 08 09 10 11 12
        break;

    case CONTROL_CMD: // 
        HandleControl(pkt);
        // AA 55 1D A3 01 0F 00 F6 57 02 00 0C 01 02 03 04 05 06 07 08 09 10 11 12
        break;

    case PARAM_READ_CMD: // 
        HandleParamRead(pkt);
        // AA 55 20 88 1 4 0 1 0 4 0 1 1
        break;

    case PARAM_WRITE_CMD: // 
        HandleParamWrite(pkt);
        // AA 55 22 33 1 11 0 1 0 7 0 1 1 6 0 3 0 0 0 53
        break;

    default:
        break;
    }
}

void HandleHandshake(SensorProtocol *pkt)
{
    uint8_t *m_data = pkt->data1;
    uint8_t m_year = m_data[0];
    uint8_t m_month = m_data[1];
    uint8_t m_date = m_data[2];
    uint8_t m_hour = m_data[3];
    uint8_t m_minu = m_data[4];
    uint8_t m_secs = m_data[6];

    SensorProtocol *resp = (SensorProtocol *)txBuffer1;
    resp->head = HEADER;
    resp->sj = 0x1234; 
    resp->version = 0X01;
    resp->cmd = HANDSHAKE_CMD;

    // DATA1
    resp->l1 = 6;
    uint8_t *resp_data = resp->data1;
    resp_data[0] = m_year;  // 
    resp_data[1] = m_month; // 
    resp_data[2] = m_date;  // 
    resp_data[3] = m_hour;  // 
    resp_data[4] = m_minu;  // 
    resp_data[5] = m_secs;  // 

    // DATA2
    uint8_t *data3 = &resp_data[resp->l1];
    uint16_t offset = 0;
    data3[offset++] = 2;
    data3[offset++] = m_sensorType111;
    data3[offset++] = 0;

    // DATA3
    uint8_t *data2 = &resp_data[resp->l1 + 3];
    offset = 0;
    data2[offset++] = 28; // L2

    // data1~data12 (ASCII "20250521")
    const char *device_IDStr = "20250712";
    uint8_t lenDeviceIDStr = strlen(device_IDStr);
    if ((offset + lenDeviceIDStr) < 50)
        memcpy(data2 + offset, device_IDStr, lenDeviceIDStr);
    offset += strlen(device_IDStr);
    for (int i = strlen(device_IDStr); i < 8; i++)
    {
        data2[offset++] = ' '; // 
    }

    // uint32_t program_version = PARA_TABLE_USE.data.programVerison;
    const char *program_version = "01.00.00";
    memcpy(data2 + offset, program_version, strlen(program_version));
    offset += strlen(program_version);

    for (int i = strlen(program_version); i < 8; i++)
    {
        data2[offset++] = ' '; // 
    }

    uint32_t hardware_version = 0x01;
    WriteU16LittleEndian(&data2[offset], hardware_version);
    offset += 2;

    // 
    const char *tool_fixtureID = "01N";
    memcpy(data2 + offset, tool_fixtureID, strlen(tool_fixtureID));
    offset += strlen(tool_fixtureID);
    // 4
    for (int i = strlen(tool_fixtureID); i < 4; i++)
    {
        data2[offset++] = ' '; // 
    }

    while (offset <= 27)
    {
        data2[offset++] = 0;
    }

    // CRC
    resp->length = 41; // 2 /*CMD*/ + 1 /*L1*/ + resp->l1+=2+1+2+1+28
    resp->crc = CalcCRC16((uint8_t *)&resp->cmd, resp->length);

    // 
    uint16_t total_len_resp = sizeof(SensorProtocol) - 3 + resp->length;
    RS485_PDA_TX_ENABLE();
    HAL_UART_Transmit(&RS485_PDA_USART, txBuffer1, total_len_resp, 100);
}

void HandleHeartbeat(SensorProtocol *pkt)
{
	uint8_t ERR_D = 0;
	uint8_t cargoLift_IN = 1;
    // data1
    uint8_t *m_data1 = pkt->data1;
    uint8_t returnSensorType = m_data1[0];

    // 2 3-10
    // data2
    uint8_t *m_data = &m_data1[pkt->l1];
    char str_sensorAddr[9] = {0}; // char *str_sensorAddr[13];
    uint8_t len = m_data[0];      // 
    if (len > 8)
		{
        len = 8;
		}
    strncpy(str_sensorAddr, (char *)&m_data[1], 8);
    str_sensorAddr[8] = '\0';

    SensorProtocol *resp = (SensorProtocol *)txBuffer2;
    resp->head = HEADER;
    resp->sj = 0x2234; //  GenerateRandom();
    resp->version = 0X01;
    resp->cmd = HEARTBEAT_CMD;

    // DATA1PDA
    resp->l1 = 2;
    uint8_t *resp_data = resp->data1;
    uint16_t offset = 0;

    // DATA1
    if (returnSensorType != m_sensorType111)
    {
        resp_data[offset++] = 0;
    }
    else
    {
        resp_data[offset++] = m_sensorType111;
    }
    resp_data[offset++] = 0;

    resp_data[offset++] = 30;
		if(PARA_TABLE_USE.data.functionChioce == 0x11)
		{
			WriteU16LittleEndian(&resp_data[offset], PARA_TABLE_USE.data.dts6012StudyDistance);
		}
		else
		{
			WriteU16LittleEndian(&resp_data[offset], PARA_TABLE_USE.data.cargoLift_dts6012StudyDistance);			
		}
    offset += 2;
		
		if(PARA_TABLE_USE.data.functionChioce == 0x11)
		{
			WriteU16LittleEndian(&resp_data[offset], PARA_TABLE_USE.data.dts6012DistanceChkThreshold);
		}
		else
		{
			WriteU16LittleEndian(&resp_data[offset], PARA_TABLE_USE.data.cargoLift_dts6012DistanceChkThreshold);
		}
    offset += 2;
		
		if(PARA_TABLE_USE.data.functionChioce == 0x11)
		{
			WriteU16LittleEndian(&resp_data[offset], PARA_TABLE_USE.data.sensorAddr);
		}
    offset += 2;
		
		if(PARA_TABLE_USE.data.functionChioce == 0x11)
		{
			WriteU16LittleEndian(&resp_data[offset], PARA_TABLE_USE.data.closingDoorTime);
    }
		offset += 2;
		
		// byte 9- 10
		if(PARA_TABLE_USE.data.functionChioce == 0x11)
		{
			WriteU16LittleEndian(&resp_data[offset], PARA_TABLE_USE.data.dts6012MinChkDistance);
		}
		else
		{
			WriteU16LittleEndian(&resp_data[offset], PARA_TABLE_USE.data.cargoLift_dts6012MinChkDistance);
		}
		offset += 2;
		
    offset += 2; // 11 12

		if(sensor_status == 1)
		{
			resp_data[offset++] = 0XAA;
		}
		else if(sensor_status == 2)
		{
			resp_data[offset++] = 0XA5;
		}
		else if(sensor_status == -1)
		{
			resp_data[offset++] = 0X55;
		}
		else if(sensor_status == 0)
		{
			resp_data[offset++] = 0XA1;
		}
		else
		{
			resp_data[offset++] = 0X00;
		}
			
			// DATA2 byte14
		if(ERR_D == 0)
		{			
			resp_data[offset++] = 0;   // b14
		}
		else
		{
			resp_data[offset++] = 0/*get_error_count(ERR_D)*/;
		}
		
		if(connectEleFlag == 1){
			resp_data[offset++] = 1; // byte15
		}
		else
		{
			offset++; // byte15
		}
    resp_data[offset++] = ObjectIsDetectedFlag;  //b16
    resp_data[offset++] = ObjectIsDetectedFlag/*IO_ND06 | IO_dts6012*/;   //b17
    offset += 3;   //18 19 20

    /*
        0xAA 
        0x55 
    */
		if(PARA_TABLE_USE.data.functionChioce == 0x11)
		{
			switch (1)
			{
			case 1:
					resp_data[offset++] = 0XAA;   //b21
					resp_data[offset++] = 0/*dts6012_data.objDetectFlag*/;  //b22
					break;
			case 0:
					resp_data[offset++] = 0X55;
					resp_data[offset++] = 0;
					break;
			default:
					break;
			}
			offset += 2;  // b23 24

			switch (nd06OutputEN)//b25 26
			{
			case 1:
					resp_data[offset++] = 0XAA;  
				if(PARA_TABLE_USE.data.sensorAddr != 0x58)
					resp_data[offset++] = nd06AV1C_objDetectFlag;
				else
					resp_data[offset++] = ObjectIsDetectedFlag;
					break;
			case 0:
					resp_data[offset++] = 0X55;
					resp_data[offset++] = 0;
					break;
			default:
					break;
			}
			offset += 4;   // byte27 28 29 30 
		}
		else
		{
			switch (cargoLift_IN)
			{
			case 1:
					resp_data[offset++] = 0XAA;   //b21
					resp_data[offset++] = /*dts6012_data.objDetectFlag*/ 0;  //b22
					break;
			case 0:
					resp_data[offset++] = 0X55;
					resp_data[offset++] = 0;
					break;
			default:
					break;
			}
			offset += 2;  // b23 24

			switch (cargoLift_IN)//b25
			{
			case 1:
					resp_data[offset++] = 0XAA;   
					resp_data[offset++] = nd06AV1C_objDetectFlag;
					break;
			case 0:
					resp_data[offset++] = 0X55;
					resp_data[offset++] = 0;
					break;
			default:
					break;
			}
			offset += 4;   // byte26 27 28 29 			
		}
		
		// DATA3
    resp_data[offset++] = 4;  // b30
    resp_data[offset++] = 1;	// b31
    resp_data[offset++] = 1;  // b32
    offset += 2;

    // DATA4
    resp_data[offset++] = 80;

    // b1-2
    WriteU16LittleEndian(&resp_data[offset], CSpara.closeDoorCmd);  //dts6012_data.firstPeakDistance);
    offset += 2;
    // b3-4
    WriteU16LittleEndian(&resp_data[offset], CSpara.CLS); //dts6012_data.firstPeakAmp);
    offset += 2;
    // b5-6
    WriteU16LittleEndian(&resp_data[offset], CSpara.openDoorCmd); //dts6012_data.secondPeakDistance);
    offset += 2;
    // b7-b8
    WriteU16LittleEndian(&resp_data[offset], CSpara.OLS); //dts6012_data.secondPeakAmp);
    offset += 2;

    // b9-b40 ND06 distance
    uint32_t pixel_dist;
    for (uint8_t i = 0; i < 4; i++)
    {
			for (uint8_t j = 0; j < 4; j++)
			{
				if(PARA_TABLE_USE.data.sensorAddr == 0x58)
				{
					if(current_vl_resl == 4)
					{
						pixel_dist = Results.distance_mm[2*(i * 4 + j) + 1];
					}
					else
					{
						uint8_t original_index, row, col;
						switch (PARA_TABLE_USE.data.closingDoorTimeThreshold)//b25
						{
							case 100:
								row = (i+1)*2;              // 实际行号：5,6,7,8
							  col = j+1;          // 实际列号：2,4,6,8
							  break;
							case 200:
							  row = (i+1)*2+8;              // 实际行号：5,6,7,8
							  col = j+1;          // 实际列号：2,4,6,8
								break;
							case 300:
								row = (i+1)*2+64;              // 实际行号：5,6,7,8
								col = j+5;          // 实际列号：2,4,6,8
								break;
							case 400:
								row =  (i+1)*2+72;              // 实际行号：5,6,7,8
								col = j+5;          // 实际列号：1,3,5,7
								break;
							default:
								break;
						}
						
						// 一维数组索引计算：col * 16 + row，注意col需要减1因为列号从1开始
						original_index = (col ) * 16 + row ;
						if(Results.distance_mm[original_index-1] > PARA_TABLE_USE.data.dts6012StudyDistance)
							pixel_dist = Results.distance_mm[original_index-1];
						else
							pixel_dist = Results.distance_mm[original_index-2];
					}
					
				}
				else
				{
					pixel_dist = nd06_data.dep[i * 4 + j];
				}
				
				if(j >= work_j && j < 4-work_j)
					WriteU16LittleEndian(&resp_data[offset], pixel_dist);
				else
					WriteU16LittleEndian(&resp_data[offset], 0);
				 offset += 2;
			}
		}

    // b41-72  ND06 amp
    uint32_t pixel_amp;
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 4; j++)
        {
					if(PARA_TABLE_USE.data.sensorAddr == 0x58)
					{
						if(current_vl_resl == 4)
						{
							pixel_amp = Results.distance_mm[2*(i * 4 + j)];
						}
						else
						{
//							 int row = 4 + i;              // 实际行号：5,6,7,8
//							int col = 10 + 2 * j;         // 实际列号：10,12,14,16
							int row =  i;              // 实际行号：5,6,7,8
							int col = 9 + 2 * j;         // 实际列号：10,12,14,16
							// 一维数组索引计算：row * 8 + col，注意col需要减1因为列号从1开始
							uint8_t original_index  = (row ) * 8 + (col );
							
							pixel_amp = Results.distance_mm[original_index];
						}
					}
					else
					{
            pixel_amp = nd06_data.amp[i * 4 + j];
					}
					
            WriteU16LittleEndian(&resp_data[offset], pixel_amp);
            offset += 2;
        }
    }
    // DATA155 
		//b73 amount of Error
		uint8_t amountError = get_error_count(ERR_D);
		if(amountError > 0 && (amountError <= 12))
		{
			resp_data [offset] = amountError;  // amountError
		}
		else
		{
			resp_data [offset] = 0;
		}
    offset += 8;

    // DATA5
		uint32_t mask = 1;
		for(uint8_t indexE = 0; indexE < 12; indexE++)
		{
			if(/*ErrData.data*/1 & mask)
			{
				resp_data[offset++] = indexE+1;
			}
			mask <<= 1;
		}
		
    offset += (12-amountError);
    //resp_data[offset] = 1;

    // ---------------------------
    // 4. CRC
    // ---------------------------
    resp->length = 135-1; // cmd 2 1 2 1 30 1 4 1 80 1 12 = data5没有L5
    resp->crc = CalcCRC16((uint8_t *)&resp->cmd, resp->length);

    // 
    uint16_t total_len_resp = sizeof(SensorProtocol) - 3 + resp->length;
    RS485_PDA_TX_ENABLE();
    HAL_UART_Transmit(&RS485_PDA_USART, txBuffer2, total_len_resp, 200);
    // HAL_UART_Transmit_IT(&RS485_PDA_USART, txBuffer2, total_len_resp);
    return;
}

uint8_t setResetParaFlg = 0;
uint8_t DTS6012IOEN_PDA = 0;
uint8_t ND06IOEN_PDA = 0;
void HandleControl(SensorProtocol *pkt)
{
    // PDA->SENSOR
    // data1
    uint8_t *m_data1 = pkt->data1;
    uint8_t returnSensorType = m_data1[0]; // L1 BYTE1
    // m_data1[1]

    // data2
    uint8_t *m_data = &m_data1[pkt->l1]; // 2  L2
    char str_sensorAddr[9] = {0};        // char *str_sensorAddr[13];
    uint8_t len = m_data[0];             //
    if (len > 8)
    {
        len = 8;
    }
    strncpy(str_sensorAddr, (char *)&m_data[1], 8); // m_data1[2]-m_data1[9]  ID
    str_sensorAddr[8] = '\0';

    /*
        data3
        - 0x00: cancel
        - 0xAA: set
        - 0X55: cancel
    */
    // L3 = 30 m_data1[10]
    uint8_t *m_data3 = &m_data1[11];       // 2+1+8
    setBeginDebug = m_data3[1];            //
    uint8_t setDts6012En_PDA = m_data3[2]; //
    uint8_t setNd06En_PDA = m_data3[3];    //
    setResetParaFlg = m_data3[4];
    // byte5 free
    uint8_t set_distance_learn = m_data3[6];   //
    uint8_t set_close_time_learn = m_data3[7]; //
		// 8-30 free

    //***********************************************************************************
    // SENSOR do PDA task
    if (setBeginDebug == 0XAA)
    {
        sensor_status = 2/*DistanceThresholdLearning_STATUS*/; // setup_STATUS
    }
    else if (setBeginDebug == 0X55)
    {
/*        
			sensor_status = NormalWorking_STATUS;    // NormalWorking_STATUS
			DistanceThresholdLearningSuccessFlg = 0; // add 2025/11/10
			DistanceThresholdLearningReqFlg = 0;
			ClosingTimeLearningReqFlg = 0;
			*/
    }
		
		 // dts6012 IO
		switch (setDts6012En_PDA)
		{
		case 0xAA:
		DTS6012IOEN_PDA = 1;
				break;

		case 0x55:
			DTS6012IOEN_PDA = 0;

				break;

		default:
				break;
		}

		// ND06 IO
		switch (setNd06En_PDA)
		{
		case 0xAA:
				ND06IOEN_PDA = 1;
				break;

		case 0x55:
				ND06IOEN_PDA = 0;
				break;

		default:
				break;
		}
		
    uint8_t doResetPara = 0;
    if (setResetParaFlg == 0XAA)
    {
			switch (PARA_TABLE_USE.data.functionChioce)
			{
				case 0x11:
				{
//					paraTable11_Reset();
					doResetPara = 1;
					break;
				}
				case 0x22: //
				{
//					paraTable22_Reset();
					doResetPara = 1;
					break;
				}
				default:
					break;
			}
        //paraTable_Reset();    
    }

    if (sensor_status == /*DistanceThresholdLearning_STATUS*/ 2)
    {
        switch (set_distance_learn)
        {
        case 0xAA:
            DistanceThresholdLearningReqFlg = 1;
            break;

        case 0x55:    
            DistanceThresholdLearningReqFlg = 0;
            break;

        default:
            break;
        }

        switch (set_close_time_learn)
        {
        case 0xAA:
            ClosingTimeLearningReqFlg = 1;
            break;

        case 0x55:
            ClosingTimeLearningReqFlg = 0;
            break;

        default:
            break;
        }
    }
    // endif(sensor_status == 2)
		
		// **********************************************************
		// SENSOR -> PDA
    /*
        - 0x88: succeed
        - 0xAA: working
            - 0X55: failed
    */
    SensorProtocol *resp = (SensorProtocol *)txBuffer3;
    resp->head = HEADER;
    resp->sj = 0x2234;
    resp->version = 0X01;
    resp->cmd = CONTROL_CMD;

    // DATA1
    resp->l1 = 2;
    uint8_t *resp_data = resp->data1;
    if (returnSensorType != m_sensorType111)
    {
        resp_data[0] = 0;
    }
    else
    {
        resp_data[0] = m_sensorType111;
    }
    // resp_data[1] = 0;  // 

    // DATA2 L =2
    resp_data[2] = 2;
    switch (setBeginDebug)
    {
    case 0XAA:
		{
        if (sensor_status == /*DistanceThresholdLearning_STATUS*/ 2)
        {
            resp_data[3] = 0X88;
        }
        else
        {
            resp_data[3] = 0X55;
        }
        break;
        // resp_data[3] = 0X88;
		}
    case 0X55:
		{
        if (sensor_status == 1)
        {
            resp_data[3] = 0X88;
        }
        else
        {
            resp_data[3] = 0X55;
        }
        break;
		}
		case 0x66:
		{
			if (sensor_status == /*DistanceThresholdLearning_STATUS*/ 2)
			{
					resp_data[3] = 0X01;
			}
			else
			{
					resp_data[3] = 0X02;
			}
			break;
		}
    default:
        break;
    }
    resp_data[4] = 0;

    // DATA3 L =2
    resp_data[5] = 2;
    switch (setDts6012En_PDA)
    {
    case 0XAA:
		{
			if(DTS6012IOEN_PDA == 1)
			{
				resp_data[6] = 0X88;
			}
			else
			{
				resp_data[6] = 0X55;
			}
        break;
		}
    case 0X55:
		{
      if(DTS6012IOEN_PDA == 0)
			{
				resp_data[6] = 0X88;
			}
			else
			{
				resp_data[6] = 0X55;
			}
        break;
		}
    default:
        break;
    }
    resp_data[7] = 0;

    // DATA4
    resp_data[8] = 2;
    switch (setNd06En_PDA)
    {
    case 0XAA:
		{
      if(ND06IOEN_PDA == 1)
			{
				resp_data[9] = 0X88;
			}
			else
			{
				resp_data[9] = 0X55;
			}
        break;
		}
    case 0X55:
		{
      if(ND06IOEN_PDA == 0)
			{
				resp_data[9] = 0X88;
			}
			else
			{
				resp_data[9] = 0X55;
			}
        break;
		}
    default:
        break;
    }
    resp_data[10] = 0;

    // DATA5
    resp_data[11] = 2;
    if (setResetParaFlg == 0xAA)
    {
        if (doResetPara == 1)
				{
            resp_data[12] = 0X88;
				}
        else
				{
            resp_data[12] = 0X55;
				}
    }
    resp_data[13] = 0;

    // DATA6 
    resp_data[14] = 2;
    // switch () {
    // case 0XAA:
    //     resp_data[15] = 0X88;
    //     break;
    // case 0X55:
    //     resp_data[15] = 0X55;
    //     break;
    // default:
    //     break;
    // }
    // resp_data[16]= 0;

    // DATA7 L = 42
    resp_data[17] = 42;
    switch (set_distance_learn)
    {
    case 0XAA:
		{
//        if ((DistanceThresholdLearningReqFlg == 1) || (DistanceThresholdLearningSuccessFlg == 1))
//        {
//          resp_data[18] = 0X88;
//					if (DistanceThresholdLearningSuccessFlg == 1 /*|| DistanceThresholdLearningSuccessFlg == 0*/)
//					{
//						resp_data[19] = 0;
//						if (PARA_TABLE_USE.data.functionChioce == 0x11)
//						{
//							// 3-4  20 21 dts
//							// 5-6  22  23 free
//							WriteU16LittleEndian(&resp_data[20],  dts6012DistancdLearnValue); //  
//							//7-8 24 25	nd06							
//							WriteU16LittleEndian(&resp_data[24], nd06DistancdLearnValue); //   PARA_TABLE_USE.data.nd06StudyDistance
//						}
//						else if (PARA_TABLE_USE.data.functionChioce == 0x22)
//						{
//							WriteU16LittleEndian(&resp_data[20],  PARA_TABLE_USE.data.cargoLift_dts6012StudyDistance );// dts6012DistancdLearnValue); //    
//							WriteU16LittleEndian(&resp_data[24], PARA_TABLE_USE.data.cargoLift_nd06StudyPixelDistance[9]  );//nd06StudyDepEven);
//						}
//						// sensorAppLogic 776
//						DistanceThresholdLearningSuccessFlg = 0;
//						dts6012DistanceThresholdLearningSuccessFlg = 0;
//						nd06DistanceThresholdLearningSuccessFlg = 0;
//					}				
//        }
//				else
//				{
					resp_data[18] = 0X55;
					WriteU16LittleEndian(&resp_data[20], 0);
					WriteU16LittleEndian(&resp_data[24], 0);
//				}
        break;
		}
    case 0X55:
        if (DistanceThresholdLearningReqFlg == 0)
        {
            resp_data[18] = 0X88;
        }
        else
        {
            resp_data[18] = 0X55;
        }
        // resp_data[19] = 0;
        // 3-6  20 21 22 23
        WriteU16LittleEndian(&resp_data[20], 0);
        // 7-10  24 25 26 27
        WriteU16LittleEndian(&resp_data[24], 0);
        
        break;

    default:
        break;
    }

    //  11-42 28 -59
    uint8_t idx = 28;
    for (uint8_t i = 0; i < 16; i++)
    {
        WriteU16LittleEndian(&resp_data[idx], PARA_TABLE_USE.data.cargoLift_nd06StudyPixelDistance[i]);
        idx += 2;
    }

    // DATA8
    resp_data[60] = 10; // +43
    switch (set_close_time_learn)
    {
    case 0XAA:
//        if ((ClosingTimeLearningReqFlg == 1) || (ClosingTimeLearningSuccessFlg == 1))
//        {
//            resp_data[61] = 0X88;
//            resp_data[62] = 0;
//            WriteU16LittleEndian(&resp_data[63], PARA_TABLE_USE.data.closingDoorTime);
//        }
//        else if (now_ClosingTimeLearning_Flg == 1)
//        {
//            resp_data[61] = 0XAA;
//            resp_data[62] = 0;
//            WriteU16LittleEndian(&resp_data[63], 0);
//        }
//        //
//        else if (ClosingTimeLearningReqFlg == 0) // ClosingTimeLearningFailed_Flg == 1
//        {
            resp_data[61] = 0X55;
            resp_data[62] = 0;
            WriteU16LittleEndian(&resp_data[63], 0);
//        }

        break;
				// add 2525/12/29
		case 0X66:
		{
//			if(ClosingTimeLearningSuccessFlg == 1 &&
//        now_ClosingTimeLearning_Flg == 0)
//			{
				resp_data[61] = 0X66;
				resp_data[62] = 0;
				WriteU16LittleEndian(&resp_data[63], PARA_TABLE_USE.data.closingDoorTime);
//			}
			break;
		}
    case 0X55:
        if (ClosingTimeLearningReqFlg == 0)
				{
            resp_data[61] = 0X88;
				}
        else
				{
            resp_data[61] = 0X55;
				}

        resp_data[62] = 0;
        WriteU16LittleEndian(&resp_data[63], 0);
        break;
    default:
        break;
    }
    // resp_data[65] = 0; //65-70

    // DATA9
    resp_data[71] = 10;
    // 72-81

    // DATA10
    resp_data[82] = 10;
    // DATA11
    resp_data[93] = 10;

    // CRC
    resp->length = 107;
    resp->crc = CalcCRC16((uint8_t *)&resp->cmd, resp->length);
    //
    uint16_t total_len_resp = sizeof(SensorProtocol) - 3 + resp->length;
    RS485_PDA_TX_ENABLE();
    HAL_UART_Transmit(&RS485_PDA_USART, txBuffer3, total_len_resp, 100);
}

void HandleParamRead(SensorProtocol *pkt)
{
    // data1
    uint8_t *m_data1 = pkt->data1;
    uint8_t returnSensorType = m_data1[0];
    // data2
    uint8_t *m_data = &m_data1[pkt->l1];
    uint8_t groupCount = m_data[1];

    // SENSOR -> PDA
    SensorProtocol *resp = (SensorProtocol *)txBuffer4;
    resp->head = HEADER;
    resp->sj = 0x2234; // 
    resp->version = 0X01;
    resp->cmd = PARAM_READ_CMD;

    resp->l1 = 2;
    uint8_t *resp_data = resp->data1;
    uint16_t offset = 0;

    // DATA1
    if (returnSensorType != m_sensorType111)
    {
        resp_data[offset++] = 0; // 1
    }
    else
    {
        resp_data[offset++] = m_sensorType111;
    }
    offset++; // 2 

    // DATA2
    // PDA
    resp_data[offset++] = 9;                  // 3
    uint8_t *resp_data2 = &resp_data[offset]; // 4
    resp_data2[0] = 1;
    WriteU32LittleEndian(&resp_data2[1], PARA_TABLE_USE.data.programVerison);
    WriteU32LittleEndian(&resp_data2[5], PARA_TABLE_USE.data.functionChioce);

    //  DATA2  44
    uint8_t *data2 = &resp_data2[9]; // DATA2 DATA1  // 10
    offset = 0;

    switch (PARA_TABLE_USE.data.functionChioce)
    {
    case 0x11:
        data2[offset++] = 44; // L2
        // 11 44/4=11
        for (int i = 2; i <= 12; i++)
        {
            uint32_t param = PARA_TABLE_USE.DATE[i];
            WriteU32LittleEndian(&data2[offset], param);
            offset += 4;
        }
        // WriteU32LittleEndian(&data2[offset], closingDoorTimeThreshold);
        WriteU32LittleEndian(&data2[offset], PARA_TABLE_USE.data.closingDoorTimeThreshold);
        offset += 4;
        // CRC
        resp->length = 60;
        resp->crc = CalcCRC16((uint8_t *)&resp->cmd, resp->length);
        break;
    case 0x22:
        // DATA2 92
        data2[offset++] = 92; // L2

        // 14-36 = 23  23*4=92
        for (int i = 13; i <= 35; i++)   //  20260107 CHANGE 
        {
            uint32_t param = PARA_TABLE_USE.DATE[i];

            WriteU32LittleEndian(&data2[offset], param);
            offset += 4;
        }
        // CRC
        resp->length = 108;
        resp->crc = CalcCRC16((uint8_t *)&resp->cmd, resp->length);
        break;

    default:
        data2[offset++] = 1;
        resp->length = 2 + 1 + resp->l1 + 1;
        break;
    }

    uint16_t total_len_resp = sizeof(SensorProtocol) - 3 + resp->length;

    RS485_PDA_TX_ENABLE();
    HAL_UART_Transmit(&RS485_PDA_USART, txBuffer4, total_len_resp, 100);
    return;
}

void HandleParamWrite(SensorProtocol *pkt)
{
		uint8_t changeF1 = 0;
    // data1
    uint8_t *m_data1 = pkt->data1;
    uint8_t returnSensorType = m_data1[0];
    // m_data1[1]  // ???

    // data2
    uint8_t *m_data = &m_data1[pkt->l1];
    uint8_t groupCount = m_data[1]; // 
    // m_data[2] = L2 =6
    uint8_t *writeData = &m_data[4]; // DATA2, pkt->data1[pkt->l1]  L2
    uint8_t writeStatus = 0;         // 

    // F0.11 = 300
    uint8_t groupIndex = writeData[0];    //  11
    uint8_t internalIndex = writeData[1]; //  0
    uint32_t funcValue = (writeData[5] << 24) | (writeData[4] << 16) |
                         (writeData[3] << 8) | writeData[2];

    if (PARA_TABLE_USE.data.functionChioce == 0x11 && groupIndex == 0 && internalIndex >= 3 && internalIndex <= 12) // 13
    {
        PARA_TABLE_USE.DATE[internalIndex - 1] = funcValue;
        writeStatus = 1;
    }
    else if (PARA_TABLE_USE.data.functionChioce == 0x11 && groupIndex == 0 && internalIndex == 13) // F1.13
    {
        // closingDoorTimeThreshold = funcValue;
        PARA_TABLE_USE.DATE[internalIndex - 1] = funcValue;
    }

    else if (PARA_TABLE_USE.data.functionChioce == 0x22 && groupIndex == 0 && internalIndex >= 3 /*14*/ && internalIndex <= 25/*36*/)
    {
			// 3-> 13
        PARA_TABLE_USE.DATE[internalIndex + 10] = funcValue;
        writeStatus = 1;
    }
    else if (groupIndex == 0 && internalIndex == 2)  // 1程序版本不可以修改
    {
			if(funcValue == 0x11 || funcValue == 0x22)  // 只可以写入11或22
			{
        PARA_TABLE_USE.data.functionChioce = funcValue;
				changeF1 = 1;
        writeStatus = 1;
			}
			else
			{
				writeStatus = 0;
			}
    }
    else
    {
        writeStatus = 0; // 
    }

    // Flash
    if (writeStatus)
    {
//        paraTable_Write();
    }

    // SENSOR -> PDA
    SensorProtocol *resp = (SensorProtocol *)txBuffer5;
    resp->head = HEADER;
    resp->sj = 0x2234; // 
    resp->version = 0X01;
    resp->cmd = PARAM_WRITE_CMD;

    resp->l1 = 2;
    uint8_t *resp_data = resp->data1;
    uint16_t offset = 0;
    uint8_t arrHeart1[2] = {0};

    // DATA1
    if (returnSensorType != m_sensorType111)
    {
        arrHeart1[0] = 0;
    }
    else
    {
        arrHeart1[0] = m_sensorType111;
    }
    memcpy(&resp_data[offset], arrHeart1, sizeof(arrHeart1));
    offset += sizeof(arrHeart1);

    // DATA2
    resp_data[offset++] = 2; // 1
    resp_data[offset++] = groupCount;
    resp_data[offset++] = 0;

    // DATA3
    uint8_t *data2 = &resp_data[offset]; // ????DATA2 ????DATA1 2+1+1
    offset = 0;
    data2[offset++] = 6;                  // 
    memcpy(&data2[offset], writeData, 6); // 

    // CRC
    resp->length = 15;
    resp->crc = CalcCRC16((uint8_t *)&resp->cmd, resp->length);

    // 
    uint16_t total_len_resp = sizeof(SensorProtocol) - 3 + resp->length;
    RS485_PDA_TX_ENABLE();
    HAL_UART_Transmit(&RS485_PDA_USART, txBuffer5, total_len_resp, 100);
		if(changeF1 == 1)
		{
			HAL_NVIC_SystemReset();
		}
    return;
}
