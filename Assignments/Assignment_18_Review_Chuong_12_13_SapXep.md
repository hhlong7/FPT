C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment18> TYPE: <Review_Chuong_12_13> LOC: <Lines of Code> DURATION: <240 minutes> |

## Review Chương 12-13: Design Patterns & Advanced C++

### Exercise 1: State Machine + Command Pattern Integration
> [!TIP]
> **Hướng dẫn tự học:** Bài tập lớn tích hợp chéo giữa các mẫu thiết kế của **Chương 13 (Design Patterns)** và **Chương 09 (Data Structures)**. Hãy xem ví dụ thực tế nạp và phân phối lệnh tại [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 3, 5, 10) và [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 2, 7).

**Ngữ cảnh**: Máy in 3D điều khiển bằng G-code commands.
**Yêu cầu**:
1. Design state machine: IDLE → HEATING → PRINTING → PAUSED → FAULT → IDLE
2. Sử dụng Function Pointer Array cho state table
3. Design Command Pattern: mỗi G-code command được đóng gói vào struct
4. Command queue (Ring Buffer) lưu pending commands
5. Trình bày flow: Wifi receives "G1 X100" → Command created → enqueued → State machine processes

### Exercise 2: RAII + Move Semantics Cho Resource Management
> [!TIP]
> **Hướng dẫn tự học:** Hãy ôn tập kỹ cơ chế Move Semantics zero-copy và quản lý tài nguyên tự động RAII tại **Chương 11 (C++ Nâng cao)**. Tham khảo gợi ý tại [11_Advanced_Cpp_Features/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/11_Advanced_Cpp_Features/example.md) (Case 2, 4, 6).

**Ngữ cảnh**: File handler class cần quản lý resource an toàn.
**Yêu cầu**:
1. Viết class `FileHandle` với RAII pattern (constructor opens file, destructor closes)
2. Thêm `= delete` cho copy constructor/assignment (non-copyable)
3. Thêm move constructor và move assignment (`&&`)
4. Viết demo: `FileHandle f1("log.txt"); FileHandle f2 = std::move(f1);`
5. Trình bày tại sao sau move, `f1` không còn quản lý file

### Exercise 3: HAL Với Strategy Pattern Cho Multi-Sensor
> [!TIP]
> **Hướng dẫn tự học:** Bài tập củng cố kiến thức **Chương 07 (C++ Hướng đối tượng nâng cao)** và **Chương 13 (Design Patterns)**. Xem kỹ ví dụ cấu trúc HAL an toàn và đa hình tĩnh CRTP tại [07_OOP_Features/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/07_OOP_Features/example.md) (Case 1, 6) và [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 1, 8, 9).

**Ngữ cảnh**: Cần support 3 sensor types với cùng interface.
**Yêu cầu**:
1. Define interface `ISensor` với `virtual int16_t read() = 0`
2. Implement 3 drivers: `TempSensor_I2C`, `TempSensor_SPI`, `TempSensor_1Wire`
3. Trong main application, chỉ dùng `ISensor*` pointer
4. Show how to switch sensor at runtime: `setSensor(new TempSensor_SPI())`
5. Analyze performance: virtual call overhead vs inline template (CRTP)

### Exercise 4: Sắp Xếp Observer Pattern Flow
> [!TIP]
> **Hướng dẫn tự học:** Hãy xem kỹ cách triển khai Observer decoupling để giảm chằng chịt giữa các module tại **Chương 13 (Design Patterns)**. Xem tệp [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 4, 6).

**Yêu cầu**: Sắp xếp event flow trong Observer pattern khi temp sensor triggers warning:
1. TempSensor::check() detects temp > threshold
2. TempSensor calls Observer::notify(OVERHEAT)
3. Observer base class iterates callback list
4. Display_Callback() triggers LCD blink
5. Fan_Callback() triggers fan at 100%
6. Motor_Callback() triggers emergency stop
7. Logger_Callback() writes fault to EEPROM

### Exercise 5: Sắp Xếp Thread-Safety Implementation Steps
> [!TIP]
> **Hướng dẫn tự học:** Ôn tập các kỹ thuật đồng bộ dữ liệu ngắt bằng biến nguyên tử tại **Chương 11 (C++ Nâng cao)**. Tham khảo tệp [11_Advanced_Cpp_Features/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/11_Advanced_Cpp_Features/example.md) (Case 5).

**Yêu cầu**: Sắp xếp các bước implement thread-safe shared data:
1. Identify shared resource: `uint32_t packet_count`
2. Declare as `std::atomic<uint32_t>` instead of plain uint32_t
3. Use `fetch_add(1)` for increment operation
4. Replace `if (count == 0)` with `load()` then compare
5. Verify no race conditions with stress test

---

## Bài Tập Sắp Xếp (Ordering Exercises)

### Exercise 6: Sắp Xếp State Machine Transition Flow
> [!TIP]
> **Hướng dẫn tự học:** Ôn tập cấu trúc máy trạng thái siêu gọn bằng con trỏ hàm tại **Chương 13 (Design Patterns)**. Xem gợi ý tại [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 3, 10).

**Yêu cầu**: Sắp xếp transition flow khi printer receives "M104 S200" (set extruder temp):
- A. UART ISR receives 'M', '1', '0', '4', ...
- B. Command parser identifies M104 (set temperature)
- C. Extract target temp: 200°C
- D. Transition state: IDLE → HEATING
- E. Heater PID controller starts regulating
- F. When temp reaches 200, transition HEATING → READY
- G. Main loop updates LCD display with current temp

### Exercise 7: Sắp Xếp Active Object Message Flow
> [!TIP]
> **Hướng dẫn tự học:** Hãy xem kỹ cách xây dựng Active Object để loại bỏ hoàn toàn Mutex và tranh chấp bộ nhớ tại **Chương 13 (Design Patterns)**. Xem tệp [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 7).

**Yêu cầu**: Sắp xếp message flow trong Active Object pattern (Config Task):
1. Wifi_Task wants to update IP configuration
2. Wifi_Task sends message to Config_Task mailbox
3. Config_Task's main loop receives message from queue
4. Config_Task parses message: SET_IP, new_ip_value
5. Config_Task updates private `ip_config` member variable
6. Config_Task sends ACK back to Wifi_Task mailbox
7. Wifi_Task receives ACK, continues operation