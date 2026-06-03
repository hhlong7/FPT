#ifndef NV_STORAGE_H
#define NV_STORAGE_H

#include "app_types.h"
#include "std_types.h"

#define NV_CONFIG_DATA_LENGTH   (16U)

typedef struct {
    uint16_t version;
    uint16_t length;
    uint8_t data[NV_CONFIG_DATA_LENGTH];
    uint16_t crc16;
} NvConfigBlock_t;

void NvStorage_Init(void);
Std_ReturnType NvStorage_LoadConfig(NvConfigBlock_t* block);
Std_ReturnType NvStorage_SaveConfig(const NvConfigBlock_t* block);
Std_ReturnType NvStorage_SaveFaultEvidence(const FaultEvidence_t* evidence);
uint16_t NvStorage_CalculateCrc16(const uint8_t* data, uint16_t length);

#endif
