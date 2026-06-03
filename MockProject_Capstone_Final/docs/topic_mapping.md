# Topic Mapping - Code Implementation

This document maps each topic concept to specific files and line numbers.

## Topic 1: Variables & Data Types

| Concept | Implementation | Location |
|---------|----------------|----------|
| Fixed-width integers (`uint8_t`, `uint32_t`) | `include/types.h` | Line 4-5 |
| `volatile` for ISR variables | `src/main.c` | Line 18: `s_tick_count` |
| `const` for ROM data | `include/config.h` | Line 7: `SYSTEM_CLOCK_HZ` |
| Integer promotion handling | `src/utils/bitwise.c` | Line 24: explicit casts |
| Endianness handling | `src/utils/bitwise.c` | Line 58-66: `Bitwise_SwapBytes32()` |

## Topic 2: Memory Layout & Compile

| Concept | Implementation | Location |
|---------|----------------|----------|
| `.data` section initialization | `src/main.c` | Line 26-31 |
| `.bss` zero-out | `src/main.c` | Line 33-36 |
| Static variables | `src/core/command.c` | Line 20: `s_cmd_queue` |
| Stack usage tracking | `include/config.h` | Line 31: `CMD_QUEUE_SIZE` |
| Memory budget | `README.md` | Section 3 |

## Topic 3: Arrays, Pointers & References

| Concept | Implementation | Location |
|---------|----------------|----------|
| Ring buffer (array + indices) | `src/utils/ringbuffer.c` | Line 16-21 |
| Pointer arithmetic | `src/core/command.c` | Line 61-68: checksum calc |
| Buffer overflow protection | `src/core/command.c` | Line 45-49 |
| Pointer to const data | `include/hal.h` | Line 59: `const UART_Config_t*` |

## Topic 4: Functions & Parameter Passing

| Concept | Implementation | Location |
|---------|----------------|----------|
| Pass by pointer (efficient) | `src/core/nvm.c` | Line 88: `NVM_ReadCalibration(CalibrationData_t*)` |
| Reentrant functions | `src/utils/ringbuffer.c` | All functions reentrant |
| Function prototypes | `include/hal.h` | Lines 26-58 |
| Static functions | `src/utils/ringbuffer.c` | Line 30: `RingBuffer_Next()` |

## Topic 5: File I/O & Debugging

| Concept | Implementation | Location |
|---------|----------------|----------|
| Debug logging | `include/config.h` | Line 47-50: `DEBUG_LOG` macro |
| UART output | `include/hal.h` | Line 36: `HAL_UART_Print()` |
| NVM storage (fault logs) | `src/core/nvm.c` | Line 102-127: FaultLog |
| Flash emulation | `src/core/nvm.c` | Line 10-21: Static structs |

## Topic 6: OOP Basics

| Concept | Implementation | Location |
|---------|----------------|----------|
| Encapsulation (struct + functions) | `include/sensor.h` | `SensorData_t` with getter functions |
| RAII concept | `include/nvm.h` | `CalibrationData_t` - self-contained |
| `typedef` for opaque types | `include/types.h` | `Result_t`, `bool` |

## Topic 7: OOP Features

| Concept | Implementation | Location |
|---------|----------------|----------|
| Virtual function table | `include/sensor.h` | Lines 27-33: `ISensor_t` with function pointers |
| Inheritance (composition) | `include/sensor.h` | `ITempSensor_t` extends concept |
| Polymorphism | `src/main.c` | Line 60: `Sensor_ReadAll()` works with any sensor |
| Pure virtual functions | `include/sensor.h` | Line 29: `SensorReadFunc_t` |

## Topic 8: Namespace & Templates

| Concept | Implementation | Location |
|---------|----------------|----------|
| `static_assert` compile-time check | `src/utils/ringbuffer.c` | Line 24-26 |
| Structured types | `include/types.h` | `Result_t` enum |
| Generic containers | `include/command.h` | `Command_t` - generic command structure |

## Topic 9: Data Structures & Algorithms

| Concept | Implementation | Location |
|---------|----------------|----------|
| Ring buffer (O(1) enqueue/dequeue) | `src/utils/ringbuffer.c` | Full implementation |
| Command queue | `src/core/command.c` | Lines 24-49 |
| Binary search readiness | `include/nvm.h` | `CalibrationData_t` sorted array ready |
| Memory pool pattern | `include/nvm.h` | `FaultLog_t` uses fixed-size array |

## Topic 10: Unit Testing

| Concept | Implementation | Location |
|---------|----------------|----------|
| Test isolation | `tests/test_ringbuffer.c` | Self-contained test buffer |
| Mock functions | `tests/test_statemachine.c` | Mock `HAL_Timer_GetTick()` |
| Test assertions | `tests/test_ringbuffer.c` | Line 19-26: `TEST_ASSERT` macro |
| Test coverage tracking | `tests/test_ringbuffer.c` | Lines 12-14: `g_tests_*` counters |

## Topic 11: Advanced C++ Features

| Concept | Implementation | Location |
|---------|----------------|----------|
| `const` correctness | `include/hal.h` | Line 59: `const UART_Config_t*` |
| Typed enums | `include/types.h` | `Result_t`, `State_t`, `Event_t` |
| Structured bindings concept | `src/core/nvm.c` | Line 87: Multiple field struct |
| Volatile for hardware registers | `include/hal.h` | Line 22: `volatile uint32_t` |

## Topic 12: Optimization & Common Defects

| Concept | Implementation | Location |
|---------|----------------|----------|
| Struct packing (`__attribute__((packed))`) | `include/nvm.h` | Line 29 |
| Bitwise AND for modulo | `src/utils/ringbuffer.c` | Line 39: `(idx + 1) & (size - 1)` |
| Division by power of 2 | `src/utils/bitwise.c` | Line 85: Right shift |
| `volatile` for shared data | `src/main.c` | Line 18 |
| No heap allocation | Entire project | No `malloc`/`free` anywhere |

## Topic 13: Design Patterns

| Concept | Implementation | Location |
|---------|----------------|----------|
| State machine (function pointer array) | `src/core/statemachine.c` | Lines 15-26 |
| Command pattern | `include/motor.h` | `MotorCommand_t` structure |
| Observer pattern (callback) | `include/sensor.h` | `SensorReadFunc_t` callback |
| HAL/Strategy pattern | `include/hal.h` | Abstract hardware interface |
| Ring buffer (producer-consumer) | `src/utils/ringbuffer.c` | Separated head/tail indices |

---

## File Summary

| File | Topics Covered |
|------|---------------|
| `include/types.h` | 1, 6, 8, 11 |
| `include/config.h` | 1, 2, 11, 12 |
| `include/hal.h` | 4, 6, 7, 11 |
| `include/sensor.h` | 3, 6, 7, 9, 13 |
| `include/motor.h` | 3, 9, 13 |
| `include/command.h` | 3, 6, 8, 9, 13 |
| `include/statemachine.h` | 7, 13 |
| `include/nvm.h` | 2, 6, 9, 10, 12 |
| `src/main.c` | 1, 2, 4, 6, 7, 9, 12, 13 |
| `src/core/statemachine.c` | 9, 13 |
| `src/core/command.c` | 3, 9, 12, 13 |
| `src/core/nvm.c` | 1, 2, 10, 12 |
| `src/utils/ringbuffer.c` | 3, 8, 9, 12 |
| `src/utils/bitwise.c` | 1, 12 |
| `tests/test_ringbuffer.c` | 10 |
| `tests/test_statemachine.c` | 10, 13 |