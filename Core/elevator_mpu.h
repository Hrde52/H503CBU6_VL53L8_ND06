#ifndef __ELEVATOR_MPU_H
#define __ELEVATOR_MPU_H

#include "stm32h5xx_hal.h"
#include <stdbool.h>

/* 电梯MPU数据结构定义 */
#pragma pack(push, 1)  // 1字节对齐

typedef struct {
    uint8_t header;         // 0x86
    uint8_t command;        // 命令字
    uint8_t floor_num;      // 楼层号
    uint8_t direction;      // 运行方向 0:停止 1:上行 2:下行
    uint8_t status;         // 状态字
    uint8_t door_status;    // 门状态
    uint8_t speed;          // 速度
    uint8_t weight;         // 载重
    uint8_t temperature;    // 温度
    uint8_t error_code;     // 错误代码
    uint8_t reserved[5];    // 保留字段
    uint8_t xor_check;      // 异或校验
} ElevatorMPU_RxFrame_t;

typedef struct {
    uint8_t header;         // 0x86
    uint8_t ack_command;    // 应答命令
    uint8_t control_cmd;    // 控制命令
    uint8_t target_floor;   // 目标楼层
    uint8param param1;      // 参数1
    uint8_t param2;         // 参数2
    uint8_t param3;         // 参数3
    uint8_t time_setting;   // 时间设置
    uint8_t mode_select;    // 模式选择
    uint8_t reserved[6];    // 保留字段
    uint8_t xor_check;      // 异或校验
} ElevatorMPU_TxFrame_t;
#pragma pack(pop)

/* 电梯状态枚举 */
typedef enum {
    ELEVATOR_IDLE = 0,
    ELEVATOR_RUNNING_UP,
    ELEVATOR_RUNNING_DOWN,
    ELEVATOR_DOOR_OPENING,
    ELEVATOR_DOOR_CLOSING,
    ELEVATOR_EMERGENCY_STOP,
    ELEVATOR_FAULT
} ElevatorState_t;

/* 公共变量声明 */
extern uint32_t elevator_last_rx_time;
extern uint32_t elevator_last_tx_time;
extern bool elevator_communication_ok;

/* 函数声明 */
void ElevatorMPU_Init(void);
void ElevatorMPU_Process40ms(void);
bool ElevatorMPU_ValidateFrame(const uint8_t *data);
void ElevatorMPU_PrepareResponse(void);
void ElevatorMPU_SendResponse(void);
void ElevatorMPU_ErrorHandler(uint8_t error_code);
void ElevatorMPU_SetControlCommand(uint8_t cmd, uint8_t param);
ElevatorState_t ElevatorMPU_GetCurrentState(void);
uint8_t ElevatorMPU_GetCurrentFloor(void);
bool ElevatorMPU_CheckTimeout(void);
void ElevatorMPU_DebugPrintFrame(const uint8_t *data);

#endif /* __ELEVATOR_MPU_H */