#ifndef __ND06AV1C_APP_H__
#define __ND06AV1C_APP_H__
#include "main.h"
#include "nd06av1c_def.h"

typedef struct 
{
	uint16_t amp[16];
	uint16_t dep[16];
}ND06_DATA;

//struct ND06_DATA nd06_data_default;


/* ND06AV1C */
extern ND06AV1C_Dev_t g_nd06av1c_device;

void ND06AV1C_OFF(void);

void ND06AV1C_ON(void);
void ND06_Reset(void);
void ND06AV1C_Init(void);


int32_t ND06AV1C_Ranging(ND06_DATA *nd06_data, uint8_t used_i, uint8_t used_j, uint8_t *nd06objDetectFlag);

































#endif
