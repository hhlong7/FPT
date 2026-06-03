#include "sensor_service.h"
#include "comm_service.h"
#include "fault_manager.h"
#include "system_sm.h"
#include "nv_storage.h"
#include "diag_service.h"
#include "watchdog_supervisor.h"

static uint32_t g_timestamp_ms = 0U;

void App_Init(void)
{
    SensorService_Init();
    CommService_Init();
    FaultManager_Init();
    SystemSm_Init();
    NvStorage_Init();
    DiagService_Init();
    WatchdogSupervisor_Init();

    SystemSm_HandleEvent(SYSTEM_EVENT_BOOT);
}

void App_1msTask(void)
{
    Std_ReturnType status;
    SensorSnapshot_t snapshot;

    g_timestamp_ms++;
    status = SensorService_Process();
    snapshot = SensorService_GetSnapshot();
    DiagService_ReportSensorSnapshot(&snapshot);

    if (status != E_OK) {
        FaultEvidence_t evidence;

        FaultManager_Report(FAULT_SENSOR_RANGE, SystemSm_GetState(), g_timestamp_ms);
        SystemSm_HandleEvent(SYSTEM_EVENT_SENSOR_FAULT);
        evidence = FaultManager_GetEvidence();
        NvStorage_SaveFaultEvidence(&evidence);
        WatchdogSupervisor_SetSensorHealthy(false);
    } else {
        WatchdogSupervisor_SetSensorHealthy(true);
    }

    DiagService_ReportState(SystemSm_GetState());
    WatchdogSupervisor_SetApplicationHealthy(SystemSm_IsReadyForWatchdogKick());
}

void App_10msTask(void)
{
    CommandFrame_t frame;

    if (CommService_HasPendingFrame()) {
        if (CommService_DequeueFrame(&frame) == E_OK) {
            SystemSm_HandleEvent(SYSTEM_EVENT_COMMAND_RECEIVED);
            WatchdogSupervisor_SetCommHealthy(true);
        }
    }

    if (CommService_HasTimeout()) {
        FaultManager_Report(FAULT_COMM_TIMEOUT, SystemSm_GetState(), g_timestamp_ms);
        SystemSm_HandleEvent(SYSTEM_EVENT_COMM_TIMEOUT);
        DiagService_ReportFault(FaultManager_GetActiveFault());
        WatchdogSupervisor_SetCommHealthy(false);
    }
}
