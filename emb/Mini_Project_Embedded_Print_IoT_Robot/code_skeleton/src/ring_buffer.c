#include "ring_buffer.h"

static bool RingBuffer_IsValidLength(uint8_t length)
{
    return ((length > 0U) && (length <= RING_BUFFER_ITEM_SIZE));
}

void RingBuffer_Init(RingBuffer_t* rb)
{
    uint8_t index;

    rb->head = 0U;
    rb->tail = 0U;
    rb->count = 0U;

    for (index = 0U; index < RING_BUFFER_CAPACITY; ++index) {
        rb->lengths[index] = 0U;
    }
}

Std_ReturnType RingBuffer_Push(RingBuffer_t* rb, const uint8_t* data, uint8_t length)
{
    uint8_t index;

    if ((rb == 0) || (data == 0) || (!RingBuffer_IsValidLength(length))) {
        return E_INVALID_PARAM;
    }

    if (rb->count >= RING_BUFFER_CAPACITY) {
        return E_BUFFER_FULL;
    }

    for (index = 0U; index < length; ++index) {
        rb->data[rb->head][index] = data[index];
    }

    rb->lengths[rb->head] = length;
    rb->head = (uint8_t)((rb->head + 1U) % RING_BUFFER_CAPACITY);
    rb->count++;

    return E_OK;
}

Std_ReturnType RingBuffer_Pop(RingBuffer_t* rb, uint8_t* data, uint8_t* length)
{
    uint8_t index;
    uint8_t stored_length;

    if ((rb == 0) || (data == 0) || (length == 0)) {
        return E_INVALID_PARAM;
    }

    if (rb->count == 0U) {
        return E_BUFFER_EMPTY;
    }

    stored_length = rb->lengths[rb->tail];

    for (index = 0U; index < stored_length; ++index) {
        data[index] = rb->data[rb->tail][index];
    }

    *length = stored_length;
    rb->lengths[rb->tail] = 0U;
    rb->tail = (uint8_t)((rb->tail + 1U) % RING_BUFFER_CAPACITY);
    rb->count--;

    return E_OK;
}

bool RingBuffer_IsEmpty(const RingBuffer_t* rb)
{
    return (rb->count == 0U);
}

bool RingBuffer_IsFull(const RingBuffer_t* rb)
{
    return (rb->count >= RING_BUFFER_CAPACITY);
}
