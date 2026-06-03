C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment19> TYPE: <CongTacLon> LOC: <Lines of Code> DURATION: <360 minutes> |

## Bài Tập Lớn: IoT Gateway Controller - Tích Hợp Toàn Diện

### Exercise 1: Thiết Kế Kiến Trúc Tổng Thể
> [!TIP]
> **Hướng dẫn tự học:** Đây là Bài tập lớn tích hợp toàn diện khóa học. Hãy ôn tập kỹ năng phân lớp kiến trúc AUTOSAR và HAL tại [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 1) và tư duy an toàn hệ thống nhúng tại tài liệu chuẩn [ADVANCED_MCU_ECU_KNOWLEDGE.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/ADVANCED_MCU_ECU_KNOWLEDGE.md).

**Ngữ cảnh**: Thiết kế firmware cho IoT Gateway với các features:
- Thu thập data từ 4 sensors (温度, 湿度, pressure, vibration)
- Giao tiếp MQTT qua WiFi
- Lưu trữ local với NVM (EEPROM/Flash)
- Watchdog supervision
- Low-power sleep mode

**Yêu cầu**:
1. Vẽ kiến trúc layered (Application → HAL → Drivers → Hardware)
2. Định nghĩa các main modules và responsibilities
3. Design data flow: Sensor → ADC → Processing → MQTT → Network
4. Xác định critical timing constraints:
   - Sensor sampling: 100ms interval
   - MQTT publish: every 30 seconds
   - Watchdog: pet every 1 second
   - NVM save: every 5 minutes

### Exercise 2: Memory Budget Analysis
> [!TIP]
> **Hướng dẫn tự học:** Hãy xem kỹ các Case Study về tối ưu hóa bộ nhớ Flash và SRAM tại **Chương 02 (Memory Layout)** và **Chương 12 (Tối ưu hóa căn lề)**. Tham khảo gợi ý tại [02_MemoryLayout_Compile/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) (Case 1, 3, 8) và [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 1, 5).

**Ngữ cảnh**: MCU với 64KB Flash, 20KB RAM, không có heap.
**Yêu cầu**:
1. Phân chia memory map:
   - .text (code): ~40KB budget
   - .rodata (const): ~8KB budget
   - .data + .bss: ~8KB budget
   - Stack: ~4KB (limit)
   - Heap: 0 (forbidden)
2. Tính toán RAM cho:
   - 4 × Sensor buffers (100 samples each, int16_t = 800 bytes)
   - MQTT tx/rx buffers (2KB each = 4KB)
   - Ring buffer for commands (256 bytes)
   - State machine variables (~500 bytes)
3. Verify total RAM: 800 + 4000 + 256 + 500 = 5.5KB (within 8KB budget)

### Exercise 3: State Machine + Command Queue Integration
> [!TIP]
> **Hướng dẫn tự học:** Ôn tập sự phối hợp nhịp nhàng giữa Hàng đợi ngắt và Máy trạng thái tại **Chương 09 (Data Structures)** và **Chương 13 (Design Patterns)**. Xem gợi ý tại [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 2, 7) và [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 3, 5, 10).

**Yêu cầu**:
1. Design state machine với states:
   - INIT → CONNECTING → CONNECTED → SAMPLING → TRANSMITTING → SLEEP → WAKE → ...
2. Implement command queue (Ring Buffer) cho incoming MQTT commands
3. Show integration: ISR receives WiFi data → enqueue command → main loop dequeues → state machine processes
4. Handle invalid commands gracefully (log error, stay in current state)

### Exercise 4: Error Handling Và Fault Recovery
> [!TIP]
> **Hướng dẫn tự học:** Tư duy an toàn chức năng và dự phòng lỗi được đúc kết sâu sắc tại **Chương 05 (Debugging & Asserts)** và **Chương 12 (Watchdog supervision)**. Tham khảo [05_FileIO_Debugging/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) (Case 8) và [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 4, 10).

**Yêu cầu**:
1. Design fault handling architecture:
   - Fault detection (sensor out-of-range, timeout, checksum error)
   - Fault logging (store in NVM for post-mortem analysis)
   - Fault recovery (retry count, fallback to safe state)
2. Implement watchdog supervision:
   - Main task pets dog every 1 second
   - If dog not pet for 3 seconds → system reset
3. Design safe state: if all sensors fail, system enters SLEEP mode with periodic wake to retry

### Exercise 5: Performance Optimization Checklist
> [!TIP]
> **Hướng dẫn tự học:** Hãy ôn tập toàn bộ các phương án tối ưu hóa hiệu năng bare-metal tại **Chương 12 (Tối ưu hóa & Lỗi thường gặp)**. Xem kỹ [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 1, 2, 3, 9).

**Yêu cầu**: Áp dụng các optimization techniques đã học:
1. Bitwise thay cho arithmetic (modulo → AND, division → shift)
2. Integer math thay cho floating-point (no FPU)
3. Function inlining cho small functions (`inline` keyword)
4. Constant propagation (`constexpr` for lookup tables)
5. Dead code elimination (`-ffunction-sections --gc-sections`)
6. Struct member ordering để eliminate padding
7. Ring buffer index calculation với AND thay cho modulo

---

## Bài Tập Sắp Xếp (Ordering Exercises)

### Exercise 6: Sắp Xếp System Boot Sequence
> [!TIP]
> **Hướng dẫn tự học:** Ôn tập quy trình khởi tạo MCU và copy bộ nhớ lúc boot tại **Chương 02 (Memory Layout & Compile)**. Xem gợi ý tại [02_MemoryLayout_Compile/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) (Case 10).

**Yêu cầu**: Sắp xếp boot sequence đúng thứ tự:
1. Power on → hardware reset
2. Copy .data init values from Flash to RAM
3. Zero .bss section
4. Initialize stack pointer (SP)
5. Call Reset_Handler → SystemInit()
6. Configure clock (PLL)
7. Initialize global objects (C++ constructors)
8. Jump to main()

### Exercise 7: Sắp Xếp Data Flow Từ Sensor Đến Cloud
> [!TIP]
> **Hướng dẫn tự học:** Dòng chảy dữ liệu từ phần cứng qua HAL đến mạng được tích hợp sâu chéo nhiều chương trong khóa học. Hãy ôn tập kỹ năng HAL Strategy và DMA đệm kép tại **Chương 03 (Double Buffering)** và **Chương 13 (HAL Pattern)**. Xem gợi ý tại [03_Arrays_Pointers_Reference/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) (Case 3) và [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 1).

**Yêu cầu**: Sắp xếp data flow path khi temperature reading reaches MQTT broker:
- A. ADC converts analog voltage to digital value (12-bit)
- B. DMA transfers ADC value to memory buffer
- C. Sensor driver reads raw value from buffer
- D. Apply calibration: temp = (raw - offset) * scale
- E. Store in circular buffer (100 samples)
- F. Every 30s: MQTT task reads latest sample
- G. Format as JSON: {"temp": 25.5, "humidity": 60}
- H. WiFi driver transmits packet to broker
- I. Cloud server receives and stores data