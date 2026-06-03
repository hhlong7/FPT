#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "platform_types.h"
#include "std_types.h"

#define RING_BUFFER_CAPACITY    (8U)
#define RING_BUFFER_ITEM_SIZE   (16U)

typedef struct {
    uint8_t data[RING_BUFFER_CAPACITY][RING_BUFFER_ITEM_SIZE];
    uint8_t lengths[RING_BUFFER_CAPACITY];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} RingBuffer_t;

void RingBuffer_Init(RingBuffer_t* rb);
Std_ReturnType RingBuffer_Push(RingBuffer_t* rb, const uint8_t* data, uint8_t length);
Std_ReturnType RingBuffer_Pop(RingBuffer_t* rb, uint8_t* data, uint8_t* length);
bool RingBuffer_IsEmpty(const RingBuffer_t* rb);
bool RingBuffer_IsFull(const RingBuffer_t* rb);

#endif
