/**
 * @file motor_stepper.c
 * @brief Stepper motor driver - Topic 13 (Command Pattern)
 * @note No heap allocation, uses command queue
 */
#include "types.h"
#include "config.h"
#include "motor.h"
#include "command.h"

/* Motor state structure */
typedef struct {
    MotorState_t state;
    int32_t current_pos;        /* Current position in steps */
    int32_t target_pos;         /* Target position in steps */
    uint16_t current_speed;     /* Current speed in steps/sec */
    MotorDir_t direction;       /* Current direction */
    uint16_t steps_remaining;  /* Steps left in current move */
    uint32_t step_period_us;    /* Time between steps (microseconds) */
    uint32_t last_step_time;    /* Last step timestamp */
} MotorAxisState_t;

/* Static motor state - Topic 2 (no heap) */
static MotorAxisState_t g_motors[MOTOR_NUM_AXES];

/* Command queue for motor commands - Topic 9 */
#define MOTOR_CMD_QUEUE_SIZE  16U
static MotorCommand_t g_motor_cmd_queue[MOTOR_CMD_QUEUE_SIZE];
static uint8_t g_motor_cmd_head = 0U;
static uint8_t g_motor_cmd_tail = 0U;
static uint8_t g_motor_cmd_count = 0U;

/* Step pulse GPIO (simulated) */
#define STEP_PIN_X   PIN_0
#define STEP_PIN_Y   PIN_1
#define DIR_PIN_X    PIN_2
#define DIR_PIN_Y    PIN_3

/* ==================== Internal Functions ==================== */

/**
 * @brief Calculate steps from position
 */
static inline uint32_t Motor_CalcSteps(int32_t from, int32_t to) {
    int32_t diff = to - from;
    if (diff < 0) {
        diff = -diff;
    }
    return (uint32_t)diff;
}

/**
 * @brief Calculate step period from speed
 * @note speed in steps/sec, period in microseconds
 */
static inline uint32_t Motor_CalcStepPeriod(uint16_t speed) {
    if (speed == 0U) {
        return 0U;
    }
    return 1000000UL / (uint32_t)speed;
}

/**
 * @brief Advance motor one step
 */
static void Motor_Step(MotorAxis_t axis) {
    if (axis >= MOTOR_NUM_AXES) {
        return;
    }

    /* Toggle step pin */
    Pin_t step_pin = (axis == MOTOR_AXIS_X) ? STEP_PIN_X : STEP_PIN_Y;
    HAL_GPIO_TogglePin(step_pin);

    /* Update position */
    if (g_motors[axis].direction == MOTOR_DIR_CW) {
        g_motors[axis].current_pos++;
    } else {
        g_motors[axis].current_pos--;
    }

    g_motors[axis].steps_remaining--;
}

/**
 * @brief Check if motor should step now
 */
static bool Motor_ShouldStep(MotorAxis_t axis) {
    uint32_t current_time = HAL_Timer_GetTick();  /* Returns ms */
    uint32_t elapsed = current_time - g_motors[axis].last_step_time;

    if (elapsed >= g_motors[axis].step_period_us / 1000U) {
        g_motors[axis].last_step_time = current_time;
        return TRUE;
    }
    return FALSE;
}

/* ==================== Motor Command Queue ==================== */

Result_t Motor_QueueCommand(const MotorCommand_t* cmd) {
    if (cmd == NULL_PTR) {
        return ERROR_INVALID_PARAM;
    }

    if (g_motor_cmd_count >= MOTOR_CMD_QUEUE_SIZE) {
        return ERROR_OVERFLOW;
    }

    g_motor_cmd_queue[g_motor_cmd_head] = *cmd;
    g_motor_cmd_head = (g_motor_cmd_head + 1U) & (MOTOR_CMD_QUEUE_SIZE - 1U);
    g_motor_cmd_count++;

    return OK;
}

uint8_t Motor_GetQueueSize(void) {
    return g_motor_cmd_count;
}

bool Motor_IsQueueEmpty(void) {
    return (g_motor_cmd_count == 0U);
}

/* ==================== Public API ==================== */

void Motor_Init(void) {
    uint8_t i;
    for (i = 0U; i < MOTOR_NUM_AXES; i++) {
        g_motors[i].state = MOTOR_STATE_IDLE;
        g_motors[i].current_pos = 0;
        g_motors[i].target_pos = 0;
        g_motors[i].current_speed = MOTOR_DEFAULT_SPEED;
        g_motors[i].direction = MOTOR_DIR_CW;
        g_motors[i].steps_remaining = 0U;
        g_motors[i].step_period_us = Motor_CalcStepPeriod(MOTOR_DEFAULT_SPEED);
        g_motors[i].last_step_time = 0U;
    }

    /* Initialize GPIO pins */
    HAL_GPIO_Init(STEP_PIN_X, PIN_MODE_OUTPUT);
    HAL_GPIO_Init(STEP_PIN_Y, PIN_MODE_OUTPUT);
    HAL_GPIO_Init(DIR_PIN_X, PIN_MODE_OUTPUT);
    HAL_GPIO_Init(DIR_PIN_Y, PIN_MODE_OUTPUT);

    /* Reset command queue */
    g_motor_cmd_head = 0U;
    g_motor_cmd_tail = 0U;
    g_motor_cmd_count = 0U;
}

void Motor_InitAxis(MotorAxis_t axis) {
    if (axis >= MOTOR_NUM_AXES) {
        return;
    }

    g_motors[axis].state = MOTOR_STATE_IDLE;
    g_motors[axis].current_pos = 0;
    g_motors[axis].target_pos = 0;
    g_motors[axis].steps_remaining = 0U;
}

Result_t Motor_MoveAbsolute(MotorAxis_t axis, int32_t position, uint16_t speed) {
    if (axis >= MOTOR_NUM_AXES) {
        return ERROR_INVALID_PARAM;
    }

    if (speed > MOTOR_MAX_SPEED) {
        return ERROR_INVALID_PARAM;
    }

    MotorAxisState_t* m = &g_motors[axis];

    m->target_pos = position;
    m->current_speed = speed;
    m->step_period_us = Motor_CalcStepPeriod(speed);
    m->steps_remaining = Motor_CalcSteps(m->current_pos, position);
    m->last_step_time = HAL_Timer_GetTick();

    /* Set direction */
    if (position > m->current_pos) {
        m->direction = MOTOR_DIR_CW;
        HAL_GPIO_WritePin(DIR_PIN_X, PIN_HIGH);
    } else {
        m->direction = MOTOR_DIR_CCW;
        HAL_GPIO_WritePin(DIR_PIN_X, PIN_LOW);
    }

    m->state = MOTOR_STATE_MOVING;

    return OK;
}

Result_t Motor_MoveRelative(MotorAxis_t axis, int32_t steps, uint16_t speed) {
    if (axis >= MOTOR_NUM_AXES) {
        return ERROR_INVALID_PARAM;
    }

    int32_t target = g_motors[axis].current_pos + steps;
    return Motor_MoveAbsolute(axis, target, speed);
}

Result_t Motor_Stop(MotorAxis_t axis) {
    if (axis >= MOTOR_NUM_AXES) {
        return ERROR_INVALID_PARAM;
    }

    g_motors[axis].steps_remaining = 0U;
    g_motors[axis].state = MOTOR_STATE_IDLE;

    return OK;
}

Result_t Motor_EmergencyStop(void) {
    uint8_t i;
    for (i = 0U; i < MOTOR_NUM_AXES; i++) {
        g_motors[i].steps_remaining = 0U;
        g_motors[i].state = MOTOR_STATE_ERROR;
    }
    return OK;
}

void Motor_Update(void) {
    uint8_t i;

    for (i = 0U; i < MOTOR_NUM_AXES; i++) {
        MotorAxisState_t* m = &g_motors[i];

        if (m->state == MOTOR_STATE_MOVING) {
            if (m->steps_remaining > 0U) {
                if (Motor_ShouldStep(i)) {
                    Motor_Step(i);
                }
            } else {
                /* Move complete */
                m->state = MOTOR_STATE_IDLE;
            }
        }
    }
}

MotorStatus_t Motor_GetStatus(MotorAxis_t axis) {
    MotorStatus_t status;

    if (axis >= MOTOR_NUM_AXES) {
        status.error_code = 0xFFU;
        return status;
    }

    MotorAxisState_t* m = &g_motors[axis];
    status.state = m->state;
    status.current_pos = m->current_pos;
    status.target_pos = m->target_pos;
    status.current_speed = m->current_speed;
    status.steps_remaining = m->steps_remaining;
    status.error_code = 0U;

    return status;
}

Position_t Motor_GetPosition(void) {
    Position_t pos;
    pos.x = g_motors[MOTOR_AXIS_X].current_pos;
    pos.y = g_motors[MOTOR_AXIS_Y].current_pos;
    pos.z = 0;
    return pos;
}

Result_t Motor_MoveHome(MotorAxis_t axis) {
    return Motor_MoveAbsolute(axis, 0, MOTOR_DEFAULT_SPEED);
}

Result_t Motor_HomeAll(void) {
    Result_t res;
    res = Motor_MoveAbsolute(MOTOR_AXIS_X, 0, MOTOR_DEFAULT_SPEED);
    if (IS_ERROR(res)) {
        return res;
    }
    return Motor_MoveAbsolute(MOTOR_AXIS_Y, 0, MOTOR_DEFAULT_SPEED);
}