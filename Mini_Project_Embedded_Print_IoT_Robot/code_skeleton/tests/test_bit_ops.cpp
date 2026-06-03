#include <cassert>
extern "C" {
#include "bit_ops.h"
}

int main()
{
    uint32_t value = 0U;
    uint16_t joined;
    uint8_t high;
    uint8_t low;
    uint32_t field_value;

    BIT_SET(value, 3U);
    assert(BIT_IS_SET(value, 3U) == true);

    BIT_CLEAR(value, 3U);
    assert(BIT_IS_SET(value, 3U) == false);

    BIT_TOGGLE(value, 1U);
    assert(BIT_IS_SET(value, 1U) == true);
    BIT_TOGGLE(value, 1U);
    assert(BIT_IS_SET(value, 1U) == false);

    joined = JOIN_U8_TO_U16(0x12U, 0x34U);
    assert(joined == 0x1234U);

    high = HIGH_BYTE_U16(joined);
    low = LOW_BYTE_U16(joined);
    assert(high == 0x12U);
    assert(low == 0x34U);

    field_value = BitOps_ExtractFieldU32(0x000001ACUL, 2U, 0x03U);
    assert(field_value == 0x03U);

    value = BitOps_InsertFieldU32(0x00000000UL, 0x02U, 4U, 0x03U);
    assert(value == 0x20UL);

    return 0;
}
