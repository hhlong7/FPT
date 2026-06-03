#include <cassert>
#include <cstring>
extern "C" {
#include "log_formatter.h"
}

int main()
{
    char buffer[64] = {0};

    assert(LogFormatter_FormatFaultEvent(15240UL, FAULT_COMM_TIMEOUT, buffer, sizeof(buffer)) == E_OK);
    assert(std::strstr(buffer, "EVT=FAULT") != nullptr);
    assert(std::strstr(buffer, "CODE=") != nullptr);

    assert(LogFormatter_FormatFaultEvent(1UL, FAULT_NONE, 0, sizeof(buffer)) == E_INVALID_PARAM);
    assert(LogFormatter_FormatFaultEvent(1UL, FAULT_NONE, buffer, 0U) == E_INVALID_PARAM);
    assert(LogFormatter_FormatFaultEvent(1UL, FAULT_NONE, buffer, 8U) == E_NOT_OK);

    return 0;
}
