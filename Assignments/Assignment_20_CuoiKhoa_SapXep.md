C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment20> TYPE: <CuoiKhoa> LOC: <Lines of Code> DURATION: <360 minutes> |

## Bài Tập Cuối Khóa: Robot Arm Controller - MISRA Compliance Review

### Exercise 1: Complete Embedded System Design
> [!TIP]
> **Hướng dẫn tự học:** Đây là Bài tập cuối khóa tích hợp toàn diện. Hãy xem lại cấu trúc lớp HAL tại [06_OOP_Basics/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) (Case 10) và mô hình State Machine tại [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 3, 10). Bạn cũng nên đối chiếu các nguyên lý functional safety trong tài liệu nền tảng [ADVANCED_MCU_ECU_KNOWLEDGE.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/ADVANCED_MCU_ECU_KNOWLEDGE.md).

**Ngữ cảnh**: Thiết kế firmware cho Robot Arm Controller với các requirements:
- 3 DOF (Degree of Freedom): X, Y, Z axes
- Encoders for position feedback (500 CPR each)
- Stepper motors with micro-stepping
- RS-485 communication for host PC
- Fault detection (overcurrent, overtemp, limit switches)
- Real-time trajectory calculation

**Yêu cầu**:
1. Chọn kiểu dữ liệu cho:
   - Motor position: `int32_t` (steps count, range -2M to +2M)
   - Encoder feedback: `uint16_t` (0-4095 for 12-bit ADC)
   - Temperature: `int16_t` (range -40 to +150°C)
   - Fault code: `uint8_t` (bitmap for multiple fault types)
   - Timestamp: `uint32_t` (milliseconds since boot)
2. Thiết kế kiến trúc theo layers:
   - Application: Trajectory planning, kinematics
   - Services: Motion control, fault monitoring
   - HAL: Motor HAL, Encoder HAL, Communication HAL
   - Drivers: Stepper driver, MAX11100 ADC, RS-485 transceiver
3. Design state machine cho operation:
   - INIT → CALIBRATING → IDLE → MOVING → FAULT → RESET

### Exercise 2: Memory Layout Optimization
> [!TIP]
> **Hướng dẫn tự học:** Hãy xem kỹ phương án phân bổ vùng nhớ BSS, DATA, RODATA và kỹ thuật Fixed-Point Math tại **Chương 02 (Memory Layout)** và **Chương 12 (Optimization)**. Tham khảo gợi ý tại [02_MemoryLayout_Compile/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) (Case 3, 4) và [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 1, 2).

**Ngữ cảnh**: MCU với 128KB Flash, 32KB RAM. Robot firmware cần fit trong constraints.
```c
// Current memory usage
const char firmware_version[] = "v2.1.3";        // .rodata
uint16_t trajectory_points[1000];                  // .data
uint8_t motor_status[3];                            // .bss
float pid_coefficients[3] = {1.0f, 0.1f, 0.05f};   // .data
uint8_t adc_raw_buffer[256];                        // .bss
```
**Yêu cầu**:
1. Phân loại mỗi biến vào đúng section (.text, .rodata, .data, .bss)
2. Tính tổng RAM usage và Flash usage
3. Optimize trajectory_points: dùng `int16_t` thay vì `float` (Q16.16 format) → tiết kiệm 2KB
4. Áp dụng const cho firmware_version → đẩy vào .rodata
5. Tính lại tổng sau optimization

### Exercise 3: Critical Function Design (MISRA Compliant)
> [!TIP]
> **Hướng dẫn tự học:** Đây là bài tập thực hành tuân thủ quy tắc viết code an toàn MISRA C:2012. Hãy xem kỹ các chỉ dẫn về validation, ép kiểu an toàn và volatile tại **Chương 01 (volatile & promotion)** và **Chương 03 (strict aliasing & safe casts)**. Tham khảo [01_Variables_DataTypes/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/01_Variables_DataTypes/example.md) (Case 2, 9) và [03_Arrays_Pointers_Reference/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) (Case 6).

**Ngữ cảnh**: Viết hàm điều khiển motor tuân thủ MISRA C:2012.
**Yêu cầu**:
1. Viết function signature với:
   - Pass by pointer (không dùng reference vì embedded)
   - Input validation (null check)
   - Volatile qualified parameters nếu accessed by ISR
2. Viết hàm:
```c
// Motor control function - MISRA compliant
extern void Motor_SetSpeed(uint8_t axis, int32_t speed);
extern bool validate_speed(int32_t speed);

void Robot_MoveAxis(uint8_t axis, int32_t target_pos) {
    // 1. Input validation - Rule 14.3 (all paths must have predictable behavior)
    if (axis > 2U) {
        Error_SetCode(ERR_INVALID_AXIS);
        return;
    }

    // 2. Validate target position
    if (!validate_speed(target_pos)) {
        Error_SetCode(ERR_INVALID_POSITION);
        return;
    }

    // 3. Check limit switches
    if (LimitSwitch_GetState(axis) == LIMIT_HIT) {
        Error_SetCode(ERR_LIMIT_SWITCH);
        return;
    }

    // 4. Calculate trajectory
    int32_t current_pos = Encoder_GetPosition(axis);

    // 5. Execute movement
    Motor_SetSpeed(axis, target_pos);
}
```
3. Đảm bảo không có warning: unused return values, implicit conversions

### Exercise 4: Testing Strategy Cho Safety-Critical System
> [!TIP]
> **Hướng dẫn tự học:** Hãy xem lại toàn bộ các phương pháp thiết kế kịch bản thử nghiệm, tiêm lỗi và độ bao phủ MC/DC tại **Chương 10 (Unit Testing)**. Tham khảo mẫu test và mô phỏng thời gian tại [10_UnitTest/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) (Case 2, 5, 6, 8).

**Ngữ cảnh**: Robot arm là safety-critical (ISO 26262 compliance).
**Yêu cầu**:
1. Design test pyramid: Unit → Integration → System → HIL
2. Áp dụng MC/DC coverage requirement (100% cho ASIL-B)
3. Liệt kê test cases cho motor control function:
   - Valid inputs: axis 0-2, position within range
   - Boundary: MAX_POS, MIN_POS, zero
   - Invalid: axis > 2, position overflow
   - Fault injection: simulate limit switch trigger
4. Trình bày cách dùng Fake Time Source để test timing-critical code

### Exercise 5: Design Pattern Implementation Review
> [!TIP]
> **Hướng dẫn tự học:** Hãy ôn tập toàn bộ các mẫu thiết kế nhúng 0-overhead chuyên sâu tại **Chương 13 (Design Patterns)**. Xem kỹ [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 1, 3, 4, 5, 7, 10).

**Yêu cầu**: Review và implement các patterns đã học:
1. **State Machine**: Implement cho Robot với 6 states sử dụng function pointer array
2. **Command Pattern**: Đóng gói movement commands vào queue
3. **Observer Pattern**: Temperature monitor notifies multiple listeners
4. **HAL Pattern**: Abstract motor interface cho multiple motor types
5. **Active Object**: Config task xử lý messages asynchronously

---

## Bài Tập Sắp Xếp (Ordering Exercises)

### Exercise 6: Sắp Xếp Toàn Bộ Quy Trình Phát Triển Firmware
> [!TIP]
> **Hướng dẫn tự học:** Lộ trình chữ V của ISO 26262 và quy trình phát triển firmware chuyên nghiệp được hướng dẫn chi tiết tại [ADVANCED_MCU_ECU_KNOWLEDGE.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/ADVANCED_MCU_ECU_KNOWLEDGE.md) và củng cố tại **Chương 10 (Unit Testing)**.

**Yêu cầu**: Sắp xếp các phase của development lifecycle đúng thứ tự:
1. Requirements Analysis → define specs
2. Software Architecture Design → layer design
3. Detailed Design → class/function design
4. Implementation → write code
5. Static Analysis → linting, MISRA check
6. Unit Testing → component test
7. Integration Testing → combine modules
8. System Testing → full system test
9. Certification → ISO 26262 / MISRA compliance
10. Deployment → release to production

### Exercise 7: Sắp Xếp Thứ Tự Debug Cho Complex Bug
> [!TIP]
> **Hướng dẫn tự học:** Để ôn tập các bước cô lập và gỡ lỗi phần cứng nhúng phức tạp, hãy xem các kịch bản debug và Heisenbug tại **Chương 05 (Debugging)** và **Chương 12 (Treo máy & Race conditions)**. Xem tệp [05_FileIO_Debugging/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) (Case 4, 7) và [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 3, 10).

**Ngữ cảnh**: Robot arm đột nhiên dừng hoạt động sau 10 phút chạy.
**Yêu cầu**: Sắp xếp các bước debug đúng thứ tự:
1. Collect symptoms: "stops after 10 min", "temperature rising"
2. Check symptoms: read temperature logs, find temp at 85°C
3. Hypothesis: thermal shutdown due to overheating
4. Verify: check if motor drivers have thermal protection
5. Root cause: heatsink disconnected
6. Fix: reattach heatsink, apply thermal paste
7. Validate: run 24-hour stress test
8. Close: document in bug tracker, update thermal design

---

## Final Assessment Checklist

### Knowledge Classification (Self-Assessment)
Trước khi hoàn thành, hãy tự đánh giá kiến thức của bạn:

**Topic 1-3 (C Fundamentals)**:
- [ ] Hiểu sự khác biệt giữa `int` và `uint32_t`
- [ ] Có thể vẽ memory layout cho struct
- [ ] Sử dụng được bitwise operations cho embedded tasks

**Topic 4-5 (Functions & Debug)**:
- [ ] Viết được reentrant function
- [ ] Hiểu stack frame mechanism
- [ ] Debug với hardware debugger (JTAG/SWD)

**Topic 6-8 (OOP & C++)**:
- [ ] Thiết kế class với encapsulation
- [ ] Sử dụng inheritance và virtual functions
- [ ] Viết template class cho generic containers

**Topic 9-11 (Data Structures & Advanced C++)**:
- [ ] Implement Ring Buffer không dùng heap
- [ ] Sử dụng std::atomic cho thread safety
- [ ] Hiểu move semantics và RAII

**Topic 12-13 (Optimization & Patterns)**:
- [ ] Tối ưu struct alignment
- [ ] Implement state machine với function pointers
- [ ] Design Observer pattern cho event handling

**Cross-Cutting Skills**:
- [ ] Tuân thủ MISRA C/C++ guidelines
- [ ] Thiết kế test cases với MC/DC coverage
- [ ] Phân tích performance với profiling tools