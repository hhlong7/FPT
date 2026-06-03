#include "command_parser.h"

Std_ReturnType CommandParser_Parse(const uint8_t* data,
                                   uint8_t length,
                                   CommandFrame_t* out_frame)
{
    uint8_t index;
    uint8_t payload_length;

    if ((data == 0) || (out_frame == 0) || (length < 2U)) {
        return E_INVALID_PARAM;
    }

    payload_length = data[1];
    if ((payload_length > 8U) || ((uint8_t)(payload_length + 2U) > length)) {
        return E_INVALID_PARAM;
    }

    out_frame->command_id = data[0];
    out_frame->payload_length = payload_length;

    for (index = 0U; index < payload_length; ++index) {
        out_frame->payload[index] = data[index + 2U];
    }

    return E_OK;
}
