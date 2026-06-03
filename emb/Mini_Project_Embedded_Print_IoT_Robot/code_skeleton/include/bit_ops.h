#ifndef BIT_OPS_H
#define BIT_OPS_H

#include "platform_types.h"

#define BIT_MASK(pos)              ((uint32_t)1UL << (pos))
#define BIT_SET(value, pos)        ((value) |= BIT_MASK(pos))
#define BIT_CLEAR(value, pos)      ((value) &= (uint32_t)(~BIT_MASK(pos)))
#define BIT_TOGGLE(value, pos)     ((value) ^= BIT_MASK(pos))
#define BIT_IS_SET(value, pos)     ((((value) & BIT_MASK(pos)) != 0U) ? true : false)

#define JOIN_U8_TO_U16(msb, lsb)   ((uint16_t)((((uint16_t)(msb)) << 8U) | ((uint16_t)(lsb))))
#define HIGH_BYTE_U16(value)       ((uint8_t)(((uint16_t)(value) >> 8U) & 0xFFU))
#define LOW_BYTE_U16(value)        ((uint8_t)((uint16_t)(value) & 0xFFU))

static inline uint32_t BitOps_ExtractFieldU32(uint32_t value, uint8_t shift, uint32_t mask)
{
    return ((value >> shift) & mask);
}

static inline uint32_t BitOps_InsertFieldU32(uint32_t base, uint32_t field, uint8_t shift, uint32_t mask)
{
    uint32_t cleared;

    cleared = base & (~(mask << shift));
    return (cleared | ((field & mask) << shift));
}

#endif
