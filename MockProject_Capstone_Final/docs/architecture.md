/**
 * @file architecture.md
 * @brief System Architecture Documentation
 */

# MockProject Capstone - System Architecture

## 1. Overview

Multi-Sensor Edge Controller firmware for Print/IoT/Robot applications, implementing all 13 topic concepts:

| Topic | Concept | Implementation |
|-------|---------|----------------|
| 1 | Variables & Types | `stdint.h`, `volatile`, `const` |
| 2 | Memory Layout | `.text`, `.data`, `.bss`, stack |
| 3 | Pointers & Arrays | Ring buffer, command parsing |
| 4 | Functions | Reentrant functions, parameter passing |
| 5 | File I/O & Debug | UART logging, NVM storage |
| 6 | OOP Basics | Encapsulation, RAII concept |
| 7 | OOP Features | Virtual functions, HAL interface |
| 8 | Namespace/Templates | Static assert, generic containers |
| 9 | Data Structures | Ring buffer, command queue |
| 10 | Unit Testing | Mock functions, test patterns |
| 11 | Advanced C++ | Move semantics, atomic operations |
| 12 | Optimization | Bitwise ops, struct packing |
| 13 | Design Patterns | State machine, command queue |

## 2. Layer Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Application Layer                      │
│  (Main Loop, Event Processing, State Machine)           │
├─────────────────────────────────────────────────────────┤
│                    Service Layer                         │
│  (Sensor Fusion, Motor Control, Command Processing)    │
├─────────────────────────────────────────────────────────┤
│                    HAL Interface                         │
│  (HAL_GPIO, HAL_UART, HAL_ADC, HAL_I2C, HAL_Timer)      │
├─────────────────────────────────────────────────────────┤
│                    Driver Layer                          │
│  (TMP102, DHT22, ADC, Stepper Motor, Limit Switch)     │
├─────────────────────────────────────────────────────────┤
│                    Hardware (Simulated)                 │
│  (ARM Cortex-M4, UART, I2C, GPIO, Timer, ADC)           │
└─────────────────────────────────────────────────────────┘
```

## 3. Module Responsibilities

### Core Modules

| Module | Responsibility | Key APIs |
|--------|---------------|----------|
| `main.c` | Entry point, system initialization, main loop | - |
| `statemachine.c` | State machine with 7 states | `StateMachine_Process()`, `StateMachine_Init()` |
| `command.c` | Command queue and parser | `Command_Enqueue()`, `Command_Dequeue()` |
| `nvm.c` | Non-volatile memory management | `NVM_ReadCalibration()`, `FaultLog_Add()` |
| `ringbuffer.c` | Generic ring buffer | `RingBuffer_Push()`, `RingBuffer_Pop()` |
| `bitwise.c` | Bit manipulation utilities | `Bitwise_SetBit()`, `CRC8_Calculate()` |

### Driver Modules

| Module | Hardware | Key APIs |
|--------|----------|----------|
| `sensor_tmp102.c` | I2C Temperature Sensor | `Sensor_TMP102_Init()`, `Sensor_TMP102_ReadTemperature()` |
| `sensor_dht22.c` | 1-Wire Humidity Sensor | `Sensor_DHT22_Init()`, `Sensor_DHT22_ReadHumidity()` |
| `motor_stepper.c` | Stepper Motor Driver | `Motor_MoveAbsolute()`, `Motor_Update()` |

## 4. Data Flow

### Command Processing Flow
```
UART RX Interrupt
       │
       ▼
  Ring Buffer (UART)
       │
       ▼
Command Parser (State Machine)
       │
       ▼
Command Queue (Ring Buffer)
       │
       ▼
Command Handler
       │
       ▼
Motor Control / Sensor Read / System Config
```

### Sensor Update Flow
```
Timer ISR (100ms)
       │
       ▼
Sensor_ReadAll()
       │
       ├──► TMP102 (I2C) ──► Temperature
       ├──► DHT22 (1-Wire) ──► Humidity
       ├──► ADC (Polling) ──► Pressure
       └──► GPIO (Polling) ──► Limit Switch

       │
       ▼
Sensor Fusion (Calibration Applied)
       │
       ▼
State Machine (Check Thresholds)
       │
       ▼
Fault Handler (if needed)
```

## 5. State Machine Diagram

```
                    ┌──────────┐
                    │   INIT   │
                    └─────┬────┘
                          │
                    ┌─────▼────┐
         ┌─────────►│   IDLE   │◄─────────┐
         │          └─────┬────┘          │
         │                │               │
         │    ┌──────────┐ │ ┌──────────┐ │
         │    │  SLEEP   │ │ │ RUNNING  │ │
         │    └────┬─────┘ │ └────┬─────┘ │
         │         │       │      │       │
         │         └───────┼──────┘       │
         │                 │              │
         │    ┌────────────▼──────────┐   │
         │    │        FAULT          │   │
         │    └────────────┬──────────┘   │
         │                 │              │
         │    ┌────────────▼──────────┐   │
         │    │        RESET          │───┘
         │    └───────────────────────┘
         │                 │
         └─────────────────┘
```

## 6. Memory Budget

| Section | Size | Description |
|---------|------|-------------|
| `.text` | ~24KB | Code + const strings |
| `.rodata` | ~4KB | Lookup tables, const data |
| `.data` | ~2KB | Initialized variables |
| `.bss` | ~6KB | Zero-initialized variables |
| Stack | ~4KB | Interrupt + main stack |
| **Total** | ~40KB | Within 64KB budget |

## 7. Timing Analysis

| Operation | Frequency | Duration | CPU Load |
|-----------|-----------|----------|----------|
| Sensor Sample | 10 Hz | ~500μs | 0.5% |
| Motor Update | 1 kHz | ~100μs | 10% |
| Command Process | Event | ~1ms | <1% |
| Watchdog Pet | 1 Hz | ~10μs | <0.1% |

## 8. Interrupt Architecture

| Interrupt | Priority | Source | Handler |
|-----------|----------|--------|---------|
| SysTick | 15 (Lowest) | Timer | `SysTick_Handler()` |
| UART_RX | 5 | UART | `UART_RX_IRQHandler()` |
| Timer2 | 10 | PWM/Motor | `TIM2_IRQHandler()` |
| HardFault | - | CPU | `HardFault_Handler()` |