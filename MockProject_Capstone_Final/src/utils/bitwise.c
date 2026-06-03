/**
 * @file bitwise.c
 * @brief Bit manipulation utilities - Topic 1 (Bitwise Operations)
 * @note MISRA compliant - no undefined behavior
 */
#include "types.h"

/* ==================== Bit Manipulation ==================== */

/**
 * @brief Set a specific bit (set bit n to 1)
 * @param value Target value
 * @param bit Bit position (0-31)
 * @return Modified value
 * @note Uses: value |= (1U << bit)
 */
static inline uint32_t Bitwise_SetBit(uint32_t value, uint8_t bit) {
    return value | (1U << bit);
}

/**
 * @brief Clear a specific bit (set bit n to 0)
 * @param value Target value
 * @param bit Bit position (0-31)
 * @return Modified value
 * @note Uses: value &= ~(1U << bit)
 */
static inline uint32_t Bitwise_ClearBit(uint32_t value, uint8_t bit) {
    return value & ~(1U << bit);
}

/**
 * @brief Toggle a specific bit (invert bit n)
 * @param value Target value
 * @param bit Bit position (0-31)
 * @return Modified value
 * @note Uses: value ^= (1U << bit)
 */
static inline uint32_t Bitwise_ToggleBit(uint32_t value, uint8_t bit) {
    return value ^ (1U << bit);
}

/**
 * @brief Check if a bit is set
 * @param value Target value
 * @param bit Bit position (0-31)
 * @return TRUE if bit is set, FALSE otherwise
 * @note Uses: (value >> bit) & 1U
 */
static inline bool Bitwise_IsBitSet(uint32_t value, uint8_t bit) {
    return ((value >> bit) & 1U) != 0U;
}

/* ==================== Bitfield Extraction ==================== */

/**
 * @brief Extract bitfield from value (MISRA compliant)
 * @param value Source value
 * @param offset Bit offset (0-31)
 * @param width Bit width (1-32)
 * @return Extracted bitfield
 * @note width must be <= 32 - offset
 */
static inline uint32_t Bitwise_Extract(uint32_t value, uint8_t offset, uint8_t width) {
    uint32_t mask = (1U << width) - 1U;
    return (value >> offset) & mask;
}

/**
 * @brief Insert bitfield into value (MISRA compliant)
 * @param value Source value
 * @param field Bitfield to insert
 * @param offset Bit offset (0-31)
 * @param width Bit width (1-32)
 * @return Modified value
 */
static inline uint32_t Bitwise_Insert(uint32_t value, uint32_t field, uint8_t offset, uint8_t width) {
    uint32_t mask = (1U << width) - 1U;
    return (value & ~(mask << offset)) | ((field & mask) << offset);
}

/* ==================== Byte Operations ==================== */

/**
 * @brief Swap byte order (Big-Endian ↔ Little-Endian) - Topic 1
 * @param value 32-bit value
 * @return Byte-swapped value
 * @note Uses: ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ...
 */
static inline uint32_t Bitwise_SwapBytes32(uint32_t value) {
    return ((value & 0x000000FFU) << 24U) |
           ((value & 0x0000FF00U) << 8U)  |
           ((value & 0x00FF0000U) >> 8U)  |
           ((value & 0xFF000000U) >> 24U);
}

/**
 * @brief Swap bytes of 16-bit value
 */
static inline uint16_t Bitwise_SwapBytes16(uint16_t value) {
    return (uint16_t)(((value & 0x00FFU) << 8U) | ((value & 0xFF00U) >> 8U));
}

/* ==================== CRC8 Calculation ==================== */

/**
 * @brief Calculate CRC8 (Dallas/Maxim 1-Wire) - Topic 1
 * @param data Pointer to data buffer
 * @param len Length of data
 * @return CRC8 value
 * @note Polynomial: 0x31 (x^8 + x^5 + x^4 + 1)
 */
uint8_t CRC8_Calculate(const uint8_t* data, uint16_t len) {
    uint8_t crc = 0U;
    uint16_t i;

    for (i = 0U; i < len; i++) {
        uint8_t j;
        crc ^= data[i];
        for (j = 0U; j < 8U; j++) {
            if ((crc & 0x80U) != 0U) {
                crc = (uint8_t)((crc << 1U) ^ 0x31U);
            } else {
                crc <<= 1U;
            }
        }
    }
    return crc;
}

/**
 * @brief Calculate CRC32 (IEEE 802.3) - Topic 1
 * @param data Pointer to data buffer
 * @param len Length of data
 * @return CRC32 value
 */
uint32_t CRC32_Calculate(const uint8_t* data, uint32_t len) {
    uint32_t crc = 0xFFFFFFFFU;
    uint32_t i;

    for (i = 0U; i < len; i++) {
        uint8_t j;
        crc ^= (uint32_t)data[i];
        for (j = 0U; j < 8U; j++) {
            if ((crc & 1U) != 0U) {
                crc = (crc >> 1U) ^ 0xEDB88320U;
            } else {
                crc >>= 1U;
            }
        }
    }
    return ~crc;
}

/* ==================== Fast Division/Multiplication ==================== */

/**
 * @brief Fast division by power of 2 using right shift
 * @note dividend / 2^n = dividend >> n
 */
static inline uint32_t FastDiv2n(uint32_t dividend, uint8_t n) {
    return dividend >> n;
}

/**
 * @brief Fast multiply by power of 2 using left shift
 * @note dividend * 2^n = dividend << n
 */
static inline uint32_t FastMul2n(uint32_t dividend, uint8_t n) {
    return dividend << n;
}

/**
 * @brief Fast modulo by power of 2 using bitwise AND
 * @note dividend % 2^n = dividend & (2^n - 1)
 */
static inline uint32_t FastMod2n(uint32_t dividend, uint32_t divisor) {
    return dividend & (divisor - 1U);
}