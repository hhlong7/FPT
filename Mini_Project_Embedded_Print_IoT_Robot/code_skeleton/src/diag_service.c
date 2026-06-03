#include "diag_service.h"

static SystemState_t g_last_state;
static FaultCode_t g_last_fault;
static SensorSnapshot_t g_last_snapshot;

void DiagService_Init(void)
{
    g_last_state = SYSTEM_STATE_BOOT;
    g_last_fault = FAULT_NONE;
}

void DiagService_ReportState(SystemState_t state)
{
    g_last_state = state;
}

void DiagService_ReportFault(FaultCode_t code)
{
    g_last_fault = code;
}

void DiagService_ReportSensorSnapshot(const SensorSnapshot_t* snapshot)
{
    if (snapshot != 0) {
        g_last_snapshot = *snapshot;
    }
}
