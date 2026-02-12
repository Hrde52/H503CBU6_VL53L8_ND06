#ifndef __ELEVATOR_MPU_H
#define __ELEVATOR_MPU_H

#include "stm32h5xx_hal.h"
#include <stdbool.h>

/* 电梯MPU数据结构定义 */
#pragma pack(push, 1) // 1字节对齐

/*
0x87
17
*/
typedef struct
{
    uint8_t header;  // 0x87
    uint8_t command; // 命令字
    uint8_t reserved0;
    uint8_t door_open_width; // 实时开门宽度

    uint8_t command_d2; // 命令字
    uint8_t reserved0_d2;
    uint8_t door_open_width_d2; // 实时开门宽度

    uint8_t inner_order; // 轿厢，内召、电梯运行
    uint8_t reserved2;

    uint8_t reserved[9]; // 保留字段
    uint8_t xor_check;  // 异或校验
} ElevatorMPU_RxFrame_t;


/*
0x86
16
*/
typedef struct {
    uint8_t header;         // 0x86
    uint8_t ack_command;    // 应答命令
    uint8_t sensor_communicate_status;    // 传感器通信状态

    uint8_t ack_command_d2;    // 应答命令
    uint8_t sensor_communicate_status_d2;    // 传感器通信状态

    uint8_t stayed_person;         // 参数2

    uint8_t reserved[9];    // 保留字段
    uint8_t xor_check;      // 异或校验
} ElevatorMPU_TxFrame_t;
#pragma pack(pop)

/* 电梯状态枚举 */
typedef enum
{
    ELEVATOR_IDLE = 0,
    ELEVATOR_RUNNING_UP,
    ELEVATOR_RUNNING_DOWN,
    ELEVATOR_DOOR_OPENING,
    ELEVATOR_DOOR_CLOSING,
    ELEVATOR_DOOR_OPENED,
    ELEVATOR_DOOR_CLOSED,
    ELEVATOR_INNER_ORDER,
    ELEVATOR_EMERGENCY_STOP,
    ELEVATOR_FAULT
} ElevatorState_t;

/* 公共变量声明 */
// extern uint32_t elevator_last_rx_time;
// extern uint32_t elevator_last_tx_time;
extern bool elevator_communication_ok;

/* 函数声明 */
void ElevatorMPU_Init(void);
void ElevatorMPU_Process40ms(void);
bool ElevatorMPU_ValidateFrame(const uint8_t *data);
void ElevatorMPU_PrepareResponse(void);
void ElevatorMPU_SendResponse(void);
// void ElevatorMPU_ErrorHandler(uint8_t error_code);
void ElevatorMPU_SetControlCommand(uint8_t cmd, uint8_t param);
ElevatorState_t ElevatorMPU_GetCurrentState(void);
uint8_t ElevatorMPU_GetCurrentFloor(void);
bool ElevatorMPU_CheckTimeout(void);
void ElevatorMPU_DebugPrintFrame(const uint8_t *data);

#endif /* __ELEVATOR_MPU_H */