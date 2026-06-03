#include "watchdog_supervisor.h"

static bool g_sensor_healthy;
static bool g_comm_healthy;
static bool g_application_healthy;

void WatchdogSupervisor_Init(void)
{
    g_sensor_healthy = false;
    g_comm_healthy = false;
    g_application_healthy = false;
}

void WatchdogSupervisor_SetSensorHealthy(bool healthy)
{
    g_sensor_healthy = healthy;
}

void WatchdogSupervisor_SetCommHealthy(bool healthy)
{
    g_comm_healthy = healthy;
}

void WatchdogSupervisor_SetApplicationHealthy(bool healthy)
{
    g_application_healthy = healthy;
}

bool WatchdogSupervisor_CanKick(void)
{
    return (g_sensor_healthy && g_comm_healthy && g_application_healthy);
}
