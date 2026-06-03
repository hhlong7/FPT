#ifndef SYSTEM_SM_H
#define SYSTEM_SM_H

#include "app_types.h"

void SystemSm_Init(void);
void SystemSm_HandleEvent(SystemEvent_t event);
SystemState_t SystemSm_GetState(void);
bool SystemSm_IsReadyForWatchdogKick(void);

#endif
