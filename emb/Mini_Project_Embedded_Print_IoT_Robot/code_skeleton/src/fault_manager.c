#include "fault_manager.h"

static FaultCode_t g_active_fault;
static FaultEvidence_t g_evidence;
static bool g_has_fault;

void FaultManager_Init(void)
{
    g_active_fault = FAULT_NONE;
    g_evidence.fault_code = FAULT_NONE;
    g_evidence.state_before_fault = SYSTEM_STATE_BOOT;
    g_evidence.timestamp_ms = 0U;
    g_has_fault = false;
}

void FaultManager_Report(FaultCode_t code, SystemState_t state, uint32_t timestamp_ms)
{
    g_active_fault = code;
    g_evidence.fault_code = code;
    g_evidence.state_before_fault = state;
    g_evidence.timestamp_ms = timestamp_ms;
    g_has_fault = (code != FAULT_NONE);
}

FaultCode_t FaultManager_GetActiveFault(void)
{
    return g_active_fault;
}

bool FaultManager_HasActiveFault(void)
{
    return g_has_fault;
}

FaultEvidence_t FaultManager_GetEvidence(void)
{
    return g_evidence;
}

void FaultManager_Clear(void)
{
    g_active_fault = FAULT_NONE;
    g_evidence.fault_code = FAULT_NONE;
    g_has_fault = false;
}
