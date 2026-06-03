#include <cassert>
extern "C" {
#include "command_parser.h"
}

int main()
{
    uint8_t raw[5] = {0x21U, 0x03U, 0x11U, 0x22U, 0x33U};
    uint8_t invalid_raw[3] = {0x21U, 0x08U, 0x11U};
    CommandFrame_t frame{};

    assert(CommandParser_Parse(raw, 5U, &frame) == E_OK);
    assert(frame.command_id == 0x21U);
    assert(frame.payload_length == 3U);
    assert(frame.payload[0] == 0x11U);
    assert(frame.payload[1] == 0x22U);
    assert(frame.payload[2] == 0x33U);

    assert(CommandParser_Parse(invalid_raw, 3U, &frame) == E_INVALID_PARAM);
    assert(CommandParser_Parse(raw, 1U, &frame) == E_INVALID_PARAM);
    assert(CommandParser_Parse(0, 5U, &frame) == E_INVALID_PARAM);
    assert(CommandParser_Parse(raw, 5U, 0) == E_INVALID_PARAM);

    return 0;
}
