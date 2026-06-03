#ifndef STD_TYPES_H
#define STD_TYPES_H

#include "platform_types.h"

typedef uint8_t Std_ReturnType;

#define E_OK              ((Std_ReturnType)0U)
#define E_NOT_OK          ((Std_ReturnType)1U)
#define E_TIMEOUT         ((Std_ReturnType)2U)
#define E_INVALID_PARAM   ((Std_ReturnType)3U)
#define E_BUFFER_FULL     ((Std_ReturnType)4U)
#define E_BUFFER_EMPTY    ((Std_ReturnType)5U)
#define E_CRC_FAIL        ((Std_ReturnType)6U)
#define E_BUSY            ((Std_ReturnType)7U)

#endif
