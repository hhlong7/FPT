#include "log_formatter.h"
#include <stdio.h>

Std_ReturnType LogFormatter_FormatFaultEvent(uint32_t tick,
                                             FaultCode_t code,
                                             char* out_buffer,
                                             uint16_t buffer_size)
{
    int written;

    if ((out_buffer == 0) || (buffer_size == 0U)) {
        return E_INVALID_PARAM;
    }

    written = snprintf(out_buffer,
                       buffer_size,
                       "[tick=%lu] EVT=FAULT CODE=%u",
                       (unsigned long)tick,
                       (unsigned int)code);

    if ((written < 0) || ((uint16_t)written >= buffer_size)) {
        return E_NOT_OK;
    }

    return E_OK;
}
