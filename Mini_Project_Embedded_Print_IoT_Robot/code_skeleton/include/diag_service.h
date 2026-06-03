#ifndef DIAG_SERVICE_H
#define DIAG_SERVICE_H

#include "app_types.h"

void DiagService_Init(void);
void DiagService_ReportState(SystemState_t state);
void DiagService_ReportFault(FaultCode_t code);
void DiagService_ReportSensorSnapshot(const SensorSnapshot_t* snapshot);

#endif
