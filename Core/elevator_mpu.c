#include "elevator_mpu.h"
#include "rs485_common.h"
#include <string.h>

/* 全局变量定义 */
static ElevatorMPU_RxFrame_t elevator_rx_data;
static ElevatorMPU_TxFrame_t elevator_tx_data;
static ElevatorState_t current_state = ELEVATOR_IDLE;
static uint8_t current_floor = 1;
static uint8_t dma_rx_buffer[16];
static bool new_data_available = false;

uint32_t elevator_last_rx_time = 0;
uint32_t elevator_last_tx_time = 0;
bool elevator_communication_ok = false;

/* 私有函数声明 */
static uint8_t CalculateXOR(const uint8_t *data, uint8_t len);
static void ProcessElevatorData(void);
static void UpdateElevatorState(void);

/**
  * @brief 初始化电梯MPU通信模块
  */
void ElevatorMPU_Init(void)
{
    /*
    memset(rxBuf, 0, RX_BUF_SIZE);
	HAL_UART_Receive_DMA(&huart1, rxBuf, RX_BUF_SIZE);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);	
    */
    /* 初始化数据结构 */
    memset(&elevator_rx_data, 0, sizeof(elevator_rx_data));
    memset(&elevator_tx_data, 0, sizeof(elevator_tx_data));
    
    /* 配置USART1 DMA循环接收 */
    if (HAL_UART_Receive_DMA(&huart1, dma_rx_buffer, 16) != HAL_OK)
    {
        ElevatorMPU_ErrorHandler(0x01);
    }
    
    /* 使能DMA传输完成中断 */
    __HAL_DMA_ENABLE_IT(&hdma_usart1_rx, DMA_IT_TC);
    
    elevator_communication_ok = true;
    // printf("Elevator MPU Module Initialized\r\n");
}

/**
  * @brief 40ms周期处理函数（在定时器中断中调用）
  */
void ElevatorMPU_Process40ms(void)
{
    /* 检查是否有新数据 */
    if (new_data_available)
    {
        /* 处理接收到的数据 */
        ProcessElevatorData();
        new_data_available = false;
    }
    
    /* 准备并发送响应 */
    ElevatorMPU_PrepareResponse();
    ElevatorMPU_SendResponse();
    
    /* 更新最后发送时间 */
    elevator_last_tx_time = HAL_GetTick();
}

/**
  * @brief 验证接收帧的合法性
  * @param data: 接收数据指针
  * @retval true: 有效帧 false: 无效帧
  */
bool ElevatorMPU_ValidateFrame(const uint8_t *data)
{
    /* 检查帧头 */
    if (data[0] != 0x86)
    {
        return false;
    }
    
    /* 检查异或校验 */
    uint8_t calc_xor = CalculateXOR(data, 15);
    if (calc_xor != data[15])
    {
        return false;
    }
    
    /* 检查命令字有效性 */
    uint8_t cmd = data[1];
    if (cmd > 0x0F)  // 假设有效命令范围为0x00-0x0F
    {
        return false;
    }
    
    return true;
}

/**
  * @brief 准备响应帧
  */
void ElevatorMPU_PrepareResponse(void)
{
    /* 填充帧头 */
    elevator_tx_data.header = 0x86;
    
    /* 应答命令（回显接收到的命令） */
    elevator_tx_data.ack_command = elevator_rx_data.command;
    
    /* 控制命令（根据状态设置） */
    switch(current_state)
    {
        case ELEVATOR_IDLE:
            elevator_tx_data.control_cmd = 0x00;
            break;
        case ELEVATOR_RUNNING_UP:
            elevator_tx_data.control_cmd = 0x01;
            break;
        case ELEVATOR_RUNNING_DOWN:
            elevator_tx_data.control_cmd = 0x02;
            break;
        case ELEVATOR_EMERGENCY_STOP:
            elevator_tx_data.control_cmd = 0x03;
            break;
        default:
            elevator_tx_data.control_cmd = 0x00;
    }
    
    /* 目标楼层 */
    elevator_tx_data.target_floor = current_floor;
    
    /* 填充其他参数 */
    elevator_tx_data.param1 = 0x00;  // 根据实际需求设置
    elevator_tx_data.param2 = 0x00;
    elevator_tx_data.param3 = 0x00;
    elevator_tx_data.time_setting = 0x00;
    elevator_tx_data.mode_select = 0x00;
    
    /* 计算校验和 */
    uint8_t *tx_buffer = (uint8_t *)&elevator_tx_data;
    elevator_tx_data.xor_check = CalculateXOR(tx_buffer, 15);
}

/**
  * @brief 发送响应帧
  */
void ElevatorMPU_SendResponse(void)
{
    /* 切换485方向控制为发送模式 */
    RS485_1_TX_Enable();
    
    /* 发送数据 */
    if (HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&elevator_tx_data, 16) != HAL_OK)
    {
        ElevatorMPU_ErrorHandler(0x02);
    }
    else
    {
        /* 等待发送完成（或使用DMA发送完成中断切换） */
        while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
        
        /* 切换回接收模式 */
        RS485_1_RX_Enable();
    }
}

/**
  * @brief DMA接收完成中断回调函数
  * @param huart: UART句柄指针
  */
void ElevatorMPU_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* 验证数据帧 */
        if (ElevatorMPU_ValidateFrame(dma_rx_buffer))
        {
            /* 拷贝有效数据 */
            memcpy(&elevator_rx_data, dma_rx_buffer, 16);
            elevator_last_rx_time = HAL_GetTick();
            new_data_available = true;
            elevator_communication_ok = true;
        }
        else
        {
            ElevatorMPU_ErrorHandler(0x03);
        }
    }
}

/**
  * @brief 设置控制命令
  * @param cmd: 控制命令
  * @param param: 命令参数
  */
void ElevatorMPU_SetControlCommand(uint8_t cmd, uint8_t param)
{
    switch(cmd)
    {
        case 0x01:  // 运行到指定楼层
            current_state = ELEVATOR_RUNNING_UP;  // 或下行
            // 这里可以设置目标楼层等逻辑
            break;
        case 0x02:  // 停止命令
            current_state = ELEVATOR_IDLE;
            break;
        case 0x03:  // 紧急停止
            current_state = ELEVATOR_EMERGENCY_STOP;
            break;
        default:
            break;
    }
}

/**
  * @brief 获取当前电梯状态
  * @retval 电梯状态
  */
ElevatorState_t ElevatorMPU_GetCurrentState(void)
{
    return current_state;
}

/**
  * @brief 获取当前楼层
  * @retval 楼层号
  */
uint8_t ElevatorMPU_GetCurrentFloor(void)
{
    return current_floor;
}

/**
  * @brief 检查通信超时
  * @retval true: 超时 false: 正常
  */
bool ElevatorMPU_CheckTimeout(void)
{
    uint32_t current_time = HAL_GetTick();
    
    if ((current_time - elevator_last_rx_time) > 200)  // 200ms超时
    {
        elevator_communication_ok = false;
        return true;
    }
    
    return false;
}

/**
  * @brief 调试打印帧数据
  * @param data: 数据指针
  */
void ElevatorMPU_DebugPrintFrame(const uint8_t *data)
{
#ifdef DEBUG_MODE
    printf("Elevator Frame: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\r\n");
#endif
}

/* 私有函数实现 */

/**
  * @brief 计算异或校验值
  */
static uint8_t CalculateXOR(const uint8_t *data, uint8_t len)
{
    uint8_t result = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        result ^= data[i];
    }
    return result;
}

/**
  * @brief 处理电梯数据
  */
static void ProcessElevatorData(void)
{
    /* 更新当前楼层 */
    current_floor = elevator_rx_data.floor_num;
    
    /* 更新运行状态 */
    switch(elevator_rx_data.direction)
    {
        case 0:
            current_state = ELEVATOR_IDLE;
            break;
        case 1:
            current_state = ELEVATOR_RUNNING_UP;
            break;
        case 2:
            current_state = ELEVATOR_RUNNING_DOWN;
            break;
    }
    
    /* 处理错误代码 */
    if (elevator_rx_data.error_code != 0)
    {
        ElevatorMPU_ErrorHandler(elevator_rx_data.error_code);
    }
    
    /* 更新电梯状态 */
    UpdateElevatorState();
}

/**
  * @brief 更新电梯状态
  */
static void UpdateElevatorState(void)
{
    // 这里可以实现更复杂的状态机逻辑
    // 例如：根据门状态、速度、重量等信息更新状态
}

/**
  * @brief 错误处理函数
  * @param error_code: 错误代码
  */
void ElevatorMPU_ErrorHandler(uint8_t error_code)
{
    /* 记录错误日志 */
#ifdef DEBUG_MODE
    printf("Elevator MPU Error: 0x%02X\r\n", error_code);
#endif
    
    /* 根据错误代码执行相应的恢复操作 */
    switch(error_code)
    {
        case 0x01:  // DMA初始化失败
            // 尝试重新初始化
            break;
        case 0x02:  // 发送失败
            // 重试发送逻辑
            break;
        case 0x03:  // 数据校验失败
            // 清空缓冲区
            memset(dma_rx_buffer, 0, 16);
            break;
        default:
            break;
    }
}