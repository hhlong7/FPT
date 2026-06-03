#ifndef COMM_SERVICE_H
#define COMM_SERVICE_H

#include "app_types.h"
#include "std_types.h"
#include "ring_buffer.h"

void CommService_Init(void);
Std_ReturnType CommService_EnqueueFrame(const CommandFrame_t* frame);
Std_ReturnType CommService_DequeueFrame(CommandFrame_t* frame);
bool CommService_HasPendingFrame(void);
void CommService_ReportTimeout(void);
bool CommService_HasTimeout(void);

#endif
