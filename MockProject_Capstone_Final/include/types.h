/**
 * @file types.h
 * @brief Common types and fixed-width integers
 * @note Tuân thủ MISRA C:2012 Rule 14.3 - No implicit conversions
 */
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* Boolean values - MISRA compliant */
#define TRUE    1U
#define FALSE   0U

/* NULL pointer - MISRA compliant */
#define NULL_PTR    ((void*)0)

/* Standard exit codes */
#define EXIT_SUCCESS    0U
#define EXIT_ERROR      1U

/* Common result type for error handling */
typedef enum {
    OK = 0U,
    ERROR = 1U,
    ERROR_TIMEOUT = 2U,
    ERROR_INVALID_PARAM = 3U,
    ERROR_BUSY = 4U,
    ERROR_OVERFLOW = 5U,
    ERROR_NOT_INITIALIZED = 6U
} Result_t;

/* Check Result macro - MISRA compliant */
#define IS_OK(res)      ((res) == OK)
#define IS_ERROR(res)   ((res) != OK)

#endif /* TYPES_H */