/**
 * @file config.h
 * @brief System configuration constants
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

/* System clock configuration */
#define SYSTEM_CLOCK_HZ          72000000UL
#define AHB_CLOCK_HZ             72000000UL
#define APB1_CLOCK_HZ            36000000UL
#define APB2_CLOCK_HZ            72000000UL

/* Timing constants */
#define TICK_RATE_HZ             1000UL
#define SENSOR_SAMPLE_RATE_MS    100U
#define WATCHDOG_TIMEOUT_MS      1000U
#define COMMAND_QUEUE_SIZE       32U
#define SENSOR_BUFFER_SIZE       64U

/* Motor configuration */
#define MOTOR_STEPS_PER_REV      200U
#define MOTOR_MICROSTEP          16U
#define MOTOR_MAX_SPEED          1000U
#define MOTOR_DEFAULT_SPEED      500U

/* Sensor thresholds */
#define TEMP_MIN_Celsius         (-40)
#define TEMP_MAX_Celsius         125
#define TEMP_WARNING_Celsius      80
#define TEMP_CRITICAL_Celsius     100
#define HUMIDITY_MIN_PERCENT     0U
#define HUMIDITY_MAX_PERCENT     100U
#define PRESSURE_MIN_KPA         50U
#define PRESSURE_MAX_KPA         150U

/* UART configuration */
#define UART_BAUD_RATE           115200U
#define UART_TIMEOUT_MS          100U

/* NVM configuration */
#define NVM_BASE_ADDR            0x0801F000UL
#define NVM_SECTOR_SIZE          2048U
#define NVM_CALIBRATION_MAGIC    0xDEADBEEFU

/* Ring buffer sizes (power of 2 for fast index calculation) */
#define RINGBUFFER_SIZE_256      256U
#define RINGBUFFER_SIZE_128      128U
#define RINGBUFFER_SIZE_64       64U

/* Debug enable/disable */
#define DEBUG_MODE               1U
#if DEBUG_MODE == 1U
    #define DEBUG_LOG(...)       UART_Print(__VA_ARGS__)
#else
    #define DEBUG_LOG(...)
#endif

#endif /* CONFIG_H */