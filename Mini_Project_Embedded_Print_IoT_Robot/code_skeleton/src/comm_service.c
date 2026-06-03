#include "comm_service.h"

static RingBuffer_t g_comm_queue;
static bool g_comm_timeout;

void CommService_Init(void)
{
    RingBuffer_Init(&g_comm_queue);
    g_comm_timeout = false;
}

Std_ReturnType CommService_EnqueueFrame(const CommandFrame_t* frame)
{
    uint8_t payload[RING_BUFFER_ITEM_SIZE];
    uint8_t index;

    if ((frame == 0) || (frame->payload_length > 8U)) {
        return E_INVALID_PARAM;
    }

    payload[0] = frame->command_id;
    payload[1] = frame->payload_length;

    for (index = 0U; index < frame->payload_length; ++index) {
        payload[index + 2U] = frame->payload[index];
    }

    return RingBuffer_Push(&g_comm_queue, payload, (uint8_t)(frame->payload_length + 2U));
}

Std_ReturnType CommService_DequeueFrame(CommandFrame_t* frame)
{
    uint8_t payload[RING_BUFFER_ITEM_SIZE];
    uint8_t length;
    uint8_t index;
    Std_ReturnType status;

    if (frame == 0) {
        return E_INVALID_PARAM;
    }

    status = RingBuffer_Pop(&g_comm_queue, payload, &length);
    if (status != E_OK) {
        return status;
    }

    frame->command_id = payload[0];
    frame->payload_length = payload[1];

    for (index = 0U; index < frame->payload_length; ++index) {
        frame->payload[index] = payload[index + 2U];
    }

    return E_OK;
}

bool CommService_HasPendingFrame(void)
{
    return (!RingBuffer_IsEmpty(&g_comm_queue));
}

void CommService_ReportTimeout(void)
{
    g_comm_timeout = true;
}

bool CommService_HasTimeout(void)
{
    return g_comm_timeout;
}
