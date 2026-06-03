#ifndef WATCHDOG_SUPERVISOR_H
#define WATCHDOG_SUPERVISOR_H

#include "platform_types.h"

void WatchdogSupervisor_Init(void);
void WatchdogSupervisor_SetSensorHealthy(bool healthy);
void WatchdogSupervisor_SetCommHealthy(bool healthy);
void WatchdogSupervisor_SetApplicationHealthy(bool healthy);
bool WatchdogSupervisor_CanKick(void);

#endif
