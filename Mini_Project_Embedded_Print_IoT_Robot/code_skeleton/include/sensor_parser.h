#ifndef SENSOR_PARSER_H
#define SENSOR_PARSER_H

#include "platform_types.h"
#include "std_types.h"

typedef struct {
    int16_t temperature_deci_c;
    uint16_t humidity_deci_percent;
} SensorFrame_t;

Std_ReturnType SensorParser_ParseTempHumidity(const uint8_t* data,
                                              uint8_t length,
                                              SensorFrame_t* out_frame);

#endif
