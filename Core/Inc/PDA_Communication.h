#ifndef __PDA_COMMUNICATION_H__
#define __PDA_COMMUNICATION_H__

#include "main.h"
#include "rs485.h"

#define HANDSHAKE_CMD   0x01
#define HEARTBEAT_CMD   0x02
#define CONTROL_CMD     0x03
#define PARAM_READ_CMD  0x04
#define PARAM_WRITE_CMD 0x07


// void PDA_Communicate(void);
// void cargoLift_PDA_Communicate(void);

extern uint32_t DistanceThresholdLearningReqFlg;
extern uint32_t ClosingTimeLearningReqFlg;
extern uint8_t now_DistanceThresholdLearning_Flg;
extern uint8_t now_ClosingTimeLearning_Flg;

extern uint8_t STOP_DistanceThresholdLearning_ReqFlg;  // PDA停止距离学习
extern uint8_t  STOP_ClosingTimeLearningReqFlg;

extern uint8_t DTS6012IOEN_PDA;
extern uint8_t ND06IOEN_PDA;
//extern uint8_t setResetParaFlg;

uint16_t CalcCRC16(uint8_t *data, uint16_t len);

void WriteU32LittleEndian(uint8_t *buf, uint32_t value);
void WriteU16LittleEndian(uint8_t *buf, uint32_t value);

uint8_t find_bit_position(uint32_t hex_value);
uint8_t get_error_count(uint32_t hex_value);

void ProcessPacket(SensorProtocol *pkt);

void HandleHandshake(SensorProtocol *pkt);

void HandleHeartbeat(SensorProtocol *pkt);

// 3控制命令处理
void HandleControl(SensorProtocol *pkt);
// 4参数读取处理
void HandleParamRead(SensorProtocol *pkt);
// 7参数写入处理
void HandleParamWrite(SensorProtocol *pkt);



#endif