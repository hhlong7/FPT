/**
 * @file test_statemachine.c
 * @brief Unit tests for State Machine - Topic 13 (Design Patterns)
 * @note Tests state transitions and event handling
 */
#include "types.h"
#include "statemachine.h"

/* Mock timer for testing */
static uint32_t s_mock_tick = 0U;
static State_t s_current_test_state = STATE_INIT;

/* Test counters */
static uint32_t g_tests_run = 0U;
static uint32_t g_tests_passed = 0U;
static uint32_t g_tests_failed = 0U;

#define TEST_ASSERT(condition) do { \
    g_tests_run++; \
    if (condition) { \
        g_tests_passed++; \
    } else { \
        g_tests_failed++; \
        return FALSE; \
    } \
} while (0)

/* Mock HAL_Timer_GetTick */
uint32_t HAL_Timer_GetTick(void) {
    return s_mock_tick;
}

/* ==================== Mock State Handlers ==================== */

void State_Init_Handler(Event_t event) {
    (void)event;
}

void State_Idle_Handler(Event_t event) {
    if (event == EVENT_START) {
        s_current_test_state = STATE_RUNNING;
    }
}

void State_Running_Handler(Event_t event) {
    if (event == EVENT_STOP) {
        s_current_test_state = STATE_IDLE;
    }
}

void State_Paused_Handler(Event_t event) {
    if (event == EVENT_RESUME) {
        s_current_test_state = STATE_RUNNING;
    }
}

void State_Fault_Handler(Event_t event) {
    (void)event;
}

void State_Reset_Handler(Event_t event) {
    if (event == EVENT_TIMER_100MS) {
        s_current_test_state = STATE_IDLE;
    }
}

void State_Sleep_Handler(Event_t event) {
    if (event == EVENT_WAKE) {
        s_current_test_state = STATE_IDLE;
    }
}

/* ==================== Test Cases ==================== */

/**
 * @test test_valid_transitions
 * @brief Test valid state transitions
 */
static bool test_valid_transitions(void) {
    s_current_test_state = STATE_INIT;

    /* INIT -> IDLE should be valid */
    TEST_ASSERT(StateMachine_CanTransition(STATE_INIT, STATE_IDLE) == TRUE);

    /* IDLE -> RUNNING should be valid */
    TEST_ASSERT(StateMachine_CanTransition(STATE_IDLE, STATE_RUNNING) == TRUE);

    /* IDLE -> FAULT should be valid */
    TEST_ASSERT(StateMachine_CanTransition(STATE_IDLE, STATE_FAULT) == TRUE);

    return TRUE;
}

/**
 * @test test_invalid_transitions
 * @brief Test that invalid transitions are rejected
 */
static bool test_invalid_transitions(void) {
    /* RUNNING -> INIT should be invalid */
    TEST_ASSERT(StateMachine_CanTransition(STATE_RUNNING, STATE_INIT) == FALSE);

    /* FAULT -> IDLE should be invalid (must go through RESET) */
    TEST_ASSERT(StateMachine_CanTransition(STATE_FAULT, STATE_IDLE) == FALSE);

    /* SLEEP -> RUNNING should be invalid (must wake first) */
    TEST_ASSERT(StateMachine_CanTransition(STATE_SLEEP, STATE_RUNNING) == FALSE);

    return TRUE;
}

/**
 * @test test_state_names
 * @brief Test state name retrieval
 */
static bool test_state_names(void) {
    TEST_ASSERT(StateMachine_GetStateName(STATE_INIT) != NULL_PTR);
    TEST_ASSERT(StateMachine_GetStateName(STATE_IDLE) != NULL_PTR);
    TEST_ASSERT(StateMachine_GetStateName(STATE_RUNNING) != NULL_PTR);
    TEST_ASSERT(StateMachine_GetStateName(STATE_PAUSED) != NULL_PTR);
    TEST_ASSERT(StateMachine_GetStateName(STATE_FAULT) != NULL_PTR);
    TEST_ASSERT(StateMachine_GetStateName(STATE_RESET) != NULL_PTR);
    TEST_ASSERT(StateMachine_GetStateName(STATE_SLEEP) != NULL_PTR);

    /* Invalid state should return "UNKNOWN" */
    TEST_ASSERT(StateMachine_GetStateName(STATE_NUM_STATES) != NULL_PTR);

    return TRUE;
}

/**
 * @test test_event_handling
 * @brief Test event processing in different states
 */
static bool test_event_handling(void) {
    StateMachine_Init();

    /* Initial state should be IDLE */
    TEST_ASSERT(StateMachine_GetCurrentState() == STATE_IDLE);

    /* Send START event */
    StateMachine_Process(EVENT_START);
    TEST_ASSERT(StateMachine_GetCurrentState() == STATE_RUNNING);

    /* Send STOP event */
    StateMachine_Process(EVENT_STOP);
    TEST_ASSERT(StateMachine_GetCurrentState() == STATE_IDLE);

    /* Send SLEEP event */
    StateMachine_Process(EVENT_SLEEP);
    TEST_ASSERT(StateMachine_GetCurrentState() == STATE_SLEEP);

    /* Send WAKE event */
    StateMachine_Process(EVENT_WAKE);
    TEST_ASSERT(StateMachine_GetCurrentState() == STATE_IDLE);

    return TRUE;
}

/**
 * @test test_fault_transition
 * @brief Test fault handling flow
 */
static bool test_fault_transition(void) {
    StateMachine_Init();

    /* Move to RUNNING state */
    StateMachine_Process(EVENT_START);
    TEST_ASSERT(StateMachine_GetCurrentState() == STATE_RUNNING);

    /* Trigger FAULT */
    StateMachine_Process(EVENT_FAULT);
    TEST_ASSERT(StateMachine_GetCurrentState() == STATE_FAULT);

    /* FAULT can only go to RESET */
    TEST_ASSERT(StateMachine_CanTransition(STATE_FAULT, STATE_RESET) == TRUE);
    TEST_ASSERT(StateMachine_CanTransition(STATE_FAULT, STATE_IDLE) == FALSE);

    /* Clear fault */
    StateMachine_Process(EVENT_FAULT_CLEAR);
    /* Note: In real implementation, fault must be cleared first */

    return TRUE;
}

/* ==================== Test Runner ==================== */

typedef bool (*TestFunc_t)(void);

static const TestFunc_t g_tests[] = {
    test_valid_transitions,
    test_invalid_transitions,
    test_state_names,
    test_event_handling,
    test_fault_transition
};

#define NUM_TESTS  (sizeof(g_tests) / sizeof(g_tests[0]))

void Test_StateMachine_RunAll(void) {
    uint16_t i;

    for (i = 0U; i < NUM_TESTS; i++) {
        g_tests[i]();
    }
}