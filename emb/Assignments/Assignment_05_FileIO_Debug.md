C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment05> TYPE: <Embedded_C> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 5: File I/O & Debugging On Bare-Metal

### Exercise 1: Lỗi Giao Tiếp Nghẽn (Blocking I/O) Trên UART
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (The Blocking printf Bug) và Case 6 (Watchdogs Triggered by I/O)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) của chương.

**Ngữ cảnh**: Cảm biến bụi PM2.5 liên tục bắn dữ liệu môi trường qua cổng Serial (UART) về Gateway với tốc độ 9600 baudrate.
Trong hàm ngắt (ISR) nhận dữ liệu, kỹ sư in log ra bằng `printf` để theo dõi.
```c
// Hàm này được CPU gọi tự động mỗi khi phần cứng UART nhận 1 ký tự
void UART_RX_Interrupt() {
    char data = UART_ReadData();
    // In log ngay lập tức để debug (Bị Redirect qua 1 cổng UART khác)
    printf("Nhan duoc: %c\n", data); 
}
```
**Vấn đề**: Ở tốc độ 9600 bps, mất khoảng `1 mili-giây` để in được 1 ký tự (10 bit). Hàm `printf` sẽ in chuỗi "Nhan duoc: X\n" dài khoảng 15 ký tự -> Mất 15 mili-giây CPU bị kẹt cứng (block) bên trong Ngắt! Trong thời gian 15ms đó, cảm biến liên tục gửi thêm dữ liệu tới, nhưng CPU đã bỏ lỡ (Drop data) và làm tràn Buffer phần cứng.
**Yêu cầu**:
1. Phân tích tác hại tàn khốc của việc dùng `printf` bên trong Hàm xử lý Ngắt (ISR) trên hệ nhúng.
2. Vẽ sơ đồ luồng (hoặc viết mã giả C) mô tả cách khắc phục: Làm sao để ISR vẫn tiếp nhận đủ ký tự mà quá trình Debug/In Log được dời (Defer) ra Main Loop xử lý khi CPU rảnh rỗi.

### Exercise 2: Mất Dữ Liệu Flash Do Power Loss (FatFs & SD Card)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Writing Logs to SD Card) và Case 10 (Non-Volatile Memory Wear Leveling)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) của chương.

**Ngữ cảnh**: Bạn thiết kế tính năng hộp đen (Blackbox) cho Robot in 3D. Mỗi phút, Robot sẽ ghi tọa độ X,Y,Z hiện tại vào file `log.txt` trên Thẻ nhớ SD (Sử dụng thư viện FatFs).
```c
f_open(&file, "log.txt", FA_OPEN_APPEND | FA_WRITE);
f_printf(&file, "Pos: %d,%d,%d\n", x, y, z);
// Code kết thúc hàm tại đây. Thẻ nhớ được mở liên tục trong 1 ngày để tốc độ ghi cao.
```
**Vấn đề**: Khi Robot đột ngột mất điện (Power loss). Bạn rút thẻ SD cắm vào máy tính, file `log.txt` hoàn toàn trống rỗng (Dung lượng 0 KB).
**Yêu cầu**:
1. Giải thích cơ chế Caching (Vùng đệm RAM) của Thư viện File System (FatFs/LittleFS) khi gọi lệnh ghi file. Tại sao dữ liệu chưa thực sự được ghi xuống chíp nhớ Flash vật lý?
2. Bổ sung hàm nào của FatFs (hoặc chuẩn C tiêu chuẩn) để ép Hệ thống xả đệm (Flush/Sync) dữ liệu vật lý xuống thẻ sau mỗi lần ghi, chống thất thoát dữ liệu do cúp điện? Nêu nhược điểm (Trade-off) của việc xả đệm liên tục này.

### Exercise 3: Vượt Qua Phụ Thuộc Của `<stdio.h>`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (Retargeting C++ std::cout) và Case 9 (Using CoreSight ITM/SWO for Logging)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) của chương.

**Ngữ cảnh**: Team quyết định loại bỏ hoàn toàn mảng thư viện chuẩn của C để tiết kiệm 30KB ROM. Không dùng `printf`, không dùng `sprintf`.
Bạn cần xuất một biến đếm số nguyên (Integer counter) ra màn hình Debug UART.
**Yêu cầu**:
Viết một hàm C nhỏ gọn `void UART_Print_Int(int32_t value)` biến đổi số nguyên thành chuỗi ASCII và gọi hàm `Hardware_SendChar(char c)` để gửi.
- Xử lý được số âm.
- Xử lý được chữ số 0.
- Không dùng phép chia thực (Float) và thư viện `<string.h>`, `<stdio.h>`.

### Exercise 4: Tự Tạo Hệ Thống Bẫy Lỗi `assert()` Bảo Vệ Sản Xuất
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 8 (Asserts in Production)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) của chương.

**Ngữ cảnh**: Hàm điều khiển Tốc độ quạt nhận tham số % từ 0 đến 100.
Bạn muốn bẫy lỗi lập trình viên truyền số sai (VD: 150) bằng lệnh `assert()`. Nhưng lệnh `assert()` của C-chuẩn sẽ Dừng Cứng máy in ngay lập tức. Trên thực địa (máy bán cho khách hàng), treo cứng là không thể chấp nhận được.
**Yêu cầu**:
1. Viết một C-Macro `CUSTOM_ASSERT(condition)` hoạt động như sau:
   - Nếu ở Cấp độ Debug (`#define DEBUG 1`): Khi điều kiện Sai, hệ thống sẽ in ra Tên File `__FILE__`, Số dòng `__LINE__` bị lỗi qua UART và `while(1);` khóa máy.
   - Nếu ở Cấp độ Release xuất xưởng (`#define DEBUG 0`): Macro `CUSTOM_ASSERT` sẽ bay màu (Trở thành khoảng trắng), không tốn ROM, không làm sập máy khách hàng. Tính năng an toàn (Fail-safe) khác của Hệ thống sẽ tự đưa số 150 về lại 100 sau.

### Exercise 5: Debug Cứng (Hardware Debugging) Và Watchpoints
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 4 (Debugging a Hard Fault), Case 5 (GPIO Profiling) và Case 7 (The Heisenbug)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/05_FileIO_Debugging/example.md) của chương.

**Ngữ cảnh**: Biến trạng thái máy in `uint8_t global_state` bỗng nhiên bị nhảy thành một số rác (VD: `0xFF`) nhưng bạn không biết dòng code nào, ở file nào, hàm nào đã ghi đè lên nó. Firmware có hơn 50,000 dòng code C.
**Yêu cầu**:
Sử dụng công cụ JTAG/SWD Debugger (ST-Link / J-Link). Hãy trình bày (dạng lý thuyết/bước thao tác) kỹ thuật gỡ lỗi **Data Watchpoint** (Điểm dừng theo dữ liệu) để "bắt quả tang" đoạn code sát nhân kia mà không cần phải chạy từng dòng (Step-over) toàn bộ 50,000 dòng code.
Trình bày thêm sự khác biệt giữa Data Watchpoint và Breakpoint thông thường.
