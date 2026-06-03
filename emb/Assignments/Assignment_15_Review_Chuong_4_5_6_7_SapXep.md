C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment15> TYPE: <Review_Chuong_4_5_6_7> LOC: <Lines of Code> DURATION: <240 minutes> |

## Review Chương 4-5-6-7: Functions, OOP & Design Patterns

### Exercise 1: Function Design Cho Hệ Thống Real-time
> [!TIP]
> **Hướng dẫn tự học:** Bài tập tích hợp kiến thức **Chương 04 (Thiết kế Hàm & Truyền tham số)** và **Chương 05 (File I/O & Debugging)**. Hãy tham chiếu gợi ý tại [04_Functions_PassingVariables/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/04_Functions_PassingVariables/example.md) (Case 3, 5, 9) và [05_FileIO_Debugging/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) (Case 1).

**Ngữ cảnh**: Viết hàm điều khiển motor với yêu cầu:
- Thời gian phản hồi < 100μs
- Không được dùng malloc (MISRA compliance)
- Hàm phải là reentrant (sử dụng trong RTOS với nhiều tasks)
**Yêu cầu**:
1. Thiết kế function signature sử dụng struct thay vì nhiều parameters
2. Giải thích tại sao truyền struct bằng const pointer (`const MotorCommand*`) tối ưu hơn pass by value
3. Viết hàm `void Motor_Execute(const MotorCommand* cmd)` đảm bảo:
   - Không dùng biến static (tránh race condition)
   - Copy dữ liệu cần thiết vào local variables
4. Trình bày cách kiểm tra reentrancy của hàm

### Exercise 2: Class Design Cho Hệ Thống Máy In (OOP Basics)
> [!TIP]
> **Hướng dẫn tự học:** Bài tập ôn tập toàn diện **Chương 06 (Lập trình hướng đối tượng cơ bản)**. Tham khảo các mẫu đóng gói và khởi tạo tại [06_OOP_Basics/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) (Case 3, 5, 7, 9).

**Ngữ cảnh**: Thiết kế class cho hệ thống điều khiển máy in 3D.
**Yêu cầu**:
1. Viết class `HeaterController` với:
   - Private: `uint16_t current_temp`, `uint16_t target_temp`, `uint16_t pid_kp`
   - Public: Constructor nhận target, method `update()` chạy mỗi 10ms
   - Method `setTarget(uint16_t temp)` và `getCurrentTemp()`
2. Trình bày nguyên tắc Encapsulation - tại sao `current_temp` phải là private
3. Nếu dùng class này trong ISR, cần lưu ý gì về thread-safety?
4. Viết phiên bản "Two-Stage Init" để đảm bảo hardware được init sau clock config

### Exercise 3: HAL Interface Cho Multiple Sensor Drivers
> [!TIP]
> **Hướng dẫn tự học:** Bài tập tổng hợp nâng cao kiến thức **Chương 07 (C++ Hướng đối tượng nâng cao)**. Hãy phân tích chi phí ẩn và giải pháp thay thế tại [07_OOP_Features/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/07_OOP_Features/example.md) (Case 1, 2, 6).

**Ngữ cảnh**: Cần hỗ trợ 3 loại cảm biến nhiệt khác nhau: TMP102 (I2C), DHT22 (1-wire), MAX31855 (SPI).
**Yêu cầu**:
1. Định nghĩa abstract base class `ISensor` với virtual destructor và pure virtual method `int16_t readTemperature()`
2. Viết 3 derived classes cho 3 sensor types, override `readTemperature()`
3. Trình bày cách sử dụng polymorphic array: `ISensor* sensors[3]`
4. Giải thích chi phí của virtual function call (vptr, vtable lookup) và khi nào nên dùng CRTP thay thế

### Exercise 4: Sắp Xếp Thứ Tự Khởi Tạo Trong Constructor
> [!TIP]
> **Hướng dẫn tự học:** Hãy tham khảo quy tắc quản lý vòng đời đối tượng và Two-Stage Init tại **Chương 06 (Lập trình hướng đối tượng cơ bản)**. Xem tệp [06_OOP_Basics/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) (Case 3, 8).

**Ngữ cảnh**: Class có nhiều thành viên phụ thuộc lẫn nhau.
```cpp
class PrinterSystem {
    Display display;           // Depends on SPI
    MotorController motors;     // Depends on Timer
    TemperatureSensor temp;     // Depends on ADC
public:
    PrinterSystem() {
        // What order should these init?
    }
};
```
**Yêu cầu**: Sắp xếp thứ tự khởi tạo đúng (giải thích dependency):
1. temp.init() - ADC must be ready first
2. display.init() - Display needs SPI clock
3. motors.init() - Motors need Timer configured

### Exercise 5: Sắp Xếp Thứ Tự Áp Dụng Design Patterns
> [!TIP]
> **Hướng dẫn tự học:** Kỹ năng phân tích dòng chảy kiến trúc hệ thống nhúng từ HAL đến App được giảng dạy sâu tại **Chương 13 (Mẫu thiết kế nhúng)**. Tham khảo tệp [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 1, 3, 5).

**Ngữ cảnh**: Khi thiết kế firmware mới, thứ tự áp dụng các pattern ảnh hưởng đến kiến trúc.
**Yêu cầu**: Sắp xếp thứ tự đúng khi design hệ thống:
1. Identify hardware interfaces → create HAL (Strategy)
2. Define main entities → create Classes (OOP)
3. Define state transitions → create State Machine
4. Identify event handlers → create Observer/Callback
5. Identify data flow → create Command Queue

---

## Bài Tập Sắp Xếp (Ordering Exercises)

### Exercise 6: Sắp Xếp Quy Trình Xử Lý Lệnh G-Code
> [!TIP]
> **Hướng dẫn tự học:** Quá trình giải nén và nội suy G-Code là sự kết hợp giữa Queue ở **Chương 09 (Cấu trúc dữ liệu)** và Command Pattern ở **Chương 13 (Design Patterns)**. Xem tệp [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 2) và [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 5).

**Yêu cầu**: Sắp xếp các bước xử lý lệnh "G1 X100 Y50 Z10" đúng thứ tự:
- A. Parse string → extract G, X, Y, Z values
- B. Validate parameters (range check)
- C. Convert to motor steps (steps/mm ratio)
- D. Add command to Ring Buffer queue
- E. Motor ISR reads from queue, generates step pulses
- F. Stepper motor actually moves to target position

### Exercise 7: Sắp Xếp Cấp Độ Trừu Tượng (Abstraction Levels)
> [!TIP]
> **Hướng dẫn tự học:** Phân lớp phần mềm nhúng để cách ly thanh ghi ngoại vi là bài học xuyên suốt, tiêu biểu tại [06_OOP_Basics/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) (Case 10) và [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 1).

**Yêu cầu**: Sắp xếp từ thấp đến cao (hardware → application):
1. `volatile uint32_t* const TIM3_CNT = (uint32_t*)0x40000400;`
2. `StepperMotor::step(unsigned steps)`
3. `MotionPlanner::moveTo(uint16_t x, uint16_t y)`
4. `GCodeExecutor::executeLine("G1 X100")`
5. Hardware register bit manipulation: `GPIOA->BSRR = (1 << 5)`
6. `ISR_TIMER3()` handler