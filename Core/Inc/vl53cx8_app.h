#ifndef VL53CX8_APP_H
#define VL53CX8_APP_H

#include "vl53l8cx_api.h"

extern VL53L8CX_ResultsData Results;

uint8_t vl53cx8_device_init(VL53L8CX_Configuration *Dev, uint8_t Resolution, uint8_t freq);
uint8_t vl53cx8_ranging_data(VL53L8CX_Configuration *Dev, VL53L8CX_ResultsData *Results);

#endif // VL53CX8_APP_H
