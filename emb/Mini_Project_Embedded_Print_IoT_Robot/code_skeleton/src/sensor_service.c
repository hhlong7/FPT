#include "sensor_service.h"

#define SENSOR_ADC_MAX_RAW           (4095U)
#define SENSOR_TEMP_MAX_DECI_C       (1250)
#define SENSOR_TEMP_MIN_DECI_C       (-400)
#define SENSOR_CURRENT_MAX_MA        (3000U)

static uint16_t g_sensor_raw_adc;
static SensorSnapshot_t g_sensor_snapshot;
static bool g_sensor_fault;

static int16_t SensorService_ConvertRawToTemperature(uint16_t raw_adc)
{
    int32_t scaled;

    scaled = (((int32_t)raw_adc * 1650) / (int32_t)SENSOR_ADC_MAX_RAW) - 400;

    if (scaled > SENSOR_TEMP_MAX_DECI_C) {
        scaled = SENSOR_TEMP_MAX_DECI_C;
    }

    if (scaled < SENSOR_TEMP_MIN_DECI_C) {
        scaled = SENSOR_TEMP_MIN_DECI_C;
    }

    return (int16_t)scaled;
}

void SensorService_Init(void)
{
    g_sensor_raw_adc = 0U;
    g_sensor_snapshot.adc_raw = 0U;
    g_sensor_snapshot.temperature_deci_c = 0;
    g_sensor_snapshot.current_milli_amp = 0U;
    g_sensor_snapshot.position_um = 0;
    g_sensor_fault = false;
}

void SensorService_SetRawAdc(uint16_t raw_adc)
{
    g_sensor_raw_adc = raw_adc;
}

Std_ReturnType SensorService_Process(void)
{
    if (g_sensor_raw_adc > SENSOR_ADC_MAX_RAW) {
        g_sensor_fault = true;
        return E_INVALID_PARAM;
    }

    g_sensor_snapshot.adc_raw = g_sensor_raw_adc;
    g_sensor_snapshot.temperature_deci_c = SensorService_ConvertRawToTemperature(g_sensor_raw_adc);
    g_sensor_snapshot.current_milli_amp = (uint16_t)(((uint32_t)g_sensor_raw_adc * SENSOR_CURRENT_MAX_MA) / SENSOR_ADC_MAX_RAW);
    g_sensor_snapshot.position_um += 100U;
    g_sensor_fault = false;

    return E_OK;
}

SensorSnapshot_t SensorService_GetSnapshot(void)
{
    return g_sensor_snapshot;
}

bool SensorService_HasFault(void)
{
    return g_sensor_fault;
}
