#ifndef __PDA_HANDLER_H
#define __PDA_HANDLER_H

#include "stm32h5xx_hal.h"
#include <stdbool.h>

/* PDA命令类型枚举 */
typedef enum {
    PDA_CMD_HEARTBEAT = 0x01,      // 心跳包
    PDA_CMD_QUERY_STATUS = 0x02,   // 查询状态
    PDA_CMD_SET_PARAM = 0x03,      // 设置参数
    PDA_CMD_CONTROL = 0x04,        // 控制命令
    PDA_CMD_DOWNLOAD_DATA = 0x05,  // 下载数据
    PDA_CMD_UPLOAD_DATA = 0x06,    // 上传数据
    PDA_CMD_FIRMWARE_UPDATE = 0x07 // 固件更新
} PDA_CommandType_t;

/* PDA命令帧结构 */
typedef struct {
    uint8_t start_flag;     // 起始标志 0xAA
    uint8_t device_id[2];   // 设备ID
    uint8_t command;        // 命令字
    uint8_t length;         // 数据长度
    uint8_t data[120];      // 数据域
    uint8_t crc16[2];       // CRC16校验
    uint8_t end_flag;       // 结束标志 0x55
} PDA_CommandFrame_t;

/* PDA响应帧结构 */
typedef struct {
    uint8_t start_flag;     // 起始标志 0xBB
    uint8_t device_id[2];   // 设备ID
    uint8_t response;       // 响应码
    uint8_t length;         // 数据长度
    uint8_t data[120];      // 数据域
    uint8_t crc16[2];       // CRC16校验
    uint8_t end_flag;       // 结束标志 0x66
} PDA_ResponseFrame_t;

/* PDA状态结构 */
typedef struct {
    bool connected;
    uint32_t last_heartbeat_time;
    uint16_t packet_count;
    uint16_t error_count;
    uint8_t signal_strength;
} PDA_Status_t;

/* 函数声明 */
void PDA_Handler_Init(void);
void PDA_Handler_Process(void);
bool PDA_Handler_ReceiveFrame(uint8_t *data, uint16_t length);
void PDA_Handler_SendResponse(PDA_CommandType_t cmd_type, uint8_t *data, uint16_t len);
void PDA_Handler_SendHeartbeat(void);
void PDA_Handler_ProcessCommand(PDA_CommandFrame_t *cmd);
void PDA_Handler_BuildResponse(uint8_t response_code, uint8_t *data, uint16_t len);
bool PDA_Handler_ValidateFrame(uint8_t *data, uint16_t length);
void PDA_Handler_ErrorHandler(uint8_t error_code);
bool PDA_Handler_CheckHeartbeat(void);
void PDA_Handler_SetDeviceID(uint16_t id);
uint16_t PDA_Handler_GetDeviceID(void);
PDA_Status_t PDA_Handler_GetStatus(void);

#endif /* __PDA_HANDLER_H */