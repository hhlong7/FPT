C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment14> TYPE: <Review_Chuong_1_2_3> LOC: <Lines of Code> DURATION: <240 minutes> |

## Review Chương 1-2-3: Variables, Memory & Pointers

### Exercise 1: Từ Kiểu Dữ Liệu Đến Con Trỏ Trong Hệ Thống Nhúng
> [!TIP]
> **Hướng dẫn tự học:** Bài tập tổng hợp này tích hợp kiến thức từ **Chương 01 (Biến & Thao tác Bit)** và **Chương 03 (Mảng & Con trỏ)**. Hãy xem kỹ gợi ý tại [01_Variables_DataTypes/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/01_Variables_DataTypes/example.md) (Case 1, 3, 10) và [03_Arrays_Pointers_Reference/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) (Case 6).

**Ngữ cảnh**: Bạn cần viết driver cho cảm biến nhiệt độ giao tiếp qua I2C.
- Cảm biến trả về 2 bytes dữ liệu (Big-Endian)
- Giá trị temperature dao động từ -40°C đến 125°C
- Cần lưu trữ 1000 mẫu lịch sử trong RAM 32KB
**Yêu cầu**:
1. Chọn kiểu dữ liệu phù hợp cho biến temperature (giải thích tại sao dùng `int16_t` thay vì `float`)
2. Tính toán kích thước bộ nhớ cần thiết cho mảng 1000 mẫu (với kiểu `int16_t`)
3. Viết hàm đọc 2 bytes từ I2C và chuyển đổi Big-Endian sang giá trị temperature sử dụng con trỏ `uint8_t*` và phép dịch bit

### Exercise 2: Phân Tích Memory Layout Của Struct Cảm Biến
> [!TIP]
> **Hướng dẫn tự học:** Bài tập này tích hợp kiến thức **Chương 02 (Memory Layout)** và các lỗi tối ưu hóa căn lề. Hãy tham khảo mẫu code phân tích tại [02_MemoryLayout_Compile/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) (Case 3, 4) và lỗi struct padding tại [12_Optimization_CommonDefects/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) (Case 1).

**Ngữ cảnh**: Định nghĩa cấu trúc dữ liệu cho gói tin cảm biến:
```c
typedef struct {
    uint8_t sensor_id;
    int16_t temperature;
    uint32_t timestamp;
    uint8_t flags;
} SensorPacket_t;
```
**Yêu cầu**:
1. Vẽ memory layout của struct trên cho MCU 32-bit (biết rằng compiler thêm padding)
2. Tính tổng kích thước thực tế của struct sau khi padding
3. Sắp xếp lại các trường để tối ưu memory (không có padding) - giải thích nguyên tắc sắp xếp
4. Tính RAM tiết kiệm được khi có 10,000 packets

### Exercise 3: Buffer Overflow Trong ISR Và Ring Buffer
> [!TIP]
> **Hướng dẫn tự học:** Bài tập này liên hệ mật thiết với kịch bản chống tràn đệm ngắt tại **Chương 03 (Mảng & Con trỏ)** và cấu trúc Queue tại **Chương 09 (Cấu trúc dữ liệu nhúng)**. Hãy tham khảo [03_Arrays_Pointers_Reference/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) (Case 1) và [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 2, 7).

**Ngữ cảnh**: UART ISR nhận dữ liệu từ cảm biến tốc độ cao 115200 baud.
```c
uint8_t rx_buffer[64];
volatile uint16_t rx_head = 0;

// ISR
void UART_RX_IRQHandler() {
    rx_buffer[rx_head] = UART->DR;
    rx_head++;
}
```
**Yêu cầu**:
1. Giải thích vấn đề race condition khi `rx_head` được đọc/ghi đồng thời bởi ISR và main loop
2. Viết lại code sử dụng `volatile` đúng cách và thêm boundary check
3. Thiết kế Ring Buffer với 2 indices (head/tail) để tránh overflow
4. Viết hàm `Pop()` để lấy dữ liệu an toàn từ Ring Buffer

### Exercise 4: Sắp Xếp Quy Trình Khởi Tạo MCU (Đúng Thứ Tự)
> [!TIP]
> **Hướng dẫn tự học:** Trình tự khởi động là kiến thức cốt lõi của **Chương 02 (Memory Layout & Compile)**. Hãy xem kỹ và phân tích luồng di chuyển dữ liệu lúc Boot tại [02_MemoryLayout_Compile/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) (Case 10).

**Ngữ cảnh**: Thứ tự khởi tạo các subsystem ảnh hưởng đến stability của hệ thống.
**Yêu cầu**: Sắp xếp các bước sau đúng thứ tự (từ 1 đến 8):
1. Configure PLL and clock division
2. Copy .data init values from Flash to RAM
3. Zero-out .bss section
4. Call static constructors (C++)
5. Initialize GPIO pins
6. Configure UART peripheral
7. Enable interrupts
8. Call main()

### Exercise 5: Sắp Xếp Các Lớp Phần Mềm (Layer Architecture)
> [!TIP]
> **Hướng dẫn tự học:** Thiết kế kiến trúc phân lớp là một tư duy cao cấp được củng cố xuyên suốt khóa học, đặc biệt tại **Chương 02 (Memory Map)** và **Chương 13 (Design Patterns)**. Hãy tham khảo mô hình kiến trúc AUTOSAR và HAL tại [13_DesignPatterns/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) (Case 1).

**Ngữ cảnh**: Firmware máy in có các layer: Hardware → HAL → Drivers → Application
**Yêu cầu**: Sắp xếp các thành phần sau vào đúng layer (từ thấp nhất đến cao nhất):
- `HAL_UART_WriteChar()`
- `printf("Temperature: %d", temp)`
- `DMA_Channel3->CCR`
- `Motor_Start(AXIS_X)`
- `GCode_Parser()` → nhận "G1 X100 Y50"
- `TIM2->CNT register`
- `I2C1->DR register` (Data Register)
- `PrinterJobManager::execute()`

---

## Bài Tập Sắp Xếp (Ordering Exercises)

### Exercise 6: Sắp Xếp Quy Trình Xử Lý Ngắt UART (ISR Flow)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này, hãy ôn tập kỹ cơ chế ngắt I/O và debug ngắt tại **Chương 03 (Con trỏ volatile)** và **Chương 05 (File I/O & Debugging)**. Xem [03_Arrays_Pointers_Reference/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) (Case 1) và [05_FileIO_Debugging/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) (Case 1).

**Ngữ cảnh**: Khi có byte đến qua UART, các bước xử lý phải đúng thứ tự.
**Yêu cầu**: Sắp xếp thứ tự đúng (A→F):
- A. CPU nhảy đến UART ISR vector table
- B. Đọc data từ UART Data Register
- C. Kiểm tra ring buffer có chỗ trống không
- D. Lưu byte vào ring buffer tại vị trí tail
- E. Clear UART interrupt flag
- F. Return from interrupt, main loop tiếp tục

### Exercise 7: Sắp Xếp Quy Trình Biên Dịch (Compile Pipeline)
> [!TIP]
> **Hướng dẫn tự học:** Hãy ôn tập chi tiết 4 giai đoạn biên dịch và cách Linker thu thập code chết tại **Chương 02 (Memory Layout & Compile)**. Tham khảo [02_MemoryLayout_Compile/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) (Case 7).

**Yêu cầu**: Sắp xếp các giai đoạn biên dịch đúng thứ tự:
1. Linker combines .o files using linker script
2. Compiler translates .c to .s (Assembly)
3. Preprocessor expands #include and #define
4. Assembler converts .s to .o (machine code)
5. Loader burns .bin to Flash memory
6. Startup code copies .data init values to RAM