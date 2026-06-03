C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment08> TYPE: <Embedded_CPP> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 8: Namespaces, Templates & `constexpr`

### Exercise 1: Rủi Ro Từ Căn Bệnh `using namespace` Toàn Cục
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Xung đột tên namespace) và Case 2 (Anonymous Namespace)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/08_Namespace_Templates/example.md) của chương.

**Ngữ cảnh**: Firmware của bạn được chia thành 2 thư viện tự viết.
Thư viện điều khiển Màn hình (LCD Library). Có một hàm `void init();`
Thư viện điều khiển Mạng (Network Library). Cũng có một hàm `void init();`
Kỹ sư A viết cả hai thư viện, đều lười nên mở file Header (`.h`) và nhét tất cả vào một cục. Thậm chí dùng `using namespace std;` ở mọi file Header.
**Vấn đề**: Khi file `main.cpp` include cả hai thư viện. Trình biên dịch báo lỗi tùm lum về việc Trùng tên Hàm (Ambiguous / Name Collision).
**Yêu cầu**:
1. Thiết kế lại 2 thư viện này. Áp dụng khai báo C++ `namespace Display` và `namespace Network`.
2. Trình bày tại sao chuẩn MISRA C++ NGHIÊM CẤM tuyệt đối việc đặt dòng lệnh `using namespace std;` ở bên trong file `.h`. Sự lây nhiễm Scope (Ô nhiễm không gian tên) xảy ra như thế nào?

### Exercise 2: Anonymous Namespace (Không Không Gian Tên Ẩn Danh) Để Đóng Gói
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Anonymous Namespace) và Case 3 (Tránh Vi Phạm ODR)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/08_Namespace_Templates/example.md) của chương.

**Ngữ cảnh**: Trong file `motor_control.cpp`, bạn có một mảng 100 số nội suy PID `int pid_history[100];` và một hàm phụ trợ `int calc_avg();`.
Bạn KHÔNG muốn bất kỳ file `.cpp` nào khác trong toàn bộ dự án dùng lệnh `extern int pid_history[100];` để lén lút đọc/ghi mảng này (Phá vỡ Encapsulation).
Trong ngôn ngữ C thuần, bạn giải quyết bằng từ khóa `static`.
Trong C++, cách hiện đại là dùng Không gian tên Ẩn danh.
**Yêu cầu**:
1. Viết cấu trúc khai báo mảng và hàm trên bên trong một Anonymous Namespace (Namespace không có tên).
2. Tính chất liên kết (Linkage) của các biến này đối với Linker là gì? Tại sao nó an toàn tuyệt đối với dự án nghìn File C++?

### Exercise 3: Sức Mạnh `constexpr` (Tính Toán Lúc Biên Dịch) Thay Thế Hàm Runtime
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 4 (Template Hằng Số Biên Dịch), Case 7 (Lấy Số Phần Tử Mảng Tĩnh) và Case 8 (static_assert kiểm tra cấu hình)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/08_Namespace_Templates/example.md) của chương.

**Ngữ cảnh**: Vi điều khiển cần tính số Ticks của Timer để tạo hàm Delay 10 Microgiây (us).
Tần số của CPU là `72MHz` (72 triệu ticks mỗi giây). Số Ticks cần thiết = `(Thời gian us * Tần số MHz) / 1000000`.
**Bad Practice**:
```cpp
uint32_t calc_delay_ticks(uint32_t delay_us, uint32_t clock_hz) {
    return (delay_us * clock_hz) / 1000000;
}
void setup_timer() { Set_Timer_Limit(calc_delay_ticks(10, 72000000)); } // Bắt con chip MCU yếu ớt phải thực hiện phép nhân/chia to lớn này mỗi lần bật máy!
```
**Yêu cầu**:
1. Khai báo lại hàm `calc_delay_ticks` bằng từ khóa `constexpr`.
2. Việc sử dụng `constexpr` giúp hệ thống tiết kiệm được Chu kỳ máy (CPU Cycle) và tránh Phân rẽ nhánh (Branching) như thế nào? Việc "Máy tính PC làm thay việc cho MCU" nghĩa là sao?

### Exercise 4: Bẫy Lỗi Lúc Build Cực Đỉnh Của `static_assert`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 8 (static_assert kiểm tra cấu hình) và Case 9 (Template Metaprogramming)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/08_Namespace_Templates/example.md) của chương.

**Ngữ cảnh**: Cấu trúc Dữ liệu Đám mây (CloudPayload) của IoT Gateway phải gửi đi một gói tin UDP chính xác là 64 Bytes. Bất kỳ ai sửa Cấu trúc này làm nó Lớn hơn hoặc Nhỏ hơn 64 bytes đều khiến Data bị hỏng (Packet corrupted).
```cpp
struct CloudPayload {
    uint32_t device_id;
    float temp_data[12];
    uint8_t crc8;
    // Lỡ có một ông Kỹ sư thêm cái biến uint32_t dummy; vô đây!
};
```
**Vấn đề**: Kỹ sư sửa xong, Compile (Build) thành công xanh lè! Nạp vô máy mới phát hiện lỗi.
**Yêu cầu**:
Hãy viết một dòng lệnh C++11 `static_assert()` đặt ngay bên dưới Struct đó. Câu lệnh này có phép màu gì để BÁO ĐỎ LỖI trên màn hình IDE ngay giây phút ông kỹ sư kia vừa ấn nút Build code?

### Exercise 5: Generic RingBuffer Và Rủi Ro Phình To Code (Template Bloat)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 5 (Code Bloat từ Templates), Case 6 (Generic RingBuffer Template) và Case 10 (State Machine với kiểu dữ liệu Template)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/08_Namespace_Templates/example.md) của chương.

**Ngữ cảnh**: Cần một Hàng Đợi (Queue/Ring Buffer). 
```cpp
template <typename T, uint16_t CAPACITY>
class RingBuffer {
    T data[CAPACITY];
    //...
};
```
Bạn khởi tạo hàng loạt:
```cpp
RingBuffer<uint8_t, 100> uart_buffer;
RingBuffer<uint8_t, 120> spi_buffer;
RingBuffer<float, 100> temp_buffer;
RingBuffer<int, 50> log_buffer;
```
**Yêu cầu**:
1. Trình biên dịch C++ (Template Instantiation) sẽ thực sự tạo ra BAO NHIÊU bản sao (Bao nhiêu Class khác nhau hoàn toàn) trong File ROM/Flash của bạn? 
2. Trình bày hiện tượng Code Bloat (Phình to mã nguồn) do lạm dụng Template với quá nhiều kiểu dữ liệu. Hãy đề xuất một cách thiết kế hạn chế rủi ro này (Ví dụ: Thay vì `100`, `120`, ta gom về một con số Size chung).
