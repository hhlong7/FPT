/**
 * @file hal.h
 * @brief Hardware Abstraction Layer interface
 * @note Tuân thủ MISRA C:2012 - No heap allocation, static memory only
 */
#ifndef HAL_H
#define HAL_H

#include "types.h"

/* GPIO pin definitions */
typedef enum {
    PIN_0 = 0U,
    PIN_1 = 1U,
    PIN_2 = 2U,
    PIN_3 = 3U,
    PIN_4 = 4U,
    PIN_5 = 5U,
    PIN_6 = 6U,
    PIN_7 = 7U
} Pin_t;

typedef enum {
    PIN_MODE_INPUT = 0U,
    PIN_MODE_OUTPUT = 1U,
    PIN_MODE_ALTERNATE = 2U,
    PIN_MODE_ANALOG = 3U
} PinMode_t;

typedef enum {
    PIN_LOW = 0U,
    PIN_HIGH = 1U
} PinState_t;

/* UART configuration */
typedef struct {
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
} UART_Config_t;

/* Timer configuration */
typedef struct {
    uint16_t period_ms;
    uint16_t prescaler;
} Timer_Config_t;

/* ADC channel definitions */
typedef enum {
    ADC_CHANNEL_0 = 0U,
    ADC_CHANNEL_1 = 1U,
    ADC_CHANNEL_2 = 2U,
    ADC_CHANNEL_3 = 3U
} ADC_Channel_t;

/* ==================== GPIO HAL ==================== */
void HAL_GPIO_Init(Pin_t pin, PinMode_t mode);
void HAL_GPIO_WritePin(Pin_t pin, PinState_t state);
PinState_t HAL_GPIO_ReadPin(Pin_t pin);
void HAL_GPIO_TogglePin(Pin_t pin);

/* ==================== UART HAL ==================== */
void HAL_UART_Init(const UART_Config_t* config);
void HAL_UART_Transmit(const uint8_t* data, uint16_t length);
uint16_t HAL_UART_Receive(uint8_t* data, uint16_t max_length);
bool HAL_UART_IsDataAvailable(void);
void HAL_UART_Print(const char* str);  /* Debug print */

/* ==================== Timer HAL ==================== */
void HAL_Timer_Init(const Timer_Config_t* config);
void HAL_Timer_Start(void);
void HAL_Timer_Stop(void);
void HAL_Timer_Delay_ms(uint32_t ms);
uint32_t HAL_Timer_GetTick(void);

/* ==================== ADC HAL ==================== */
void HAL_ADC_Init(void);
uint16_t HAL_ADC_Read(ADC_Channel_t channel);
uint16_t HAL_ADC_ReadAverage(ADC_Channel_t channel, uint8_t samples);

/* ==================== I2C HAL ==================== */
void HAL_I2C_Init(void);
Result_t HAL_I2C_Write(uint8_t addr, const uint8_t* data, uint8_t len);
Result_t HAL_I2C_Read(uint8_t addr, uint8_t* data, uint8_t len);
Result_t HAL_I2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t value);
Result_t HAL_I2C_ReadReg(uint8_t addr, uint8_t reg, uint8_t* value);

/* ==================== Watchdog HAL ==================== */
void HAL_WDG_Init(uint32_t timeout_ms);
void HAL_WDG_Feed(void);

/* ==================== System HAL ==================== */
void HAL_SystemReset(void);
uint32_t HAL_GetResetCause(void);

#endif /* HAL_H */