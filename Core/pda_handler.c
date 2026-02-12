#include "pda_handler.h"
#include "rs485_common.h"
#include <string.h>

/* 全局变量定义 */
static PDA_Status_t pda_status = {0};
static uint16_t device_id = 0x1234;  // 默认设备ID
static uint8_t rx_buffer[128];
static uint8_t tx_buffer[130];
static bool pending_response = false;

/**
  * @brief 初始化PDA处理器
  */
void PDA_Handler_Init(void)
{
    /* 初始化状态 */
    pda_status.connected = false;
    pda_status.last_heartbeat_time = 0;
    pda_status.packet_count = 0;
    pda_status.error_count = 0;
    pda_status.signal_strength = 0;
    
    /* 配置USART2 DMA接收（启用空闲中断） */
    if (HAL_UART_Receive_DMA(&huart2, rx_buffer, sizeof(rx_buffer)) != HAL_OK)
    {
        PDA_Handler_ErrorHandler(0x01);
    }
    
    /* 使能空闲中断 */
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    
    printf("PDA Handler Module Initialized\r\n");
}

/**
  * @brief PDA主处理函数（在主循环中调用）
  */
void PDA_Handler_Process(void)
{
    /* 检查心跳是否超时 */
    PDA_Handler_CheckHeartbeat();
    
    /* 如果有待发送的响应，发送它 */
    // 这里可以根据需要实现发送队列
    
    /* 定期发送心跳响应（如果收到心跳） */
    // 可以在此处实现周期性状态上报
}

/**
  * @brief 接收并处理PDA数据帧
  * @param data: 接收数据指针
  * @param length: 数据长度
  * @retval true: 处理成功 false: 处理失败
  */
bool PDA_Handler_ReceiveFrame(uint8_t *data, uint16_t length)
{
    /* 验证帧有效性 */
    if (!PDA_Handler_ValidateFrame(data, length))
    {
        pda_status.error_count++;
        return false;
    }
    
    /* 解析命令帧 */
    PDA_CommandFrame_t *cmd_frame = (PDA_CommandFrame_t *)data;
    
    /* 处理命令 */
    PDA_Handler_ProcessCommand(cmd_frame);
    
    pda_status.packet_count++;
    return true;
}

/**
  * @brief 发送响应帧
  * @param cmd_type: 命令类型
  * @param data: 响应数据
  * @param len: 数据长度
  */
void PDA_Handler_SendResponse(PDA_CommandType_t cmd_type, uint8_t *data, uint16_t len)
{
    PDA_ResponseFrame_t *resp = (PDA_ResponseFrame_t *)tx_buffer;
    
    /* 填充响应帧 */
    resp->start_flag = 0xBB;
    resp->device_id[0] = (device_id >> 8) & 0xFF;
    resp->device_id[1] = device_id & 0xFF;
    resp->response = 0x00;  // 成功响应
    
    /* 复制数据 */
    if (len > 0 && data != NULL)
    {
        memcpy(resp->data, data, len > 120 ? 120 : len);
    }
    resp->length = len > 120 ? 120 : len;
    
    resp->end_flag = 0x66;
    
    /* 计算CRC16校验 */
    // 这里需要实现CRC16计算函数
    // resp->crc16 = CalculateCRC16(...);
    
    /* 发送响应 */
    RS485_2_TX_Enable();
    
    uint16_t total_len = sizeof(PDA_ResponseFrame_t) - (120 - resp->length);
    if (HAL_UART_Transmit_DMA(&huart2, tx_buffer, total_len) != HAL_OK)
    {
        PDA_Handler_ErrorHandler(0x02);
    }
    else
    {
        /* 等待发送完成 */
        while(HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX);
        
        /* 切换回接收模式 */
        RS485_2_RX_Enable();
    }
}

/**
  * @brief 发送心跳响应
  */
void PDA_Handler_SendHeartbeat(void)
{
    uint8_t heartbeat_data[4];
    heartbeat_data[0] = 0x01;  // 心跳类型
    heartbeat_data[1] = pda_status.signal_strength;
    heartbeat_data[2] = (pda_status.packet_count >> 8) & 0xFF;
    heartbeat_data[3] = pda_status.packet_count & 0xFF;
    
    PDA_Handler_SendResponse(PDA_CMD_HEARTBEAT, heartbeat_data, 4);
}

/**
  * @brief 处理PDA命令
  * @param cmd: 命令帧指针
  */
void PDA_Handler_ProcessCommand(PDA_CommandFrame_t *cmd)
{
    switch(cmd->command)
    {
        case PDA_CMD_HEARTBEAT:
            /* 处理心跳包 */
            pda_status.last_heartbeat_time = HAL_GetTick();
            pda_status.connected = true;
            PDA_Handler_SendHeartbeat();
            break;
            
        case PDA_CMD_QUERY_STATUS:
            /* 处理状态查询 */
            PDA_Handler_ProcessStatusQuery(cmd);
            break;
            
        case PDA_CMD_SET_PARAM:
            /* 处理参数设置 */
            PDA_Handler_ProcessSetParam(cmd);
            break;
            
        case PDA_CMD_CONTROL:
            /* 处理控制命令 */
            PDA_Handler_ProcessControl(cmd);
            break;
            
        case PDA_CMD_DOWNLOAD_DATA:
            /* 处理数据下载 */
            PDA_Handler_ProcessDataDownload(cmd);
            break;
            
        default:
            /* 未知命令 */
            PDA_Handler_SendErrorResponse(0xFF, "Unknown command");
            break;
    }
}

/**
  * @brief 构建响应帧
  * @param response_code: 响应码
  * @param data: 响应数据
  * @param len: 数据长度
  */
void PDA_Handler_BuildResponse(uint8_t response_code, uint8_t *data, uint16_t len)
{
    // 构建响应帧的通用函数
    // 可以根据需要扩展
}

/**
  * @brief 验证帧的合法性
  * @param data: 数据指针
  * @param length: 数据长度
  * @retval true: 有效帧 false: 无效帧
  */
bool PDA_Handler_ValidateFrame(uint8_t *data, uint16_t length)
{
    /* 检查最小长度 */
    if (length < 8)  // 帧头+设备ID+命令+长度+CRC+帧尾的最小长度
    {
        return false;
    }
    
    /* 检查起始和结束标志 */
    if (data[0] != 0xAA || data[length-1] != 0x55)
    {
        return false;
    }
    
    /* 检查长度字段是否匹配 */
    uint8_t data_length = data[4];
    if (length != (data_length + 8))  // 8个字节的固定头部
    {
        return false;
    }
    
    /* 检查CRC校验（需要实现CRC16函数） */
    // if (!CheckCRC16(data, length))
    // {
    //     return false;
    // }
    
    return true;
}

/**
  * @brief 检查心跳超时
  * @retval true: 心跳正常 false: 心跳超时
  */
bool PDA_Handler_CheckHeartbeat(void)
{
    uint32_t current_time = HAL_GetTick();
    
    if (pda_status.connected && 
        (current_time - pda_status.last_heartbeat_time) > 500)  // 500ms超时
    {
        pda_status.connected = false;
        pda_status.signal_strength = 0;
        return false;
    }
    
    return true;
}

/**
  * @brief 设置设备ID
  * @param id: 设备ID
  */
void PDA_Handler_SetDeviceID(uint16_t id)
{
    device_id = id;
}

/**
  * @brief 获取设备ID
  * @retval 设备ID
  */
uint16_t PDA_Handler_GetDeviceID(void)
{
    return device_id;
}

/**
  * @brief 获取PDA状态
  * @retval PDA状态结构
  */
PDA_Status_t PDA_Handler_GetStatus(void)
{
    return pda_status;
}

/**
  * @brief USART2空闲中断回调函数
  * @param huart: UART句柄指针
  */
void PDA_Handler_IdleCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        /* 停止DMA接收 */
        HAL_UART_DMAStop(&huart2);
        
        /* 计算实际接收长度 */
        uint16_t rx_len = sizeof(rx_buffer) - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
        
        /* 处理接收到的数据 */
        if (rx_len > 0)
        {
            PDA_Handler_ReceiveFrame(rx_buffer, rx_len);
        }
        
        /* 重新启动DMA接收 */
        memset(rx_buffer, 0, sizeof(rx_buffer));
        HAL_UART_Receive_DMA(&huart2, rx_buffer, sizeof(rx_buffer));
    }
}

/* 以下是具体的命令处理函数 */

/**
  * @brief 处理状态查询命令
  */
static void PDA_Handler_ProcessStatusQuery(PDA_CommandFrame_t *cmd)
{
    uint8_t status_data[20];
    
    // 填充状态信息
    status_data[0] = pda_status.connected ? 0x01 : 0x00;
    status_data[1] = pda_status.signal_strength;
    status_data[2] = (pda_status.packet_count >> 8) & 0xFF;
    status_data[3] = pda_status.packet_count & 0xFF;
    status_data[4] = (pda_status.error_count >> 8) & 0xFF;
    status_data[5] = pda_status.error_count & 0xFF;
    
    // 可以添加更多状态信息
    
    PDA_Handler_SendResponse(PDA_CMD_QUERY_STATUS, status_data, 20);
}

/**
  * @brief 处理参数设置命令
  */
static void PDA_Handler_ProcessSetParam(PDA_CommandFrame_t *cmd)
{
    // 解析参数并设置
    // 这里需要根据实际协议实现
    
    uint8_t response_data[4] = {0x00, 0x00, 0x00, 0x00};  // 成功响应
    PDA_Handler_SendResponse(PDA_CMD_SET_PARAM, response_data, 4);
}

/**
  * @brief 处理控制命令
  */
static void PDA_Handler_ProcessControl(PDA_CommandFrame_t *cmd)
{
    // 解析控制命令并执行
    // 可以调用电梯控制接口
    
    uint8_t response_data[4] = {0x00, 0x00, 0x00, 0x00};  // 成功响应
    PDA_Handler_SendResponse(PDA_CMD_CONTROL, response_data, 4);
}

/**
  * @brief 处理数据下载命令
  */
static void PDA_Handler_ProcessDataDownload(PDA_CommandFrame_t *cmd)
{
    // 处理数据下载请求
    // 可以分片发送数据
    
    uint8_t response_data[130];  // 最大130字节
    // 填充需要下载的数据
    
    PDA_Handler_SendResponse(PDA_CMD_DOWNLOAD_DATA, response_data, 130);
}

/**
  * @brief 发送错误响应
  */
static void PDA_Handler_SendErrorResponse(uint8_t error_code, const char *message)
{
    uint8_t error_data[32];
    error_data[0] = error_code;
    strncpy((char *)&error_data[1], message, 31);
    
    // 构建错误响应帧
    // 实际实现中可能需要不同的帧结构
}

/**
  * @brief 错误处理函数
  * @param error_code: 错误代码
  */
void PDA_Handler_ErrorHandler(uint8_t error_code)
{
#ifdef DEBUG_MODE
    printf("PDA Handler Error: 0x%02X\r\n", error_code);
#endif
    
    // 根据错误代码执行恢复操作
}