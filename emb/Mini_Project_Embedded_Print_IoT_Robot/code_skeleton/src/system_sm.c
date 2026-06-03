#include "system_sm.h"
#include "fault_manager.h"

static SystemState_t g_system_state;

void SystemSm_Init(void)
{
    g_system_state = SYSTEM_STATE_BOOT;
}

void SystemSm_HandleEvent(SystemEvent_t event)
{
    switch (g_system_state) {
        case SYSTEM_STATE_BOOT:
            if (event == SYSTEM_EVENT_BOOT) {
                g_system_state = SYSTEM_STATE_SELF_TEST;
            }
            break;

        case SYSTEM_STATE_SELF_TEST:
            if (event == SYSTEM_EVENT_SELF_TEST_DONE) {
                g_system_state = SYSTEM_STATE_READY;
            }
            break;

        case SYSTEM_STATE_READY:
            if (event == SYSTEM_EVENT_COMMAND_RECEIVED) {
                g_system_state = SYSTEM_STATE_ACTIVE;
            } else if ((event == SYSTEM_EVENT_SENSOR_FAULT) || (event == SYSTEM_EVENT_COMM_TIMEOUT)) {
                g_system_state = SYSTEM_STATE_FAULT;
            } else if (event == SYSTEM_EVENT_SLEEP_REQUEST) {
                g_system_state = SYSTEM_STATE_SLEEP;
            }
            break;

        case SYSTEM_STATE_ACTIVE:
            if ((event == SYSTEM_EVENT_SENSOR_FAULT) || (event == SYSTEM_EVENT_COMM_TIMEOUT)) {
                g_system_state = SYSTEM_STATE_FAULT;
            } else if (event == SYSTEM_EVENT_SLEEP_REQUEST) {
                g_system_state = SYSTEM_STATE_SLEEP;
            }
            break;

        case SYSTEM_STATE_FAULT:
            if ((event == SYSTEM_EVENT_FAULT_ACK) && (!FaultManager_HasActiveFault())) {
                g_system_state = SYSTEM_STATE_READY;
            }
            break;

        case SYSTEM_STATE_SLEEP:
            if (event == SYSTEM_EVENT_WAKEUP_REQUEST) {
                g_system_state = SYSTEM_STATE_READY;
            }
            break;

        default:
            g_system_state = SYSTEM_STATE_FAULT;
            break;
    }
}

SystemState_t SystemSm_GetState(void)
{
    return g_system_state;
}

bool SystemSm_IsReadyForWatchdogKick(void)
{
    return ((g_system_state == SYSTEM_STATE_READY) || (g_system_state == SYSTEM_STATE_ACTIVE));
}
