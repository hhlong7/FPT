/**
 * @file nvm.h
 * @brief Non-volatile memory interface - Topic 10 (Unit Test) - Mock-able
 */
#ifndef NVM_H
#define NVM_H

#include "types.h"
#include "config.h"

/* NVM status */
typedef enum {
    NVM_OK = 0U,
    NVM_ERROR_VERIFY = 1U,
    NVM_ERROR_TIMEOUT = 2U,
    NVM_ERROR_LOCKED = 3U
} NVM_Status_t;

/* Calibration data structure - Struct optimization (Topic 12) */
typedef struct __attribute__((packed)) {
    uint32_t magic;              /* Must be NVM_CALIBRATION_MAGIC */
    int16_t temp_offset;
    int16_t humidity_offset;
    int16_t pressure_offset;
    uint16_t motor_x_steps_per_mm;
    uint16_t motor_y_steps_per_mm;
    uint32_t crc32;
} CalibrationData_t;

/* Assert struct size is known at compile time - no padding issues */
_Static_assert(sizeof(CalibrationData_t) == 24U, "CalibrationData_t size mismatch");

/* Fault log entry */
typedef struct {
    uint8_t fault_id;
    uint8_t severity;       /* 1=WARNING, 2=ERROR, 3=CRITICAL */
    uint32_t timestamp_ms;
    uint16_t data;
} FaultLogEntry_t;

/* Fault log - Ring buffer in NVM */
#define FAULT_LOG_SIZE  16U

typedef struct {
    uint8_t head;
    uint8_t tail;
    uint8_t count;
    FaultLogEntry_t entries[FAULT_LOG_SIZE];
} FaultLog_t;

/* System configuration - Stored in NVM */
typedef struct {
    uint32_t magic;
    uint32_t device_id;
    uint16_t uart_baud;
    uint8_t sensor_enabled;
    uint8_t motor_enabled;
    uint32_t crc32;
} SystemConfig_t;

/* ==================== NVM API ==================== */
void NVM_Init(void);
NVM_Status_t NVM_ReadCalibration(CalibrationData_t* cal);
NVM_Status_t NVM_WriteCalibration(const CalibrationData_t* cal);
bool NVM_IsCalibrationValid(void);

/* ==================== Fault Log API ==================== */
void FaultLog_Init(void);
void FaultLog_Add(uint8_t fault_id, uint8_t severity, uint16_t data);
uint8_t FaultLog_GetCount(void);
bool FaultLog_GetEntry(uint8_t index, FaultLogEntry_t* entry);
void FaultLog_Clear(void);

/* ==================== System Config API ==================== */
void NVM_LoadConfig(SystemConfig_t* config);
void NVM_SaveConfig(const SystemConfig_t* config);
uint32_t NVM_CalculateCRC(const void* data, uint32_t len);

#endif /* NVM_H */