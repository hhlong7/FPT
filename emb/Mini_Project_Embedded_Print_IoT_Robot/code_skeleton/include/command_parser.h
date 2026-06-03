#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "app_types.h"
#include "std_types.h"

Std_ReturnType CommandParser_Parse(const uint8_t* data,
                                   uint8_t length,
                                   CommandFrame_t* out_frame);

#endif
