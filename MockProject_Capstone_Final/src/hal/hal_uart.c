/**
 * @file hal_uart.c
 * @brief UART HAL implementation - Topic 5 (File I/O & Debugging)
 * @note Simulated HAL for embedded targets
 */
#include "types.h"
#include "config.h"
#include "hal.h"
#include "ringbuffer.h"

/* UART registers (simulated) */
typedef struct {
    volatile uint32_t SR;    /* Status Register */
    volatile uint32_t DR;    /* Data Register */
    volatile uint32_t BRR;   /* Baud Rate Register */
    volatile uint32_t CR1;   /* Control Register 1 */
} UART_Reg_t;

#define UART1_BASE     ((UART_Reg_t*)0x40013800U)
#define UART1          ((UART_Reg_t*)UART1_BASE)

/* Status flags */
#define UART_SR_TXE    (1U << 7)   /* Transmit data register empty */
#define UART_SR_RXNE   (1U << 5)   /* Read data register not empty */

/* ==================== UART Configuration ==================== */

void HAL_UART_Init(const UART_Config_t* config) {
    if (config == NULL_PTR) {
        return;
    }

    /* Configure baud rate */
    uint32_t brr = (APB1_CLOCK_HZ / config->baud_rate);
    UART1->BRR = brr;

    /* Enable UART, TX, RX */
    UART1->CR1 = (1U << 13) | (1U << 3) | (1U << 2);

    /* Initialize ring buffers */
    UART_RingBuffer_Init();
}

void HAL_UART_Transmit(const uint8_t* data, uint16_t length) {
    if (data == NULL_PTR) {
        return;
    }

    uint16_t i;
    for (i = 0U; i < length; i++) {
        /* Wait for TX buffer empty */
        while ((UART1->SR & UART_SR_TXE) == 0U) {
            /* Spin lock */
        }
        UART1->DR = data[i];
    }
}

uint16_t HAL_UART_Receive(uint8_t* data, uint16_t max_length) {
    if (data == NULL_PTR) {
        return 0U;
    }

    uint16_t count = 0U;
    while ((count < max_length) && UART_RingBuffer_Pop(&data[count])) {
        count++;
    }
    return count;
}

bool HAL_UART_IsDataAvailable(void) {
    return !RingBuffer_IsEmpty_(NULL);  /* Would need actual ring buffer */
}

void HAL_UART_Print(const char* str) {
    if (str == NULL_PTR) {
        return;
    }

    /* Transmit string character by character */
    uint16_t i = 0U;
    while (str[i] != '\0') {
        /* Wait for TX buffer empty */
        while ((UART1->SR & UART_SR_TXE) == 0U) {
            /* Spin lock */
        }
        UART1->DR = (uint8_t)str[i];
        i++;
    }
}

/* ==================== Simulated ISR Handler ==================== */

void UART1_IRQHandler(void) {
    /* Check if RXNE flag is set */
    if ((UART1->SR & UART_SR_RXNE) != 0U) {
        /* Read data and push to ring buffer */
        uint8_t data = (uint8_t)(UART1->DR & 0xFFU);
        UART_RingBuffer_Push(data);
    }
}