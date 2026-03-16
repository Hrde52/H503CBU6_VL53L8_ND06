#ifndef VL53CX8_APP_H
#define VL53CX8_APP_H

#include "vl53l8cx_api.h"

extern VL53L8CX_ResultsData Results;

uint8_t vl53cx8_device_init(VL53L8CX_Configuration *Dev, uint8_t Resolution, uint8_t freq);
uint8_t vl53cx8_ranging_data(VL53L8CX_Configuration *Dev, VL53L8CX_ResultsData *Results);
void process_second_targets(VL53L8CX_ResultsData *results, uint32_t study_Dsitance, uint8_t thresh, uint8_t *detect_flag);

#endif // VL53CX8_APP_H
