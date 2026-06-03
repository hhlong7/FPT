#include "sensor_parser.h"
#include "bit_ops.h"

Std_ReturnType SensorParser_ParseTempHumidity(const uint8_t* data,
                                              uint8_t length,
                                              SensorFrame_t* out_frame)
{
    if ((data == 0) || (out_frame == 0) || (length < 4U)) {
        return E_INVALID_PARAM;
    }

    out_frame->humidity_deci_percent = JOIN_U8_TO_U16(data[0], data[1]);
    out_frame->temperature_deci_c = (int16_t)JOIN_U8_TO_U16(data[2], data[3]);

    return E_OK;
}
