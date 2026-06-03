# File I/O & Debugging - 10 Case Studies

## Case 1: The Blocking `printf` Bug
**Context**: Printing log messages inside a UART Receive ISR.
**Bad Practice**:
```c
void UART_ISR() {
    uint8_t data = UDR0;
    printf("Received: %c\n", data); // Blocks for milliseconds!
}
```
**Explanation**: `printf` over UART blocks the CPU. While printing, the hardware receives 5 more bytes, but the ISR is still stuck printing the first one. The UART hardware buffer overflows, and data is lost.
**Good Practice**: Push the received byte into a Ring Buffer, and set a flag. Let the Main Loop or a low-priority Task do the printing.

## Case 2: Writing Logs to SD Card
**Context**: An IoT node logging temperature every 5 seconds to an SD Card using FatFs.
**Practice**: 
```c
f_open(&file, "log.csv", FA_OPEN_APPEND | FA_WRITE);
f_printf(&file, "%lu,%f\n", millis(), temp);
f_sync(&file); // Crucial! Flushes the internal buffer to physical flash.
f_close(&file);
```
**Explanation**: If power is lost before `f_sync` or `f_close`, the data written to the file will be permanently lost because it was only sitting in the MCU's RAM buffer.

## Case 3: Retargeting C++ `std::cout`
**Context**: Using modern C++ on an ARM Cortex-M.
**Practice**: Just like `printf` requires rewriting `_write`, `std::cout` requires it too. However, pulling in `<iostream>` on an MCU often adds 100KB+ to the binary and uses dynamic memory, which violates MISRA and exhausts Flash.
**Rule**: Avoid `<iostream>` in embedded systems. Use lightweight C-style formatting or specialized embedded C++ loggers (e.g., `fmt` library tailored for no-allocation).

## Case 4: Debugging a Hard Fault
**Context**: The MCU suddenly freezes or resets.
**Practice**: When an invalid memory address is accessed (e.g., dereferencing a NULL pointer), an ARM Cortex-M CPU jumps to the `HardFault_Handler`. 
**Debugging Step**: Connect a JTAG/SWD debugger. Place a breakpoint inside the `HardFault_Handler`. When the CPU stops there, look at the Call Stack window in your IDE. It will show you exactly which function caused the crash.

## Case 5: GPIO Profiling (Timing Code Execution)
**Context**: You need to know exactly how long an image processing algorithm takes on a printer.
**Practice**:
```c
void process_image() {
    GPIO_SetPin(PORTA, PIN1);   // Pin goes HIGH
    run_heavy_algorithm();
    GPIO_ClearPin(PORTA, PIN1); // Pin goes LOW
}
```
**Debugging Step**: Hook an oscilloscope to PA1. Measure the width of the pulse. This is 100% accurate and has almost zero overhead, unlike using timers and printing the result.

## Case 6: Watchdogs Triggered by I/O
**Context**: Erasing a sector of SPI Flash memory.
**Problem**: The system resets randomly.
**Explanation**: Erasing flash can take 500ms. If the Hardware Watchdog Timer (WDT) is set to timeout in 300ms, the CPU will be reset while waiting for the flash to erase.
**Fix**: Temporarily "kick/pet" the watchdog during the long I/O operation, or split the erase into smaller non-blocking chunks if the hardware supports it.

## Case 7: The "Heisenbug" (Optimization Issues)
**Context**: Code works perfectly in Debug build (`-O0`) but crashes in Release build (`-O2` or `-Os`).
**Explanation**: Often caused by missing `volatile` keywords on hardware registers or ISR-shared variables. In `-O0`, the compiler reads RAM every time. In `-O2`, it caches the variable in a CPU register, missing updates from the hardware. 
**Fix**: Add `volatile` to shared flags and hardware addresses.

## Case 8: Asserts in Production
**Context**: Releasing the final firmware for a medical device.
**Practice**: 
```c
#ifdef NDEBUG
  #define assert(expr) ((void)0) // Asserts are removed in Release
#else
  #define assert(expr) if(!(expr)) { halt_system(__FILE__, __LINE__); }
#endif
```
**Explanation**: Asserts are for finding bugs during development. In production (Release build), asserts are usually compiled out (`NDEBUG`) to save space and prevent intentional halting, replacing them with graceful error recovery mechanisms.

## Case 9: Using CoreSight ITM/SWO for Logging
**Context**: UART logging is too slow.
**Practice**: ARM Cortex-M processors have a feature called SWO (Serial Wire Output) or ITM (Instrumentation Trace Macrocell). It allows you to send `printf` style messages directly through the SWD debugging cable at speeds of megabytes per second, without wasting a UART peripheral.

## Case 10: Non-Volatile Memory (NVM/EEPROM) Wear Leveling
**Context**: Saving a print counter every time a page is printed.
**Bad Practice**: Writing directly to the same EEPROM address. EEPROM/Flash has a limited lifespan (e.g., 100,000 writes).
**Good Practice**: Use a Wear-Leveling algorithm (or a lightweight filesystem like LittleFS) that spreads the writes across the entire flash sector, extending the memory's lifespan from months to decades.