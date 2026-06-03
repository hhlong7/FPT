/**
 * @file statemachine.h
 * @brief State machine interface - Topic 13 (Design Patterns) - State Pattern
 */
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "types.h"
#include "config.h"

/* Main states */
typedef enum {
    STATE_INIT = 0U,
    STATE_IDLE = 1U,
    STATE_RUNNING = 2U,
    STATE_PAUSED = 3U,
    STATE_FAULT = 4U,
    STATE_RESET = 5U,
    STATE_SLEEP = 6U,
    STATE_NUM_STATES = 7U
} State_t;

/* Events */
typedef enum {
    EVENT_NONE = 0U,
    EVENT_START = 1U,
    EVENT_STOP = 2U,
    EVENT_PAUSE = 3U,
    EVENT_RESUME = 4U,
    EVENT_RESET = 5U,
    EVENT_FAULT = 6U,
    EVENT_FAULT_CLEAR = 7U,
    EVENT_SLEEP = 8U,
    EVENT_WAKE = 9U,
    EVENT_CMD_RECEIVED = 10U,
    EVENT_TIMER_10MS = 11U,
    EVENT_TIMER_100MS = 12U,
    EVENT_SENSOR_UPDATE = 13U
} Event_t;

/* State machine context */
typedef struct {
    State_t current_state;
    State_t previous_state;
    uint32_t state_enter_time_ms;
    uint32_t state_execution_count;
    uint8_t fault_reason;
    bool fault_reported;
} StateMachine_t;

/* State handler function pointer - Function Pointer Array (Topic 13) */
typedef void (*StateHandler_t)(Event_t event);

/* ==================== State Machine API ==================== */
void StateMachine_Init(void);
void StateMachine_Process(Event_t event);
State_t StateMachine_GetCurrentState(void);
const char* StateMachine_GetStateName(State_t state);
uint32_t StateMachine_GetStateDuration(void);

/* ==================== State Handlers ==================== */
void State_Init_Handler(Event_t event);
void State_Idle_Handler(Event_t event);
void State_Running_Handler(Event_t event);
void State_Paused_Handler(Event_t event);
void State_Fault_Handler(Event_t event);
void State_Reset_Handler(Event_t event);
void State_Sleep_Handler(Event_t event);

/* ==================== State Transition Validation ==================== */
bool StateMachine_CanTransition(State_t from, State_t to);

#endif /* STATEMACHINE_H */