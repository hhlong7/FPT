#include <cassert>
extern "C" {
#include "ring_buffer.h"
}

int main()
{
    RingBuffer_t rb;
    uint8_t tx[3] = {1U, 2U, 3U};
    uint8_t rx[16] = {0U};
    uint8_t length = 0U;

    RingBuffer_Init(&rb);
    assert(RingBuffer_Push(&rb, tx, 3U) == E_OK);
    assert(RingBuffer_Pop(&rb, rx, &length) == E_OK);
    assert(length == 3U);
    assert(rx[0] == 1U);
    assert(rx[1] == 2U);
    assert(rx[2] == 3U);

    return 0;
}
