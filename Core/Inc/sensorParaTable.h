#ifndef __SENSOR_PARA_TABLE_H__
#define __SENSOR_PARA_TABLE_H__

#include "main.h"

#define FF_COUNT 37  //35

typedef union {
    struct
    {
        uint32_t programVerison; 						// 0.1 0x0100
        uint32_t functionChioce; 						// 0.2 0x11
        uint32_t sensorAddr;     						// 0.3 0x52
        
        uint32_t dts6012StudyDistance;        			// 0.4 2300
        uint32_t dts6012DistanceChkThreshold; 			// 0.5 300
        uint32_t dts6012MaxChkDistance;       			// 0.6 4000
        uint32_t dts6012MinChkDistance;       			// 0.7 300
        uint32_t nd06StudyDistance;           			// 0.8 2300
        uint32_t nd06DistanceChkThreshold;    			// 0.9 300
        uint32_t nd06MaxChkDistance;          			// 0.10 4000
        uint32_t nd06MinChkDistance;          			// 0.11 300
        uint32_t closingDoorTime;            		 	// 0.12 1400
				uint32_t closingDoorTimeThreshold;     // 0.13    100
        
        uint32_t cargoLift_dts6012StudyDistance;        // 0.14 2300
        uint32_t cargoLift_dts6012DistanceChkThreshold; // 0.14 300
        uint32_t cargoLift_dts6012MaxChkDistance;       // 0.15 4000
        uint32_t cargoLift_dts6012MinChkDistance;       // 0.16 300
        uint32_t cargoLift_nd06DistanceChkThreshold;    // 0.17 200
        uint32_t cargoLift_nd06MaxChkDistance;          // 0.18 4000
        uint32_t cargoLift_nd06MinChkDistance;          // 0.19 300
        uint32_t cargoLift_nd06StudyPixelDistance[16];
				uint32_t passToolingStatus;
    } data;
    uint32_t DATE[FF_COUNT];
} FF_PARA_USE;

typedef union {
    struct
    {
        uint32_t F00;
        uint32_t F01;
        uint32_t F02;
        uint32_t F03;
        uint32_t F04;
        uint32_t F05;
        uint32_t F06;
    } data;
    uint32_t DATE[FF_COUNT];
} FF_PARA_SAVE;

extern FF_PARA_SAVE FF_PARA_TABLE;
extern FF_PARA_USE PARA_TABLE_USE;

void paraTable_Write(void);
void paraTable_Read(void);
void paraTable_Init(void);
void paraTable_Reset(void);
void paraTable11_Reset(void);
void paraTable22_Reset(void);
#endif
