# Functions & Passing Variables - 10 Case Studies

## Case 1: State Machine Update Function
**Context**: A robot navigation task.
**Bad Practice**: `void update_nav(RobotState state)`
**Good Practice**: `void update_nav(RobotState& state)`
**Explanation**: Passing a struct by value creates a copy. The function would modify its local copy, leaving the original state unchanged. Passing by reference allows modifying the actual state object efficiently.

## Case 2: Reading I2C Sensor (Out Parameter)
**Context**: Reading temperature and humidity from a single I2C transaction.
**Practice**: 
```c
// Use pointers to return multiple values from a C function
bool read_bme280(float* out_temp, float* out_hum) {
    if (out_temp == NULL || out_hum == NULL) return false;
    // ... hardware read ...
    *out_temp = 25.5f;
    *out_hum = 60.0f;
    return true; // Success status
}
```

## Case 3: Optimizing Parameter Passing (ARM Cortex-M ABI)
**Context**: A math function for coordinate transformation.
**Practice**:
```cpp
// Fast: Uses registers R0, R1, R2, R3
void calc_kinematics(int x, int y, int z, int w); 

// Slower: 'yaw' and 'pitch' are pushed to the RAM Stack
void calc_kinematics(int x, int y, int z, int w, int yaw, int pitch);
```
**Fix**: Group parameters into a `const struct&` to pass everything via a single register (the pointer to the struct).

## Case 4: The Danger of Global Variables vs. Parameters
**Context**: A function filtering ADC noise.
**Bad Practice**:
```cpp
uint16_t adc_val; // Global
void filter_adc() { adc_val = (adc_val + prev_val) / 2; }
```
**Good Practice**: 
```cpp
uint16_t filter_adc(uint16_t current, uint16_t previous) { 
    return (current + previous) / 2; 
}
```
**Explanation**: Functions should be "pure" where possible, relying only on their inputs. Relying on globals makes code hard to test, hard to reuse, and dangerous in multi-threaded (RTOS) environments.

## Case 5: Reentrancy Bug in an IoT Node
**Context**: A string formatting function called by Main Loop and an ISR.
**Bad Practice**:
```c
char* format_timestamp(uint32_t unix_time) {
    static char buffer[32]; // STATIC variable! Non-reentrant!
    sprintf(buffer, "Time: %lu", unix_time);
    return buffer;
}
```
**Explanation**: If the Main Loop is formatting a string, and an ISR interrupts it and calls `format_timestamp()`, the ISR will overwrite `buffer`. When the Main Loop resumes, it gets corrupted data.
**Fix**: Let the caller provide the buffer: `void format_timestamp(uint32_t unix_time, char* out_buf, size_t buf_len)`

## Case 6: Callback Functions (Function Pointers)
**Context**: A Timer module needs to notify the Application Layer when it expires, without knowing about the Application Layer (Decoupling).
**Practice**:
```c
typedef void (*TimerCallback)(void);
void Timer_Start(uint32_t ms, TimerCallback cb) {
    // ... setup hardware ...
    // Store cb to call it later in ISR
}
// Application:
void ToggleLED() { /*...*/ }
// ...
Timer_Start(1000, ToggleLED);
```

## Case 7: Inline Getter/Setter for Register Encapsulation
**Context**: Creating a clean C++ API for hardware.
**Practice**:
```cpp
class GPIO {
public:
    // 'inline' prevents function call overhead. Compiled exactly like a raw register access.
    inline void setHigh() { *PORT_SET_REG = (1 << pin_); }
private:
    volatile uint32_t* PORT_SET_REG;
    uint8_t pin_;
};
```

## Case 8: Array Decay in Functions
**Context**: Calculating the checksum of a packet.
**Bad Practice**:
```cpp
uint8_t calc_crc(uint8_t packet[256]) {
    // sizeof(packet) returns 4 (size of pointer), NOT 256!
    for(int i = 0; i < sizeof(packet); i++) { ... } 
}
```
**Good Practice**:
```cpp
uint8_t calc_crc(const uint8_t* packet, uint16_t length) {
    // Must pass length explicitly
}
```

## Case 9: Read-Only Struct Passing (MISRA)
**Context**: Initializing a motor driver.
**Practice**:
```c
// 'const' guarantees the init function won't alter the user's config struct
void Motor_Init(const MotorConfig_t* config) {
    if(config == NULL) return;
    HW_REG_SPEED = config->max_speed;
}
```

## Case 10: Stack Overflow from Recursion
**Context**: Calculating Factorial or parsing a tree.
**Bad Practice**: `int factorial(int n) { return (n==1) ? 1 : n * factorial(n-1); }`
**Explanation**: MISRA C strictly forbids recursion. In embedded systems, the stack is tiny (e.g., 1KB-4KB). Recursion depth is unpredictable and will quickly crash the microcontroller. Always use iterative loops (`while`, `for`) instead.