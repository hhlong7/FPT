/**
 * @file main.c
 * @brief Main entry point - MockProject Capstone
 * @note Topic 1-13 Integration: All concepts combined
 */
#include "types.h"
#include "config.h"
#include "hal.h"
#include "sensor.h"
#include "motor.h"
#include "command.h"
#include "statemachine.h"
#include "nvm.h"

/* External variables */
extern uint32_t _estack;       /* Defined in linker script */
extern uint32_t _sidata;      /* .data init values in Flash */
extern uint32_t _sdata;        /* .data start in RAM */
extern uint32_t _edata;        /* .data end in RAM */
extern uint32_t _sbss;         /* .bss start */
extern uint32_t _ebss;         /* .bss end */

/* ==================== Static Variables (Topic 2 - Memory Layout) ==================== */
static volatile uint32_t s_tick_count = 0U;  /* volatile for ISR access */
static SensorData_t s_sensor_data;            /* Sensor fusion data */
static bool s_system_ready = FALSE;

/* ==================== Main Entry Point ==================== */
int main(void) {
    /* 1. Copy .data init values from Flash to RAM */
    uint32_t* src = &_sidata;
    uint32_t* dest = &_sdata;
    while (dest < &_edata) {
        *dest++ = *src++;
    }

    /* 2. Zero-out .bss section */
    dest = &_sbss;
    while (dest < &_ebss) {
        *dest++ = 0U;
    }

    /* 3. Call C++ static constructors (if any) */
    /* __libc_init_array(); */

    /* 4. Initialize system clock */
    SystemClock_Init();

    /* 5. Initialize watchdog */
    HAL_WDG_Init(WATCHDOG_TIMEOUT_MS);

    /* 6. Initialize UART for debug output */
    UART_Config_t uart_cfg = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = 8U,
        .stop_bits = 1U,
        .parity = 0U
    };
    HAL_UART_Init(&uart_cfg);
    HAL_UART_Print("=== MockProject Capstone Starting ===\r\n");

    /* 7. Initialize sensors */
    Sensor_InitAll();
    HAL_UART_Print("Sensors initialized\r\n");

    /* 8. Initialize motors */
    Motor_Init();
    HAL_UART_Print("Motors initialized\r\n");

    /* 9. Initialize command queue */
    Command_Init();
    HAL_UART_Print("Command queue initialized\r\n");

    /* 10. Initialize state machine */
    StateMachine_Init();
    HAL_UART_Print("State machine initialized\r\n");

    /* 11. Initialize NVM */
    NVM_Init();
    FaultLog_Init();
    HAL_UART_Print("NVM initialized\r\n");

    /* 12. Initialize timer for system tick */
    Timer_Config_t timer_cfg = {
        .period_ms = 1U,  /* 1ms tick */
        .prescaler = (uint16_t)(AHB_CLOCK_HZ / 1000000UL) /* 1us resolution */
    };
    HAL_Timer_Init(&timer_cfg);
    HAL_Timer_Start();

    s_system_ready = TRUE;
    HAL_UART_Print("=== System Ready ===\r\n");

    /* ==================== Main Loop (Super-loop / Event-driven) ==================== */
    while (1) {
        /* Pet watchdog - Topic 2 */
        HAL_WDG_Feed();

        /* Process sensor data every 100ms - Topic 1 */
        if ((s_tick_count % SENSOR_SAMPLE_RATE_MS) == 0U) {
            if (IS_OK(Sensor_ReadAll(&s_sensor_data))) {
                /* Check for sensor faults - Topic 12 */
                if (s_sensor_data.temperature_celsius > TEMP_CRITICAL_Celsius) {
                    StateMachine_Process(EVENT_FAULT);
                }
            }
        }

        /* Process command queue - Topic 9 (Ring Buffer) */
        Command_t cmd;
        while (!Command_IsEmpty()) {
            if (IS_OK(Command_Dequeue(&cmd))) {
                StateMachine_Process(EVENT_CMD_RECEIVED);
                CommandHandler_Execute(&cmd);
            }
        }

        /* Update state machine - Topic 13 (State Pattern) */
        Event_t event = EVENT_NONE;
        if ((s_tick_count % 10U) == 0U) {
            event = EVENT_TIMER_10MS;
        }
        if ((s_tick_count % 100U) == 0U) {
            event = EVENT_TIMER_100MS;
        }
        if (event != EVENT_NONE) {
            StateMachine_Process(event);
        }

        /* Update motors - Topic 13 (Command Pattern) */
        Motor_Update();

        /* Delay to control loop frequency */
        HAL_Timer_Delay_ms(1U);
    }

    /* Never reached */
    return EXIT_SUCCESS;
}

/* ==================== SysTick Handler (ISR) ==================== */
void SysTick_Handler(void) {
    s_tick_count++;
}

/* ==================== HardFault Handler ==================== */
void HardFault_Handler(void) {
    /* Log fault and reset */
    FaultLog_Add(0x01U, 3U, 0U);  /* Hard fault, critical */
    HAL_SystemReset();
}