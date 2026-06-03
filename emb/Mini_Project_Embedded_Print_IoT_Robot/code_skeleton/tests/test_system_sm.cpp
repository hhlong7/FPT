#include <cassert>
extern "C" {
#include "system_sm.h"
#include "fault_manager.h"
}

int main()
{
    SystemSm_Init();
    FaultManager_Init();

    assert(SystemSm_GetState() == SYSTEM_STATE_BOOT);

    SystemSm_HandleEvent(SYSTEM_EVENT_BOOT);
    assert(SystemSm_GetState() == SYSTEM_STATE_SELF_TEST);

    SystemSm_HandleEvent(SYSTEM_EVENT_SELF_TEST_DONE);
    assert(SystemSm_GetState() == SYSTEM_STATE_READY);

    SystemSm_HandleEvent(SYSTEM_EVENT_COMMAND_RECEIVED);
    assert(SystemSm_GetState() == SYSTEM_STATE_ACTIVE);

    SystemSm_HandleEvent(SYSTEM_EVENT_SENSOR_FAULT);
    assert(SystemSm_GetState() == SYSTEM_STATE_FAULT);

    FaultManager_Clear();
    SystemSm_HandleEvent(SYSTEM_EVENT_FAULT_ACK);
    assert(SystemSm_GetState() == SYSTEM_STATE_READY);

    return 0;
}
