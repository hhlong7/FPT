# Data Structures & Algorithms - 10 Case Studies (Thực tế Embedded / IoT / Robotics)

## Case 1: Tối Ưu Phép Chia Nhờ Bitwise (Power of 2)
**Ngữ cảnh**: Cần tính chỉ số của phần tử cuối cùng trong một mảng đệm khi con trỏ Index xoay vòng (Ring Buffer Wrap-around).
**Bad Practice**: `next_index = (current_index + 1) % 100;` (MCU ARM Cortex-M0 không có bộ chia phần cứng, phép chia `%` tốn hàng chục chu kỳ CPU).
**Good Practice**: Kích thước mảng chọn là 128 (Lũy thừa của 2).
`next_index = (current_index + 1) & 127;`
**Giải thích**: Phép toán AND (`&`) chỉ mất 1 chu kỳ máy (CPU Cycle). Nhanh hơn phép Modulo `%` khoảng 20-50 lần.

## Case 2: Static Array (Mảng Tĩnh) vs C++ `std::vector`
**Ngữ cảnh**: Cần lưu trữ một danh sách các tọa độ điểm cần in (Waypoints) trong hệ thống máy in 3D.
**Bad Practice**: Sử dụng `std::vector<Point>` vì kích thước tự do thay đổi. Khi in tới các chi tiết phức tạp, `std::vector` gọi Cấp phát động (`new`), gây đứt quãng thời gian thực (Jitter), đầu in bị khựng lại tạo thành bọng nhựa.
**Good Practice**: Tạo `class StaticVector<Point, MAX_POINTS>` dùng mảng tĩnh bên trong. Hoặc sử dụng mảng cố định `Point waypoints[100]` và một biến `uint8_t count`.

## Case 3: Thuật Toán Lọc EMA (Exponential Moving Average)
**Ngữ cảnh**: Cảm biến Nhiệt độ bị nhiễu do dòng khởi động của Motor rò rỉ vào đường nguồn.
**Thuật toán EMA Code Mẫu**:
```c
uint16_t filtered_temp = 0;
// Gọi hàm này 100 lần mỗi giây
void update_temp(uint16_t raw_adc) {
    // 90% dựa trên lịch sử, 10% cập nhật mới. Không dùng số Float!
    filtered_temp = (filtered_temp * 9 + raw_adc * 1) / 10; 
}
```
**Lợi ích**: Cực kỳ tiết kiệm RAM vì chỉ cần 1 biến lưu trạng thái `filtered_temp` (không cần mảng lưu lịch sử như Moving Average) và siêu tốc độ vì chỉ dùng phép nhân chia số nguyên.

## Case 4: Binary Search Trên Bảng Flash ROM
**Ngữ cảnh**: Giao tiếp CAN Bus lỗi. Có 500 mã lỗi (Error Codes) và cần tìm thông điệp chuỗi (String Message) tương ứng để báo lên Màn hình HMI.
**Bad Practice**: Dùng vòng lặp `for` (Linear Search) duyệt qua mảng 500 phần tử tốn nhiều thời gian.
**Good Practice**: Mảng mã lỗi được sắp xếp sẵn lúc Compile. Viết thuật toán Binary Search (`O(log N)`). Tìm kiếm trong 500 phần tử chỉ mất tối đa 9 vòng lặp.
```cpp
// Dữ liệu tĩnh trên ROM
const ErrorMap error_table[500] = { {0x10, "Volt Low"}, {0x15, "Overheat"} /*...*/ };
```

## Case 5: Kỹ Thuật Bitset / Bit Flags Thay Vì Mảng Bool
**Ngữ cảnh**: Quản lý trạng thái hoàn thành của 64 cảm biến tự kiểm tra (Self-test) trên Robot.
**Bad Practice**: `bool sensor_ready[64];` -> Tiêu tốn 64 bytes RAM.
**Good Practice**: `uint64_t sensor_ready_flags = 0;` -> Chỉ tốn 8 bytes RAM.
**Thuật toán truy cập**:
```c
void set_ready(uint8_t sensor_id) { sensor_ready_flags |= (1ULL << sensor_id); }
bool is_ready(uint8_t sensor_id) { return (sensor_ready_flags & (1ULL << sensor_id)); }
```

## Case 6: Memory Pool Allocator (Cấp Phát Bộ Nhớ Khối Chặt)
**Ngữ cảnh**: Xử lý gói tin mạng IoT (Network Packets). Các gói tin đến và đi liên tục. Bạn không thể dùng `malloc/free`.
**Giải Pháp (Mẫu thiết kế)**: Tạo một **Memory Pool** (Hồ chứa bộ nhớ). Bạn tạo sẵn một mảng tĩnh chứa 10 bộ đệm lớn (Mỗi bộ 256 byte). Sử dụng cấu trúc Dánh sách liên kết (Linked List) để quản lý bộ đệm nào đang Trống (Free) và bộ đệm nào đang Bận (Used). Hàm cấp phát sẽ lấy O(1) buffer trống ra mà không làm phân mảnh RAM. 

## Case 7: Ring Buffer Để Tách Ghép Nhiệm Vụ (Producer-Consumer)
**Ngữ cảnh**: Cảm biến đo nhịp tim gửi dữ liệu qua UART 1000 mẫu/giây. Thuật toán phân tích trên Main Loop mất 5ms để chạy.
**Vấn đề**: ISR nhận UART có độ ưu tiên cao sẽ liên tục ghi đè dữ liệu chưa kịp xử lý.
**Giải pháp thuật toán**: 
1. Mảng tĩnh `rx_ring_buffer[2048]`.
2. ISR (Producer) ghi vào vị trí `Head`.
3. Main Loop (Consumer) đọc từ vị trí `Tail`.
Thiết kế Data Structure này giải quyết xung đột Thread-Safety nếu Head và Tail được cập nhật cẩn thận (Atomic).

## Case 8: Lookup Table (LUT) Thay Cho `sin()` / `cos()`
**Ngữ cảnh**: Thuật toán Tính toán tọa độ chân Robot (Inverse Kinematics).
**Bad Practice**: Gọi hàm `#include <math.h>` và dùng `float x = radius * cos(angle);`. Quá chậm!
**Good Practice**:
```c
// Bảng tính sẵn từ 0 đến 359 độ (đã nhân lên 1000 để dùng số nguyên)
const int16_t COS_TABLE_1000[360] = { 1000, 999, 999, 998, /*...*/ };

int32_t x_mm = (radius_mm * COS_TABLE_1000[angle_deg]) / 1000;
```
Tránh hoàn toàn thư viện `math.h` và Floating-Point math. Tốc độ tăng vài chục lần.

## Case 9: Double Buffering Cho Thuật Toán Cập Nhật Màn Hình (Display)
**Ngữ cảnh**: Quét tia sáng cho đầu in hoặc cập nhật màn hình TFT LCD của thiết bị IoT. Màn hình bị "xé hình" (Tearing) do MCU vừa vẽ vừa xuất tín hiệu (Send to Display).
**CTDL**: Hai mảng Framebuffer (A và B).
**Thuật toán**: 
1. CPU vẽ đồ họa vào Framebuffer A.
2. Hardware DMA lấy dữ liệu từ Framebuffer B xuất ra màn hình (chạy nền).
3. Đổi con trỏ tráo đổi vị trí A và B (Pointer Swapping). Tránh việc sao chép mảng khổng lồ.

## Case 10: Xử Lý Ngắt (Interrupt) Bằng Queued State Machine
**Ngữ cảnh**: Có nút bấm khẩn cấp (E-Stop). Bấm nút gọi ISR.
**Vấn đề**: Không được phép xử lý logic dừng động cơ nặng nề ngay bên trong ISR. 
**Giải pháp Queue**:
1. Trong ISR, chỉ lấy Time-stamp và loại Event đưa vào một Event Queue (Hàng đợi sự kiện - Cấu trúc dữ liệu).
2. Main Loop lấy Event ra từ Queue và gọi Hàm xử lý của State Machine tương ứng. Đảm bảo ISR luôn ngắn và trả CPU về rất nhanh.
