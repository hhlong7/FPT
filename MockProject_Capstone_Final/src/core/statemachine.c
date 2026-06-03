/**
 * @file statemachine.c
 * @brief State Machine implementation - Topic 13 (Design Patterns)
 * @note Uses Function Pointer Array for O(1) state transition
 */
#include "types.h"
#include "statemachine.h"
#include "config.h"

/* ==================== State Table (Function Pointer Array) ==================== */
/* Topic 13 - State Machine Pattern */
static const StateHandler_t g_state_table[STATE_NUM_STATES] = {
    [STATE_INIT]    = State_Init_Handler,
    [STATE_IDLE]    = State_Idle_Handler,
    [STATE_RUNNING] = State_Running_Handler,
    [STATE_PAUSED]  = State_Paused_Handler,
    [STATE_FAULT]   = State_Fault_Handler,
    [STATE_RESET]   = State_Reset_Handler,
    [STATE_SLEEP]   = State_Sleep_Handler
};

/* State names for debugging */
static const char* const g_state_names[STATE_NUM_STATES] = {
    [STATE_INIT]    = "INIT",
    [STATE_IDLE]    = "IDLE",
    [STATE_RUNNING] = "RUNNING",
    [STATE_PAUSED]  = "PAUSED",
    [STATE_FAULT]   = "FAULT",
    [STATE_RESET]    = "RESET",
    [STATE_SLEEP]    = "SLEEP"
};

/* Valid state transitions - Prevents invalid transitions */
static const uint8_t g_valid_transitions[STATE_NUM_STATES] = {
    [STATE_INIT]    = (1U << STATE_IDLE),
    [STATE_IDLE]    = (1U << STATE_RUNNING) | (1U << STATE_SLEEP) | (1U << STATE_FAULT),
    [STATE_RUNNING] = (1U << STATE_IDLE) | (1U << STATE_PAUSED) | (1U << STATE_FAULT),
    [STATE_PAUSED]  = (1U << STATE_RUNNING) | (1U << STATE_IDLE) | (1U << STATE_FAULT),
    [STATE_FAULT]   = (1U << STATE_RESET),
    [STATE_RESET]   = (1U << STATE_INIT) | (1U << STATE_IDLE),
    [STATE_SLEEP]   = (1U << STATE_IDLE)
};

/* ==================== State Machine Context ==================== */
static StateMachine_t g_sm = {
    .current_state = STATE_INIT,
    .previous_state = STATE_INIT,
    .state_enter_time_ms = 0U,
    .state_execution_count = 0U,
    .fault_reason = 0U,
    .fault_reported = FALSE
};

/* ==================== Internal Functions ==================== */

/**
 * @brief Check if transition from->to is valid
 */
bool StateMachine_CanTransition(State_t from, State_t to) {
    if (from >= STATE_NUM_STATES || to >= STATE_NUM_STATES) {
        return FALSE;
    }
    return ((g_valid_transitions[from] & (1U << to)) != 0U);
}

/**
 * @brief Transition to new state
 */
static void StateMachine_Transition(State_t new_state) {
    if (g_sm.current_state == new_state) {
        return;  /* No transition needed */
    }

    if (!StateMachine_CanTransition(g_sm.current_state, new_state)) {
        /* Invalid transition - log error */
        return;
    }

    g_sm.previous_state = g_sm.current_state;
    g_sm.current_state = new_state;
    g_sm.state_enter_time_ms = HAL_Timer_GetTick();
    g_sm.state_execution_count = 0U;
}

/* ==================== Public API ==================== */

void StateMachine_Init(void) {
    g_sm.current_state = STATE_INIT;
    g_sm.previous_state = STATE_INIT;
    g_sm.state_enter_time_ms = 0U;
    g_sm.state_execution_count = 0U;
    g_sm.fault_reason = 0U;
    g_sm.fault_reported = FALSE;

    StateMachine_Transition(STATE_IDLE);
}

void StateMachine_Process(Event_t event) {
    StateHandler_t handler = g_state_table[g_sm.current_state];
    if (handler != NULL_PTR) {
        handler(event);  /* Call state handler */
        g_sm.state_execution_count++;
    }
}

State_t StateMachine_GetCurrentState(void) {
    return g_sm.current_state;
}

const char* StateMachine_GetStateName(State_t state) {
    if (state >= STATE_NUM_STATES) {
        return "UNKNOWN";
    }
    return g_state_names[state];
}

uint32_t StateMachine_GetStateDuration(void) {
    return HAL_Timer_GetTick() - g_sm.state_enter_time_ms;
}

/* ==================== State Handlers ==================== */

/**
 * INIT state handler - System initialization
 */
void State_Init_Handler(Event_t event) {
    (void)event;  /* Unused */
    StateMachine_Transition(STATE_IDLE);
}

/**
 * IDLE state handler - Waiting for commands
 */
void State_Idle_Handler(Event_t event) {
    switch (event) {
        case EVENT_START:
            StateMachine_Transition(STATE_RUNNING);
            break;

        case EVENT_SLEEP:
            StateMachine_Transition(STATE_SLEEP);
            break;

        case EVENT_FAULT:
            StateMachine_Transition(STATE_FAULT);
            break;

        default:
            break;
    }
}

/**
 * RUNNING state handler - Actively processing
 */
void State_Running_Handler(Event_t event) {
    switch (event) {
        case EVENT_STOP:
        case EVENT_PAUSE:
            StateMachine_Transition(STATE_PAUSED);
            break;

        case EVENT_FAULT:
            StateMachine_Transition(STATE_FAULT);
            break;

        case EVENT_TIMER_100MS:
            /* Periodic sensor update check */
            break;

        default:
            break;
    }
}

/**
 * PAUSED state handler - Paused but ready to resume
 */
void State_Paused_Handler(Event_t event) {
    switch (event) {
        case EVENT_RESUME:
            StateMachine_Transition(STATE_RUNNING);
            break;

        case EVENT_STOP:
            StateMachine_Transition(STATE_IDLE);
            break;

        case EVENT_FAULT:
            StateMachine_Transition(STATE_FAULT);
            break;

        default:
            break;
    }
}

/**
 * FAULT state handler - Error condition
 */
void State_Fault_Handler(Event_t event) {
    switch (event) {
        case EVENT_FAULT_CLEAR:
            /* Check if fault is cleared */
            StateMachine_Transition(STATE_RESET);
            break;

        default:
            break;
    }
}

/**
 * RESET state handler - Recovery sequence
 */
void State_Reset_Handler(Event_t event) {
    switch (event) {
        case EVENT_RESET:
            StateMachine_Transition(STATE_INIT);
            break;

        case EVENT_TIMER_100MS:
            /* Wait for reset to complete */
            break;

        default:
            break;
    }
}

/**
 * SLEEP state handler - Low power mode
 */
void State_Sleep_Handler(Event_t event) {
    switch (event) {
        case EVENT_WAKE:
            StateMachine_Transition(STATE_IDLE);
            break;

        default:
            break;
    }
}