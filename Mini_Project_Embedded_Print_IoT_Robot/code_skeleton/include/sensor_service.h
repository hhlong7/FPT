#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include "app_types.h"
#include "std_types.h"

void SensorService_Init(void);
void SensorService_SetRawAdc(uint16_t raw_adc);
Std_ReturnType SensorService_Process(void);
SensorSnapshot_t SensorService_GetSnapshot(void);
bool SensorService_HasFault(void);

#endif
