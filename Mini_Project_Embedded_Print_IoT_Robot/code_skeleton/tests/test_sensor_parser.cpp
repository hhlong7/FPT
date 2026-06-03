#include <cassert>
extern "C" {
#include "sensor_parser.h"
}

int main()
{
    uint8_t raw[4] = {0x01U, 0xF4U, 0x00U, 0xFAU};
    SensorFrame_t frame{};

    assert(SensorParser_ParseTempHumidity(raw, 4U, &frame) == E_OK);
    assert(frame.humidity_deci_percent == 500U);
    assert(frame.temperature_deci_c == 250);

    assert(SensorParser_ParseTempHumidity(raw, 3U, &frame) == E_INVALID_PARAM);
    assert(SensorParser_ParseTempHumidity(0, 4U, &frame) == E_INVALID_PARAM);
    assert(SensorParser_ParseTempHumidity(raw, 4U, 0) == E_INVALID_PARAM);

    return 0;
}
