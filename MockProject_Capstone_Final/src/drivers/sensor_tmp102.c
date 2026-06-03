/**
 * @file sensor_tmp102.c
 * @brief TMP102 I2C Temperature Sensor Driver - Topic 7 (HAL Strategy)
 */
#include "types.h"
#include "config.h"
#include "sensor.h"
#include "hal.h"

/* TMP102 I2C Address */
#define TMP102_ADDR         0x48U

/* TMP102 Registers */
#define TMP102_REG_TEMP     0x00U
#define TMP102_REG_CONFIG   0x01U

/* TMP102 Configuration */
#define TMP102_CONFIG_RES   0x00U

/* Sensor state */
static int16_t s_temp_raw = 0;
static SensorStatus_t s_temp_status = SENSOR_OK;
static uint32_t s_sample_count = 0U;
static uint32_t s_error_count = 0U;

/* Calibration offset */
static int16_t s_calibration_offset = 0;

/* ==================== TMP102 Specific Functions ==================== */

static Result_t TMP102_ReadRegister(uint8_t reg, uint8_t* data, uint8_t len) {
    Result_t result;

    /* Write register address */
    result = HAL_I2C_WriteReg(TMP102_ADDR, reg, 0U);
    if (IS_ERROR(result)) {
        return result;
    }

    /* Read data */
    return HAL_I2C_Read(TMP102_ADDR, data, len);
}

static Result_t TMP102_WriteRegister(uint8_t reg, uint8_t value) {
    return HAL_I2C_WriteReg(TMP102_ADDR, reg, value);
}

/* ==================== Public API ==================== */

Result_t Sensor_TMP102_Init(void) {
    /* Configure TMP102 for 12-bit resolution */
    Result_t result = TMP102_WriteRegister(TMP102_REG_CONFIG, TMP102_CONFIG_RES);
    if (IS_ERROR(result)) {
        s_temp_status = SENSOR_ERROR_HARDWARE;
        return result;
    }

    s_temp_status = SENSOR_OK;
    s_sample_count = 0U;
    s_error_count = 0U;
    s_calibration_offset = 0;

    return OK;
}

int16_t Sensor_TMP102_ReadTemperature(void) {
    uint8_t data[2U] = {0U, 0U};
    Result_t result;

    /* Read 2 bytes from temperature register */
    result = HAL_I2C_Read(TMP102_ADDR, data, 2U);

    if (IS_ERROR(result)) {
        s_error_count++;
        s_temp_status = SENSOR_ERROR_TIMEOUT;
        return s_temp_raw;  /* Return last known value */
    }

    /* TMP102 data is in big-endian format (12-bit, left-aligned) */
    /* Convert to signed 16-bit value */
    int16_t raw = (int16_t)((((uint16_t)data[0U]) << 8U) | data[1U]);

    /* Right-shift by 4 bits (12-bit resolution) */
    raw = raw >> 4U;

    /* Apply calibration offset */
    raw += s_calibration_offset;

    /* Clamp to valid range */
    if (raw < TEMP_MIN_Celsius) {
        raw = TEMP_MIN_Celsius;
        s_temp_status = SENSOR_ERROR_OUT_OF_RANGE;
    } else if (raw > TEMP_MAX_Celsius) {
        raw = TEMP_MAX_Celsius;
        s_temp_status = SENSOR_ERROR_OUT_OF_RANGE;
    } else {
        s_temp_status = SENSOR_OK;
    }

    s_temp_raw = raw;
    s_sample_count++;

    return raw;
}

/* ==================== Generic Sensor API ==================== */

void Sensor_InitAll(void) {
    /* Initialize all sensors */
    Sensor_TMP102_Init();
    /* Other sensors would be initialized here */
}

Result_t Sensor_ReadAll(SensorData_t* data) {
    if (data == NULL_PTR) {
        return ERROR_INVALID_PARAM;
    }

    /* Read temperature from TMP102 */
    int16_t temp = Sensor_TMP102_ReadTemperature();
    data->temperature_celsius = (float)temp / 16.0f;  /* 12-bit resolution */

    /* Read humidity (simulated) */
    data->humidity_percent = 50.0f;

    /* Read pressure (simulated) */
    data->pressure_kpa = 101.3f;

    /* Read limit switch */
    data->limit_switch_triggered = Sensor_LimitSwitch_IsTriggered();

    /* Update sample count */
    data->sample_count++;
    data->error_count = s_error_count;

    return OK;
}

Result_t Sensor_Calibrate(SensorType_t type, int16_t offset) {
    if (type == SENSOR_TYPE_TEMPERATURE) {
        s_calibration_offset = offset;
        return OK;
    }
    return ERROR_INVALID_PARAM;
}

SensorStatus_t Sensor_GetStatus(SensorType_t type) {
    if (type == SENSOR_TYPE_TEMPERATURE) {
        return s_temp_status;
    }
    return SENSOR_ERROR_HARDWARE;
}

Result_t Sensor_ADC_Init(void) {
    HAL_ADC_Init();
    return OK;
}

uint16_t Sensor_ADC_ReadPressure(void) {
    /* Read from ADC channel 0 */
    uint16_t raw_adc = HAL_ADC_Read(ADC_CHANNEL_0);

    /* Convert ADC value to kPa (0-4095 ADC -> 50-150 kPa) */
    float pressure = 50.0f + ((float)raw_adc / 4095.0f) * 100.0f;

    return (uint16_t)pressure;
}

bool Sensor_LimitSwitch_IsTriggered(void) {
    /* Read limit switch GPIO pin */
    PinState_t state = HAL_GPIO_ReadPin(PIN_7);
    return (state == PIN_LOW);  /* Active low switch */
}