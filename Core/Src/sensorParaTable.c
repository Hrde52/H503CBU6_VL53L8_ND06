#include "sensorParaTable.h"
#include <string.h>
//#include "stmflash.h"

#define PARA_TABLE_FLASH_SAVE_ADDR 0x0800E000 // 0x08018000 /* ����FLASH �����ַ */

uint32_t ParaTable_Default[FF_COUNT] =
{
	0x0100, // ����汾
	0x11,	// 0x11ƽ̨�ݣ�0x22����
	0x58,	// ��������ַ  51һ¥53��¥��¥��52

	/*ƽ̨�ݲ���*/
	2300, // dts6012ѧϰ���룬��λmm
	200,  // dts6012�����ֵ����λmm
	4000, // dts6012�������룬��λmm
	200,  // dts6012��С�����룬��λmm
	2300, // nd06ѧϰ���룬��λmm  ��ʯ��¥2100��¥��2300
	200,  // nd06�����ֵ����λmm
	4000, // nd06�������룬��λmm
	200,  // nd06��С�����룬��λmm
	1400, // ����ʱ�䣬��λms    // ����1��1400��С 1200
	100,
	
	/*���ݲ���*/
	2300, // dts6012룬λmm
	300,  // dts6012ֵλmm
	4000, // dts6012룬λmm
	300,  // dts6012λmm
	200,  // nd06ֵλmm
	4000, // nd06λmm
	300,  // nd06λmm
	2300, // nd06��1
	2300, // nd06��2
	2300, // nd06��3
	2300, // nd06��4
	2300, // nd06��5
	2300, // nd06��6
	2300, // nd06��7
	2300, // nd06��8
	2300, // nd06��9
	2300, // nd06��10
	2300, // nd06��11
	2300, // nd06��12
	2300, // nd06��13
	2300, // nd06��14
	2300, // nd06��15
	2300, // nd06��16
	0
};

FF_PARA_USE PARA_TABLE_USE;
FF_PARA_SAVE FF_PARA_TABLE;
uint32_t ParaTableSave[FF_COUNT + 1];

uint32_t paraTableSum(uint32_t *num, uint32_t len)
{
	uint32_t sumValue = 0;
	uint32_t i = 0;
	for (i = 0; i < len; i++)
	{
		sumValue += num[i];
	}
	return sumValue;
}

void paraTable_Write(void)
{
	memcpy(ParaTableSave, PARA_TABLE_USE.DATE, sizeof(PARA_TABLE_USE.DATE));
	ParaTableSave[FF_COUNT] = paraTableSum(PARA_TABLE_USE.DATE, sizeof(PARA_TABLE_USE.DATE) / 4);
//	stmflash_write(PARA_TABLE_FLASH_SAVE_ADDR, ParaTableSave, sizeof(ParaTableSave) / 4);
}

void paraTable_Read(void)
{
//	stmflash_read(PARA_TABLE_FLASH_SAVE_ADDR, ParaTableSave, sizeof(ParaTableSave) / 4);
	memcpy(PARA_TABLE_USE.DATE, ParaTableSave, sizeof(PARA_TABLE_USE.DATE));
}

void paraTable_Init(void)
{
	memcpy(PARA_TABLE_USE.DATE, ParaTable_Default, sizeof(ParaTable_Default));
	/*
	uint32_t datatemp = 0;
	datatemp = stmflash_read_word(PARA_TABLE_FLASH_SAVE_ADDR);
	if (datatemp == 0XFFFFFFFF) // 
	{
		memcpy(PARA_TABLE_USE.DATE, ParaTable_Default, sizeof(ParaTable_Default));
		paraTable_Write();
	}
	else 
	{
		paraTable_Read();
		
	}
	*/
}

void paraTable_Reset(void)
{
//	uint32_t datatemp = 0;
//	datatemp = stmflash_read_word(PARA_TABLE_FLASH_SAVE_ADDR);

	memcpy(PARA_TABLE_USE.DATE, ParaTable_Default, sizeof(ParaTable_Default));
	paraTable_Write();
}

void paraTable11_Reset(void)
{
	memcpy(&PARA_TABLE_USE.DATE[3], &ParaTable_Default[3], 10 * sizeof(uint32_t));
	paraTable_Write();
}

void paraTable22_Reset(void)
{
	memcpy(&PARA_TABLE_USE.DATE[13], &ParaTable_Default[13], 23*sizeof(uint32_t));
	paraTable_Write();
}
