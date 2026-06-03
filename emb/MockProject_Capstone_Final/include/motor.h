/**
 * @file motor.h
 * @brief Motor control interface - Topic 13 (Design Patterns) - Command Pattern
 */
#ifndef MOTOR_H
#define MOTOR_H

#include "types.h"
#include "config.h"

/* Motor axis definitions */
typedef enum {
    MOTOR_AXIS_X = 0U,
    MOTOR_AXIS_Y = 1U,
    MOTOR_AXIS_Z = 2U,
    MOTOR_NUM_AXES = 3U
} MotorAxis_t;

/* Motor state */
typedef enum {
    MOTOR_STATE_IDLE = 0U,
    MOTOR_STATE_MOVING = 1U,
    MOTOR_STATE_HOMING = 2U,
    MOTOR_STATE_ERROR = 3U
} MotorState_t;

/* Motor direction */
typedef enum {
    MOTOR_DIR_CW = 0U,   /* Clockwise */
    MOTOR_DIR_CCW = 1U    /* Counter-clockwise */
} MotorDir_t;

/* Position structure */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} Position_t;

/* Motor command - Command Pattern (Topic 13) */
typedef struct {
    uint8_t command_id;
    MotorAxis_t axis;
    int32_t target_position;
    uint16_t speed;
    MotorDir_t direction;
    uint8_t payload[8];
} MotorCommand_t;

/* Motor status */
typedef struct {
    MotorState_t state;
    Position_t current_pos;
    Position_t target_pos;
    uint16_t current_speed;
    uint32_t steps_remaining;
    uint8_t error_code;
} MotorStatus_t;

/* ==================== Motor API ==================== */
void Motor_Init(void);
void Motor_InitAxis(MotorAxis_t axis);

Result_t Motor_MoveAbsolute(MotorAxis_t axis, int32_t position, uint16_t speed);
Result_t Motor_MoveRelative(MotorAxis_t axis, int32_t steps, uint16_t speed);
Result_t Motor_MoveHome(MotorAxis_t axis);
Result_t Motor_Stop(MotorAxis_t axis);
Result_t Motor_EmergencyStop(void);

void Motor_Update(void);  /* Call in main loop or timer ISR */
MotorStatus_t Motor_GetStatus(MotorAxis_t axis);
Position_t Motor_GetPosition(void);

/* Command queue - Command Pattern (Topic 13) */
Result_t Motor_QueueCommand(const MotorCommand_t* cmd);
uint8_t Motor_GetQueueSize(void);
bool Motor_IsQueueEmpty(void);

/* Homing sequence */
Result_t Motor_HomeAll(void);

#endif /* MOTOR_H */