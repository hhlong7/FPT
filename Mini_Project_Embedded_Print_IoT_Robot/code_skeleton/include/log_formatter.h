#ifndef LOG_FORMATTER_H
#define LOG_FORMATTER_H

#include "app_types.h"
#include "std_types.h"

Std_ReturnType LogFormatter_FormatFaultEvent(uint32_t tick,
                                             FaultCode_t code,
                                             char* out_buffer,
                                             uint16_t buffer_size);

#endif
