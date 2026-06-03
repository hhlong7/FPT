/**
 * @file test_ringbuffer.c
 * @brief Unit tests for Ring Buffer - Topic 10 (Unit Testing)
 * @note Uses Unity test framework pattern (can be ported to Ceedling/GoogleTest)
 */
#include "types.h"
#include "config.h"

/* Mock ring buffer for testing */
#define TEST_BUFFER_SIZE  16U
static uint8_t test_buffer[TEST_BUFFER_SIZE];
static uint16_t test_head = 0U;
static uint16_t test_tail = 0U;
static uint16_t test_count = 0U;

/* Test result tracking */
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

/* ==================== Helper Functions ==================== */

static void Test_RingBuffer_Init(void) {
    test_head = 0U;
    test_tail = 0U;
    test_count = 0U;
}

static bool Test_RingBuffer_Push(uint8_t data) {
    if (test_count >= TEST_BUFFER_SIZE) {
        return FALSE;
    }
    test_buffer[test_head] = data;
    test_head = (test_head + 1U) & (TEST_BUFFER_SIZE - 1U);
    test_count++;
    return TRUE;
}

static bool Test_RingBuffer_Pop(uint8_t* data) {
    if (test_count == 0U) {
        return FALSE;
    }
    *data = test_buffer[test_tail];
    test_tail = (test_tail + 1U) & (TEST_BUFFER_SIZE - 1U);
    test_count--;
    return TRUE;
}

/* ==================== Test Cases ==================== */

/**
 * @test test_empty_buffer
 * @brief Test empty buffer behavior
 */
static bool test_empty_buffer(void) {
    Test_RingBuffer_Init();

    /* Pop should fail on empty buffer */
    uint8_t data = 0U;
    TEST_ASSERT(Test_RingBuffer_Pop(&data) == FALSE);

    /* Count should be 0 */
    TEST_ASSERT(test_count == 0U);

    return TRUE;
}

/**
 * @test test_push_pop_single
 * @brief Test single push/pop operation
 */
static bool test_push_pop_single(void) {
    Test_RingBuffer_Init();

    /* Push one element */
    TEST_ASSERT(Test_RingBuffer_Push(0x55U) == TRUE);
    TEST_ASSERT(test_count == 1U);

    /* Pop should return same element */
    uint8_t data = 0U;
    TEST_ASSERT(Test_RingBuffer_Pop(&data) == TRUE);
    TEST_ASSERT(data == 0x55U);
    TEST_ASSERT(test_count == 0U);

    return TRUE;
}

/**
 * @test test_fifo_order
 * @brief Test FIFO order (First In, First Out)
 */
static bool test_fifo_order(void) {
    Test_RingBuffer_Init();
    uint8_t i;

    /* Push sequence 1-10 */
    for (i = 1U; i <= 10U; i++) {
        TEST_ASSERT(Test_RingBuffer_Push(i));
    }

    /* Pop should return in same order */
    for (i = 1U; i <= 10U; i++) {
        uint8_t data = 0U;
        TEST_ASSERT(Test_RingBuffer_Pop(&data) == TRUE);
        TEST_ASSERT(data == i);
    }

    return TRUE;
}

/**
 * @test test_wrap_around
 * @brief Test buffer wrap-around (power of 2 optimization)
 */
static bool test_wrap_around(void) {
    Test_RingBuffer_Init();
    uint8_t i;

    /* Fill buffer completely */
    for (i = 0U; i < TEST_BUFFER_SIZE; i++) {
        TEST_ASSERT(Test_RingBuffer_Push(i) == TRUE);
        TEST_ASSERT(test_count == (i + 1U));
    }

    /* Buffer should be full */
    TEST_ASSERT(Test_RingBuffer_Push(0xFFU) == FALSE);

    /* Empty and verify order */
    for (i = 0U; i < TEST_BUFFER_SIZE; i++) {
        uint8_t data = 0U;
        TEST_ASSERT(Test_RingBuffer_Pop(&data) == TRUE);
        TEST_ASSERT(data == i);
    }

    /* Should be empty now */
    uint8_t data = 0U;
    TEST_ASSERT(Test_RingBuffer_Pop(&data) == FALSE);

    return TRUE;
}

/**
 * @test test_overwrite_oldest
 * @brief Test that pushing to full buffer overwrites oldest
 */
static bool test_overwrite_oldest(void) {
    Test_RingBuffer_Init();
    uint8_t i;

    /* Fill buffer */
    for (i = 0U; i < TEST_BUFFER_SIZE; i++) {
        TEST_ASSERT(Test_RingBuffer_Push(i) == TRUE);
    }

    /* Push one more (should fail without overwrite behavior) */
    TEST_ASSERT(Test_RingBuffer_Push(0xFFU) == FALSE);

    /* All original elements should still be there */
    for (i = 0U; i < TEST_BUFFER_SIZE; i++) {
        uint8_t data = 0U;
        TEST_ASSERT(Test_RingBuffer_Pop(&data) == TRUE);
        TEST_ASSERT(data == i);
    }

    return TRUE;
}

/* ==================== Test Runner ==================== */

typedef bool (*TestFunc_t)(void);

static const TestFunc_t g_tests[] = {
    test_empty_buffer,
    test_push_pop_single,
    test_fifo_order,
    test_wrap_around,
    test_overwrite_oldest
};

#define NUM_TESTS  (sizeof(g_tests) / sizeof(g_tests[0]))

void Test_RunAll(void) {
    uint16_t i;

    for (i = 0U; i < NUM_TESTS; i++) {
        if (g_tests[i]()) {
            /* Test passed */
        } else {
            /* Test failed - g_tests_failed already incremented */
        }
    }
}

uint32_t Test_GetPassed(void) {
    return g_tests_passed;
}

uint32_t Test_GetFailed(void) {
    return g_tests_failed;
}

uint32_t Test_GetTotal(void) {
    return g_tests_run;
}