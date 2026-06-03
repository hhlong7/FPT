#ifndef APP_TYPES_H
#define APP_TYPES_H

#include "platform_types.h"

typedef enum {
    SYSTEM_STATE_BOOT = 0,
    SYSTEM_STATE_SELF_TEST,
    SYSTEM_STATE_READY,
    SYSTEM_STATE_ACTIVE,
    SYSTEM_STATE_FAULT,
    SYSTEM_STATE_SLEEP
} SystemState_t;

typedef enum {
    SYSTEM_EVENT_BOOT = 0,
    SYSTEM_EVENT_SELF_TEST_DONE,
    SYSTEM_EVENT_COMMAND_RECEIVED,
    SYSTEM_EVENT_SENSOR_FAULT,
    SYSTEM_EVENT_COMM_TIMEOUT,
    SYSTEM_EVENT_SLEEP_REQUEST,
    SYSTEM_EVENT_WAKEUP_REQUEST,
    SYSTEM_EVENT_FAULT_ACK
} SystemEvent_t;

typedef enum {
    FAULT_NONE = 0,
    FAULT_SENSOR_RANGE,
    FAULT_COMM_TIMEOUT,
    FAULT_NVM_CRC,
    FAULT_WATCHDOG_MISS,
    FAULT_ACTUATOR_STALL
} FaultCode_t;

typedef struct {
    uint16_t adc_raw;
    int16_t temperature_deci_c;
    uint16_t current_milli_amp;
    int32_t position_um;
} SensorSnapshot_t;

typedef struct {
    uint8_t command_id;
    uint8_t payload[8];
    uint8_t payload_length;
} CommandFrame_t;

typedef struct {
    FaultCode_t fault_code;
    SystemState_t state_before_fault;
    uint32_t timestamp_ms;
} FaultEvidence_t;

#endif
