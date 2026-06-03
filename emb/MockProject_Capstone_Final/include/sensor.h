/**
 * @file sensor.h
 * @brief Sensor interface - Topic 7 (OOP Features) - Strategy Pattern
 */
#ifndef SENSOR_H
#define SENSOR_H

#include "types.h"
#include "config.h"

/* Sensor types */
typedef enum {
    SENSOR_TYPE_TEMPERATURE = 0U,
    SENSOR_TYPE_HUMIDITY = 1U,
    SENSOR_TYPE_PRESSURE = 2U,
    SENSOR_TYPE_LIMIT_SWITCH = 3U
} SensorType_t;

/* Sensor status */
typedef enum {
    SENSOR_OK = 0U,
    SENSOR_ERROR_TIMEOUT = 1U,
    SENSOR_ERROR_OUT_OF_RANGE = 2U,
    SENSOR_ERROR_HARDWARE = 3U
} SensorStatus_t;

/* Raw sensor reading */
typedef struct {
    int16_t raw_value;
    uint32_t timestamp_ms;
    SensorStatus_t status;
} SensorReading_t;

/* Calibrated sensor data */
typedef struct {
    float temperature_celsius;
    float humidity_percent;
    float pressure_kpa;
    bool limit_switch_triggered;
    uint32_t sample_count;
    uint32_t error_count;
} SensorData_t;

/* Sensor configuration */
typedef struct {
    SensorType_t type;
    uint8_t sample_interval_ms;
    uint8_t averaging_samples;
    int16_t calibration_offset;
    float calibration_scale;
} SensorConfig_t;

/* Abstract sensor interface - Strategy Pattern (Topic 7) */
typedef struct ISensor ISensor_t;
typedef Result_t (*SensorInitFunc_t)(const ISensor_t* dev);
typedef Result_t (*SensorReadFunc_t)(const ISensor_t* dev, SensorReading_t* reading);
typedef Result_t (*SensorCalibrateFunc_t)(ISensor_t* dev, int16_t offset);

/* Sensor interface vtable - Virtual methods */
struct ISensor {
    SensorType_t type;
    SensorConfig_t config;
    SensorReadFunc_t read;
    SensorCalibrateFunc_t calibrate;
};

/* Temperature sensor interface (I2C) */
typedef struct ITempSensor ITempSensor_t;
typedef int16_t (*TempReadFunc_t)(const ITempSensor_t* dev);

struct ITempSensor {
    uint8_t i2c_address;
    TempReadFunc_t read_temp;
};

/* ==================== Sensor API ==================== */
void Sensor_InitAll(void);
Result_t Sensor_ReadAll(SensorData_t* data);
Result_t Sensor_Calibrate(SensorType_t type, int16_t offset);
SensorStatus_t Sensor_GetStatus(SensorType_t type);

/* Individual sensor functions */
Result_t Sensor_TMP102_Init(void);
int16_t Sensor_TMP102_ReadTemperature(void);

Result_t Sensor_DHT22_Init(void);
float Sensor_DHT22_ReadHumidity(void);

Result_t Sensor_ADC_Init(void);
uint16_t Sensor_ADC_ReadPressure(void);

bool Sensor_LimitSwitch_IsTriggered(void);

#endif /* SENSOR_H */