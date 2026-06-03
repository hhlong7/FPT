C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment17> TYPE: <Review_Chuong_10_11_12> LOC: <Lines of Code> DURATION: <240 minutes> |

## Review Chương 10-11-12: Testing & Optimization

### Exercise 1: Unit Test Cho State Machine
> [!TIP]
> **Hướng dẫn tự học:** Bài tập tổng hợp kiến thức từ **Chương 10 (Unit Testing)** kết hợp với **Chương 13 (Design Patterns)**. Hãy tham khảo cách thiết kế test case và mock thời gian tại [10_UnitTest/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) (Case 4, 5) và mô hình State Machine tại [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 3, 10).

**Ngữ cảnh**: Cần test state machine có 4 states: IDLE → HEATING → PRINTING → FAULT.
**Yêu cầu**:
1. Liệt kê các test cases cần thiết cho state transitions (sử dụng BVA/boundary analysis)
2. Viết test pseudo-code cho transition IDLE → HEATING (trigger: temp < target)
3. Test case cho invalid transition (VD: PRINTING → IDLE không được phép)
4. Trình bày cách dùng fake time source để test timing-sensitive transitions

### Exercise 2: Memory Optimization Cho Struct Alignment
> [!TIP]
> **Hướng dẫn tự học:** Ôn tập kỹ thuật tối ưu hóa bộ nhớ cốt tử bằng cách căn lề struct tại **Chương 12 (Optimization & Defects)**. Hãy xem ví dụ thực tế và biểu đồ bộ nhớ tại [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 1).

**Ngữ cảnh**: Sensor payload struct đang gây lãng phí RAM:
```c
struct Payload {
    uint8_t id;      // 1 byte
    uint32_t ts;     // 4 bytes → padding 3 bytes before
    int16_t temp;    // 2 bytes
    uint8_t flag;    // 1 byte → total 16 bytes due to padding
};
```
**Yêu cầu**:
1. Vẽ memory layout hiện tại (có padding)
2. Tính RAM lãng phí cho 1000 packets
3. Sắp xếp lại struct để tối ưu (không padding): `uint8_t id, flag; int16_t temp; uint32_t ts;`
4. Tính kích thước mới và RAM tiết kiệm

### Exercise 3: Performance Optimization Với Bitwise & Fixed-Point
> [!TIP]
> **Hướng dẫn tự học:** Bài tập tổng hợp nâng cao kỹ năng triệt tiêu float bằng toán số nguyên tại **Chương 12 (Tối ưu hóa nhúng)**. Tham khảo mẫu fixed-point PID và dịch bit tại [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 2, 3).

**Ngữ cảnh**: Tính toán PID controller chạy 10,000 lần/giây.
```c
int32_t error = target - current;
int32_t derivative = (error - prev_error) / delta_time;
output = (Kp * error + Ki * integral + Kd * derivative) / 256;
```
**Yêu cầu**:
1. Viết lại sử dụng fixed-point math (Q16.16 format) thay vì float
2. Thay phép chia `/256` bằng dịch bit `>> 8`
3. Tính CPU cycles tiết kiệm nếu không có FPU (float → integer)
4. Trình bày cách tránh overflow trong integral term

### Exercise 4: Sắp Xếp Test Coverage Levels
> [!TIP]
> **Hướng dẫn tự học:** Độ bao phủ code là kiến thức chuyên sâu của **Chương 10 (Unit Testing & MC/DC)**. Hãy xem ví dụ thực tiễn giải thích tại [10_UnitTest/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) (Case 8).

**Yêu cầu**: Sắp xếp theo thứ tự độ phủ code (từ thấp đến cao):
1. Statement Coverage - each statement executed
2. Branch Coverage - each decision point both true/false
3. Condition Coverage - each boolean expression evaluated
4. MC/DC Coverage - Modified Condition/Decision per ISO 26262
5. Function Coverage - each function called at least once

### Exercise 5: Sắp Xếp Các Bước Tối Ưu Hóa Code
> [!TIP]
> **Hướng dẫn tự học:** Hãy xem lại toàn bộ các phương án cấu hình cờ compiler và tối ưu hóa code nhúng tại **Chương 12 (Optimization)**. Tham khảo tệp [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 7, 9).

**Ngữ cảnh**: Quy trình tối ưu hóa firmware từ source đến binary.
**Yêu cầu**: Sắp xếp đúng thứ tự:
1. Profile code to identify bottlenecks
2. Apply bitwise optimizations where applicable
3. Enable compiler optimization flags (-O2, -Os)
4. Use -ffunction-sections --gc-sections for dead code elimination
5. Verify with size tool: text + data + bss
6. Run MC/DC test coverage analysis

---

## Bài Tập Sắp Xếp (Ordering Exercises)

### Exercise 6: Sắp Xếp Quy Trình Debug Với Watchpoint
> [!TIP]
> **Hướng dẫn tự học:** Hãy ôn tập kỹ năng hardware debugging và sử dụng Watchpoint tại **Chương 05 (Debugging)** và **Chương 12 (Race conditions)**. Xem gợi ý tại [05_FileIO_Debugging/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) (Case 4, 7) và [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 3).

**Yêu cầu**: Sắp xếp các bước debug data corruption bằng hardware watchpoint:
1. Identify variable: `uint8_t system_state`
2. Set data watchpoint on system_state address
3. Run program until watchpoint triggers
4. Inspect call stack to find write location
5. Identify ISR that's accidentally modifying the variable
6. Add volatile qualifier and atomic protection

### Exercise 7: Sắp Xếp Thứ Tự Boundary Test Cases
> [!TIP]
> **Hướng dẫn tự học:** Kỹ thuật kiểm thử đường biên phân vùng là bài học sống còn của **Chương 10 (Unit Testing)**. Tham khảo tệp [10_UnitTest/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) (Case 2, 6).

**Ngữ cảnh**: Phân loại nhiệt độ cảm biến.
**Yêu cầu**: Sắp xếp các giá trị test cần thiết (boundary analysis):
- -1, 0 (boundary between FREEZE and COLD)
- 9, 10, 11 (boundary between COLD and NORMAL)
- 29, 30, 31 (boundary between NORMAL and HOT)
- 49, 50, 51 (boundary between HOT and DANGER)
- -100, 100 (out of range test)