#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

#include "app_types.h"
#include "std_types.h"

void FaultManager_Init(void);
void FaultManager_Report(FaultCode_t code, SystemState_t state, uint32_t timestamp_ms);
FaultCode_t FaultManager_GetActiveFault(void);
bool FaultManager_HasActiveFault(void);
FaultEvidence_t FaultManager_GetEvidence(void);
void FaultManager_Clear(void);

#endif
