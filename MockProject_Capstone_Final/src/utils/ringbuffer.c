/**
 * @file ringbuffer.c
 * @brief Generic Ring Buffer implementation - Topic 9 (Data Structures)
 * @note Size MUST be power of 2 for fast modulo using bitwise AND
 */
#include "types.h"
#include "config.h"

/* Ring buffer structure - No heap allocation (MISRA compliant) */
typedef struct {
    uint8_t* buffer;
    uint16_t size;           /* Must be power of 2 */
    uint16_t head;           /* Write index */
    uint16_t tail;           /* Read index */
    volatile uint16_t count; /* Current count - volatile for ISR access */
} RingBuffer_t;

/* Static allocation - Topic 2 (Memory Layout) */
static uint8_t s_tx_buffer[RINGBUFFER_SIZE_256];
static uint8_t s_rx_buffer[RINGBUFFER_SIZE_256];
static RingBuffer_t s_uart_tx = { .buffer = s_tx_buffer, .size = RINGBUFFER_SIZE_256 };
static RingBuffer_t s_uart_rx = { .buffer = s_rx_buffer, .size = RINGBUFFER_SIZE_256 };

/* Static assert - Size must be power of 2 (Topic 8 - static_assert) */
_Static_assert((RINGBUFFER_SIZE_256 & (RINGBUFFER_SIZE_256 - 1U)) == 0U,
                "RINGBUFFER_SIZE_256 must be power of 2");
_Static_assert((RINGBUFFER_SIZE_128 & (RINGBUFFER_SIZE_128 - 1U)) == 0U,
                "RINGBUFFER_SIZE_128 must be power of 2");

/* ==================== Internal Functions ==================== */

/**
 * @brief Calculate next index using bitwise AND (fast modulo)
 * @note (x + 1) & (size - 1) is equivalent to (x + 1) % size
 *       but much faster (1 CPU cycle vs 30+ cycles)
 * @param current Current index
 * @param size Ring buffer size (power of 2)
 * @return Next index
 */
static inline uint16_t RingBuffer_Next(uint16_t current, uint16_t size) {
    return (current + 1U) & (size - 1U);
}

/**
 * @brief Check if ring buffer is full
 */
static inline bool RingBuffer_IsFull(const RingBuffer_t* rb) {
    return (rb->count >= rb->size);
}

/**
 * @brief Check if ring buffer is empty
 */
static inline bool RingBuffer_IsEmpty(const RingBuffer_t* rb) {
    return (rb->count == 0U);
}

/* ==================== Public API ==================== */

void RingBuffer_Init(RingBuffer_t* rb, uint8_t* buffer, uint16_t size) {
    rb->buffer = buffer;
    rb->size = size;
    rb->head = 0U;
    rb->tail = 0U;
    rb->count = 0U;
}

bool RingBuffer_Push(RingBuffer_t* rb, uint8_t data) {
    if (RingBuffer_IsFull(rb)) {
        return FALSE;  /* Buffer overflow - Topic 3 */
    }

    rb->buffer[rb->head] = data;
    rb->head = RingBuffer_Next(rb->head, rb->size);
    rb->count++;
    return TRUE;
}

bool RingBuffer_Pop(RingBuffer_t* rb, uint8_t* data) {
    if (RingBuffer_IsEmpty(rb)) {
        return FALSE;  /* Buffer underflow */
    }

    *data = rb->buffer[rb->tail];
    rb->tail = RingBuffer_Next(rb->tail, rb->size);
    rb->count--;
    return TRUE;
}

uint16_t RingBuffer_GetCount(const RingBuffer_t* rb) {
    return rb->count;
}

bool RingBuffer_IsFull_(const RingBuffer_t* rb) {
    return RingBuffer_IsFull(rb);
}

bool RingBuffer_IsEmpty_(const RingBuffer_t* rb) {
    return RingBuffer_IsEmpty(rb);
}

void RingBuffer_Clear(RingBuffer_t* rb) {
    rb->head = 0U;
    rb->tail = 0U;
    rb->count = 0U;
}

/* ==================== UART Buffer Access ==================== */

void UART_RingBuffer_Init(void) {
    RingBuffer_Init(&s_uart_tx, s_tx_buffer, RINGBUFFER_SIZE_256);
    RingBuffer_Init(&s_uart_rx, s_rx_buffer, RINGBUFFER_SIZE_256);
}

bool UART_RingBuffer_Push(uint8_t data) {
    return RingBuffer_Push(&s_uart_rx, data);
}

bool UART_RingBuffer_Pop(uint8_t* data) {
    return RingBuffer_Pop(&s_uart_rx, data);
}