/**
 * @file nvm.c
 * @brief Non-volatile memory management - Topic 10 (Unit Test)
 * @note Emulates Flash-based NVM for calibration and fault logs
 */
#include "types.h"
#include "config.h"
#include "nvm.h"
#include "bitwise.h"

/* ==================== Static Variables (Stored in Flash) ==================== */
static CalibrationData_t s_calibration __attribute__((section(".rodata"))) = {
    .magic = NVM_CALIBRATION_MAGIC,
    .temp_offset = 0,
    .humidity_offset = 0,
    .pressure_offset = 0,
    .motor_x_steps_per_mm = 80U,
    .motor_y_steps_per_mm = 80U,
    .crc32 = 0U
};

static FaultLog_t s_fault_log __attribute__((section(".bss"))) = {
    .head = 0U,
    .tail = 0U,
    .count = 0U
};

/* Default system config */
static const SystemConfig_t s_default_config = {
    .magic = 0xCAFEBABEU,
    .device_id = 0x12345678U,
    .uart_baud = UART_BAUD_RATE,
    .sensor_enabled = 0x0FU,  /* All sensors enabled */
    .motor_enabled = 0x03U,  /* X, Y motors enabled */
    .crc32 = 0U
};

/* Runtime config (in RAM) */
static SystemConfig_t s_runtime_config;

/* ==================== Internal Functions ==================== */

/**
 * @brief Calculate CRC32 of data
 */
static uint32_t CalcCRC(const void* data, uint32_t len) {
    return CRC32_Calculate((const uint8_t*)data, len);
}

/**
 * @brief Verify calibration data integrity
 */
static bool VerifyCalibration(const CalibrationData_t* cal) {
    if (cal == NULL_PTR) {
        return FALSE;
    }

    if (cal->magic != NVM_CALIBRATION_MAGIC) {
        return FALSE;
    }

    /* Verify CRC (exclude CRC field itself) */
    uint32_t stored_crc = cal->crc32;
    CalibrationData_t temp = *cal;
    temp.crc32 = 0U;
    uint32_t calc_crc = CalcCRC(&temp, sizeof(CalibrationData_t) - sizeof(uint32_t));

    return (stored_crc == calc_crc);
}

/**
 * @brief Update calibration CRC
 */
static void UpdateCalibrationCRC(CalibrationData_t* cal) {
    if (cal == NULL_PTR) {
        return;
    }
    cal->crc32 = 0U;
    cal->crc32 = CalcCRC(cal, sizeof(CalibrationData_t) - sizeof(uint32_t));
}

/* ==================== NVM Public API ==================== */

void NVM_Init(void) {
    FaultLog_Init();

    /* Load config from NVM */
    if (s_default_config.magic == 0xCAFEBABEU) {
        s_runtime_config = s_default_config;
    }
}

NVM_Status_t NVM_ReadCalibration(CalibrationData_t* cal) {
    if (cal == NULL_PTR) {
        return NVM_ERROR_VERIFY;
    }

    *cal = s_calibration;

    if (!VerifyCalibration(cal)) {
        return NVM_ERROR_VERIFY;
    }

    return NVM_OK;
}

NVM_Status_t NVM_WriteCalibration(const CalibrationData_t* cal) {
    if (cal == NULL_PTR) {
        return NVM_ERROR_VERIFY;
    }

    if (!VerifyCalibration(cal)) {
        return NVM_ERROR_VERIFY;
    }

    /* In real implementation, this would write to Flash */
    /* Flash unlock, erase sector, program, lock */
    s_calibration = *cal;

    return NVM_OK;
}

bool NVM_IsCalibrationValid(void) {
    return VerifyCalibration(&s_calibration);
}

/* ==================== Fault Log API ==================== */

void FaultLog_Init(void) {
    s_fault_log.head = 0U;
    s_fault_log.tail = 0U;
    s_fault_log.count = 0U;
}

void FaultLog_Add(uint8_t fault_id, uint8_t severity, uint16_t data) {
    s_fault_log.entries[s_fault_log.head].fault_id = fault_id;
    s_fault_log.entries[s_fault_log.head].severity = severity;
    s_fault_log.entries[s_fault_log.head].timestamp_ms = HAL_Timer_GetTick();
    s_fault_log.entries[s_fault_log.head].data = data;

    /* Advance head with wrap-around */
    s_fault_log.head = (s_fault_log.head + 1U) & (FAULT_LOG_SIZE - 1U);

    /* Increment count (with overflow protection) */
    if (s_fault_log.count < FAULT_LOG_SIZE) {
        s_fault_log.count++;
    } else {
        /* Buffer full - advance tail */
        s_fault_log.tail = (s_fault_log.tail + 1U) & (FAULT_LOG_SIZE - 1U);
    }
}

uint8_t FaultLog_GetCount(void) {
    return s_fault_log.count;
}

bool FaultLog_GetEntry(uint8_t index, FaultLogEntry_t* entry) {
    if (entry == NULL_PTR) {
        return FALSE;
    }

    if (index >= s_fault_log.count) {
        return FALSE;
    }

    uint8_t actual_index = (s_fault_log.tail + index) & (FAULT_LOG_SIZE - 1U);
    *entry = s_fault_log.entries[actual_index];

    return TRUE;
}

void FaultLog_Clear(void) {
    s_fault_log.head = 0U;
    s_fault_log.tail = 0U;
    s_fault_log.count = 0U;
}

/* ==================== System Config API ==================== */

void NVM_LoadConfig(SystemConfig_t* config) {
    if (config == NULL_PTR) {
        return;
    }
    *config = s_runtime_config;
}

void NVM_SaveConfig(const SystemConfig_t* config) {
    if (config == NULL_PTR) {
        return;
    }

    /* Verify config */
    SystemConfig_t temp = *config;
    temp.crc32 = 0U;
    uint32_t calc_crc = CalcCRC(&temp, sizeof(SystemConfig_t) - sizeof(uint32_t));

    /* Update CRC and save */
    s_runtime_config = *config;
    s_runtime_config.crc32 = calc_crc;

    /* In real implementation, write to Flash */
}

uint32_t NVM_CalculateCRC(const void* data, uint32_t len) {
    return CalcCRC(data, len);
}