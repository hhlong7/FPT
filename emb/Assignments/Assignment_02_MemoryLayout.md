C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment02> TYPE: <Embedded_C> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 2: Memory Layout & Compile Process

### Exercise 1: Phân Tích Bộ Nhớ (BSS, DATA, RODATA)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (Saving RAM with const Data), Case 4 (Zero-Initialized vs Explicit-Initialized) và Case 10 (Watchdog Timer and Startup Time)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) của chương.

**Ngữ cảnh**: Firmware cho bộ điều khiển Robot gắp vật cản. Đoạn code dưới đây khai báo một số biến toàn cục và cục bộ.
```c
#include <stdint.h>

const uint32_t PI_x1000 = 3141;         // (1)
uint8_t robot_status = 1;               // (2)
uint8_t error_log[1024];                // (3)

void robot_move() {
    uint32_t target_position = 500;     // (4)
    static uint32_t move_count = 0;     // (5)
    // ...
}
```
**Yêu cầu**:
1. Phân loại 5 biến trên vào các phân vùng bộ nhớ tương ứng trên Vi điều khiển: `.rodata`, `.data`, `.bss`, `Stack`, `Heap`.
2. Trình bày chi tiết quá trình Khởi động (C Startup/Boot) làm thế nào để mảng `error_log` có giá trị toàn số `0` trước khi hàm `main()` được gọi.

### Exercise 2: Stack Overflow Trầm Trọng Bằng Mảng Cục Bộ
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (The Massive Local Array - Stack Overflow) và Case 9 (Avoiding Recursion)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) của chương.

**Ngữ cảnh**: Xử lý một ảnh Bitmap 128x64 pixel từ thẻ SD để in lên Màn hình OLED.
```c
void process_display_image() {
    uint8_t image_buffer[8192]; // Cấp phát 8KB mảng
    // Đọc ảnh từ thẻ SD vào mảng này...
    draw_to_oled(image_buffer);
}
```
**Vấn đề**: Con chip MCU đang dùng chỉ có tổng cộng 16KB RAM. Mỗi lần gọi hàm `process_display_image()`, máy lập tức bị sập (Crash) và đơ cứng.
**Yêu cầu**:
1. Giải thích nguyên nhân hệ thống bị sập (Phân tích cơ chế hoạt động của ngăn xếp Stack).
2. Đưa ra 2 cách giải quyết khác nhau để lưu trữ mảng này một cách an toàn mà không dùng `malloc()`. Phân tích ưu/nhược điểm của từng cách.

### Exercise 3: Thảm Họa `malloc` Trên Hệ Nhúng (Memory Fragmentation)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Memory Fragmentation -malloc)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) của chương.

**Ngữ cảnh**: Trạm Gateway IoT liên tục nhận gói tin JSON qua Wifi. Vì kích thước gói tin biến động, kỹ sư cũ viết code như sau:
```c
void parse_wifi_packet(uint16_t packet_size) {
    char* json_payload = (char*)malloc(packet_size);
    
    if (json_payload != NULL) {
        wifi_read(json_payload, packet_size);
        process_json(json_payload);
        free(json_payload);
    }
}
```
**Vấn đề**: Trong 1 ngày đầu, hệ thống chạy trơn tru. Đến ngày thứ 2, `malloc` luôn trả về `NULL` khiến hệ thống mất mạng. Mặc dù tổng dung lượng RAM trên chip vẫn báo còn trống hơn 10KB.
**Yêu cầu**:
1. Hiện tượng này gọi là gì trong quản lý bộ nhớ động? Vẽ mô hình (bằng Text) hoặc miêu tả vì sao RAM trống 10KB mà `malloc(200)` vẫn có thể thất bại.
2. Theo chuẩn MISRA C/C++, việc sử dụng Heap bị cấm ở đâu? Hãy thiết kế lại đoạn code trên sử dụng một Mảng cấp phát tĩnh (Static Allocation) với cấu trúc `Memory Pool` nhỏ gọn.

### Exercise 4: Tối Ưu Hóa Giai Đoạn Linker (Linker Scripts)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 5 (Linker Placement for DMA) và Case 7 (Unused Function Elimination)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) của chương.

**Ngữ cảnh**: Firmware của bạn khi Compile xong báo kích thước Flash là `35KB`. Con chip của bạn chỉ có `32KB` Flash.
Bạn biết chắc chắn trong dự án có một số thư viện Toán học C lớn (như tính Cosin, ma trận) được `#include` vào nhưng chưa bao giờ được gọi. Tuy nhiên, Linker vẫn ôm toàn bộ đống hàm đó nhét vào File thực thi `.bin`.
**Yêu cầu**:
1. Trình bày cách sử dụng các Cờ biên dịch (Compiler & Linker flags) trong GCC để bật tính năng **"Dead Code Elimination"** (Loại bỏ code chết). (Gợi ý liên quan đến `ffunction-sections` và `--gc-sections`).
2. Giải thích nguyên lý hoạt động của Linker khi bật các cờ này.

### Exercise 5: Phân Tích Cảnh Báo "Naked Pointer" Của Linker
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 6 (Catching Code Size Issues - Map File) và Case 8 (The Cost of printf and Floating Point)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/02_MemoryLayout_Compile/example.md) của chương.

**Ngữ cảnh**: Bạn chia code làm nhiều file.
File `uart.c`: `uint16_t baud_rate = 9600;`
File `main.c`: Cần in cái baud rate này ra, bạn viết:
```c
extern uint8_t baud_rate; // Khai báo extern để lấy biến từ file uart.c sang
printf("Baud: %d\n", baud_rate);
```
**Vấn đề**: C Compiler không báo lỗi. Linker không báo lỗi. Nhưng lúc chạy, in ra màn hình con số `128` (Một số rác hoàn toàn sai lệch).
**Yêu cầu**:
1. Phân tích nguyên nhân lỗi dựa trên sự khác biệt về kích thước kiểu dữ liệu trong khai báo `extern`.
2. Linker của ngôn ngữ C thuần túy có lưu trữ thông tin về "Kiểu dữ liệu" (Type Information) của biến không? Nếu đây là C++, hiện tượng này có được báo lỗi lúc Compile/Link hay không? (Gợi ý: Name Mangling trong C++).
