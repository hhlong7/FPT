# Variables & Data Types - 10 Case Studies for Embedded / IoT / Robotics

## Case 1: Motor Control Speed Variable
**Context**: A robot wheel motor speed is controlled by a PWM signal (0-100%).
**Bad Practice**: `int speed;`
**Good Practice**: `uint8_t pwm_duty_cycle = 0U;`
**Explanation**: PWM percentage never exceeds 100, and cannot be negative. Using `uint8_t` saves RAM and communicates intent.

## Case 2: Reading a Hardware Switch (Debouncing)
**Context**: A mechanical limit switch on an industrial printer.
**Bad Practice**: `bool switch_state = READ_PIN();`
**Good Practice**: `volatile bool is_limit_switch_pressed = false;` (If updated via Interrupt).
**Explanation**: If updated in an ISR, the main loop must see the change immediately. `volatile` prevents the compiler from caching the value in a CPU register.

## Case 3: Sensor Data Buffer (Memory Optimization)
**Context**: An IoT node collecting temperature readings every second for an hour.
**Bad Practice**: `double temp_log[3600];` (Consumes 28.8 KB RAM!).
**Good Practice**: `int16_t temp_log_x100[3600];` (Consumes 7.2 KB RAM).
**Explanation**: Store 25.43°C as `2543` using a 16-bit integer. Floating-point math is slow on MCUs without an FPU and consumes 2-4x more RAM.

## Case 4: Storing Font Data for LCD Printer Display
**Context**: A 3D printer needs to display characters on an OLED screen.
**Bad Practice**: `uint8_t font_arial[1024] = {...};`
**Good Practice**: `const uint8_t font_arial[1024] = {...};`
**Explanation**: Without `const`, the 1KB array is copied from Flash to RAM at boot. With `const`, the CPU reads directly from Flash, saving precious RAM.

## Case 5: Handling Time / Uptime
**Context**: An IoT gateway tracking time since boot (millis).
**Bad Practice**: `int uptime = millis();` (Overflows in ~24 days).
**Good Practice**: `uint32_t uptime_ms = millis();` (Overflows in ~49 days).
**Explanation**: Standardizes on 32-bit unsigned. Always use `uint32_t` for `millis()` and handle rollover explicitly using subtraction: `if(current_ms - last_ms >= interval)`.

## Case 6: Bitwise Flags for System State
**Context**: A robot has various boolean states (moving, error, low_battery, wifi_connected).
**Bad Practice**: 4 separate `bool` variables (Takes 4 bytes).
**Good Practice**: `uint8_t system_status_flags = 0x00U;` (Takes 1 byte).
**Explanation**: Using bitmasks (`#define FLAG_ERROR 0x01U`) combined with bitwise operators (`|=`, `&= ~`) saves memory and allows updating multiple states atomically.

## Case 7: ADC Reading (12-bit Resolution)
**Context**: Reading a battery voltage sensor via a 12-bit ADC.
**Bad Practice**: `uint8_t adc_val = read_adc();` (Truncates data!).
**Good Practice**: `uint16_t adc_raw_val = read_adc();`
**Explanation**: A 12-bit ADC returns values from 0-4095. A `uint8_t` can only hold 0-255. A `uint16_t` correctly holds the 12-bit data.

## Case 8: Magic Numbers in Configuration
**Context**: Setting the baud rate for a UART printer interface.
**Bad Practice**: `init_uart(115200);`
**Good Practice**: `constexpr uint32_t PRINTER_BAUD_RATE = 115200U; init_uart(PRINTER_BAUD_RATE);`
**Explanation**: `constexpr` (C++) or `#define` / `const` (C) makes the code readable and compliant with MISRA rules against "magic numbers".

## Case 9: Avoiding Signed/Unsigned Mismatch
**Context**: A loop counting down packets to send.
**Bad Practice**: 
```cpp
for (uint8_t i = 5; i >= 0; i--) { send(); } // Infinite loop!
```
**Good Practice**: 
```cpp
for (int8_t i = 5; i >= 0; i--) { send(); }
```
**Explanation**: An unsigned integer is always `>= 0`. When it reaches 0 and is decremented, it wraps around to 255. Using a signed integer prevents this specific infinite loop.

## Case 10: Struct Padding and Network Payloads
**Context**: Sending a data packet over LoRa (IoT).
**Bad Practice**:
```cpp
struct Payload {
    uint8_t id;
    uint32_t data; // Compiler adds 3 bytes of padding here!
};
```
**Good Practice**:
```cpp
#pragma pack(push, 1)
struct Payload {
    uint8_t id;
    uint32_t data;
};
#pragma pack(pop)
```
**Explanation**: By default, compilers align 32-bit integers to 4-byte boundaries, adding hidden padding bytes. When transmitting structs over networks or serial lines, you must pack them to prevent sending garbage bytes and wasting bandwidth.
