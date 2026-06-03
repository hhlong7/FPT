C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment12> TYPE: <Embedded_Optimization> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 12: Tối Ưu Hóa Bộ Nhớ Cốt Tử & Cạm Bẫy (Optimization & Defects)

### Exercise 1: Lỗ Hổng Trầm Trọng: Padding Struct Lấy Đi 30% RAM
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Struct Padding làm rò rỉ RAM) và Case 5 (Biến Tạm Khổng Lồ Gây Tràn Stack)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) của chương.

**Ngữ cảnh**: Firmware Quản lý Nhiệt độ Đầu In 3D và Cảm Biến Trục.
Lập trình viên thiếu kinh nghiệm C khai báo Dữ liệu truyền nhận Nội bộ (Bus payload) như sau:
```c
struct SensorPayload {
    uint8_t   sensor_id;       // 1 byte
    uint64_t  timestamp_ms;    // 8 bytes
    uint16_t  temperature;     // 2 bytes
    uint32_t  error_code;      // 4 bytes
    uint8_t   is_active;       // 1 byte
};
```
Kỹ sư tạo mảng Hàng đợi Lịch sử `struct SensorPayload history[1000];` (Chứa 1000 Gói tin). MCU ARM Cortex-M4 có thanh ghi 32-bit (Căn lề bộ nhớ Alignment = 4/8 bytes).
**Yêu cầu**:
1. Vẽ biểu đồ Bộ nhớ (Memory Layout Text Diagram) cho Struct trên. Chỉ ra rõ những chỗ Trình biên dịch Nhét Byte Đệm Lề (Padding Bytes) vào.
2. Tính Tổng Kích thước THỰC TẾ của Struct `SensorPayload` và Tổng lượng RAM bị LÃNG PHÍ cho Mảng `history[1000]`.
3. Sắp xếp lại (Tối ưu hóa) các Biến trong Struct để kích thước Gói Nhỏ Nhất Có Thể (Không còn hoặc ít nhất Padding). Tổng số RAM tiết kiệm được là bao nhiêu KB?

### Exercise 2: Rơi Tự Do Thời Gian Thực Với Phép Toán Nổi (Float Division)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Phép nhân chia Float Tàn Phá Tốc Độ) và Case 6 (Quên Từ Khóa volatile Của Trạng Thái)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) của chương.

**Ngữ cảnh**: Cần tính độ dốc PID (Derivative) của Động cơ. Vi điều khiển KHÔNG CÓ lõi thực FPU phần cứng.
```c
// Chạy 5000 vòng lặp 1 giây.
float Kp = 1.55f;
float current_speed = 1200.0f;
float error = 50.0f;

float output = (error * Kp) / current_speed; // Toán Nổi (Floating-Point Math)
```
Một phép Chia số thực Không có FPU sẽ ép Trình biên dịch sinh ra một Hàm "Sotfware Emulation" khổng lồ, lấy đi Gần 1000 Chu Kỳ Máy CPU cho MỘT lần chia. Hệ thống Robot bị Chậm nhịp!
**Yêu cầu**:
Tiến hành Tối ưu hóa sang TOÁN SỐ NGUYÊN DẤU PHẨY TĨNH (Fixed-Point Integer Math).
1. Loại bỏ hoàn toàn `float`. Thay thế bằng số nguyên tỷ lệ `int32_t` (Bằng cách nhân hằng số Lên 1000 để dịch dời phần thập phân). 
2. Viết lại biểu thức `output = (error * Kp) / current_speed;` bằng phép toán số nguyên `int32_t`. (Ví dụ: `Kp_1000 = 1550`, `current_speed_1000 = 1200000`).

### Exercise 3: Ngăn Chặn Lỗi Race Condition Trong Ngắt
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (Race Condition Làm Nhảy Các Dữ Liệu)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) của chương.

**Ngữ cảnh**: Xử lý biến dữ liệu chung đa luồng bị thay đổi liên tục trong ngắt.
**Yêu cầu**:
1. Phân tích nguyên nhân xảy ra lỗi Race Condition làm nhảy sai lệch dữ liệu biến đếm.
2. Viết đoạn code tạo vùng tới hạn (Critical Section) tắt ngắt tạm thời để bảo vệ an toàn cho chuỗi lệnh cập nhật biến đa byte.

### Exercise 4: Phòng Ngừa Kẹt Vòng Lặp Bằng Watchdog & Timeout
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 4 (Treo Máy Do Quên Khởi Tạo Pointer) và Case 10 (Vòng Lặp Chờ Vô Hạn)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) của chương.

**Ngữ cảnh**: Chờ đợi phần cứng hoàn thành tác vụ ghi xóa Flash.
**Yêu cầu**:
1. Phân tích sự cố kẹt vòng lặp vô hạn `while` dẫn đến kích hoạt WDT cứng reset chip ngắt quãng.
2. Thiết kế giải pháp an toàn bằng cách cấu hình bộ đếm timeout ngắt kết nối an toàn bảo toàn trạng thái thiết bị.

### Exercise 5: Cắt Tỉa Hàm Không Sử Dụng Bằng Linker Sections & gc-sections
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 7 (Rò Rỉ Tài Nguyên Phần Cứng), Case 8 (Code Cồng Kềnh Do printf) và Case 9 (Dead-Code Chặn Tối Ưu Hóa)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/12_Optimization_CommonDefects/example.md) của chương.

**Ngữ cảnh**: Firmware Flash chỉ còn dư 2KB. Bạn mang theo Thư viện Giao diện UI Đồ Họa mã nguồn mở rất lớn (Dài 200 Hàm). Nhưng Ứng dụng thực tế của Máy in bạn chỉ sử dụng đúng 5 Hàm vẽ Chữ và Đường Kẻ trong số đó.
**Vấn đề**: Lúc bấm nút Compile. Linker lôi Toàn Bộ 200 Hàm nhét vô File `.bin`. Chip Máy In báo ĐẦY FLASH (Flash Overflow)!
**Yêu cầu**:
Việc tối ưu Cấu trúc Dự Án đòi hỏi kỹ sư Nhúng nắm Cờ Trình Biên Dịch (Compiler Flags).
- Sử dụng `-ffunction-sections -fdata-sections` trong bước Compile C. 
- Trình biên dịch làm gì đối với mỗi hàm ở bước này? (Gợi ý: Nó có cắt hàm thành từng Mảnh Block độc lập không?).
- Sử dụng `--gc-sections` (Garbage Collect Sections) ở bước Linker. Linker "Tìm Điểm Bắt Đầu (Roots/Main) -> Truy Dấu Đồ Thị Đỉnh Hàm" và thực hiện XÓA BỎ cái gì?
Trình bày nguyên lý dọn rác Linker kinh điển này.
