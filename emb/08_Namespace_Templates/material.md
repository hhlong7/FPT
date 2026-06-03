# Chủ Đề 8: Không Gian Tên (Namespaces) & Khuôn Mẫu (Templates) - Đỉnh Cao Tối Ưu Compile-Time

## Lời Mở Đầu
Trong các dự án Nhúng/IoT phức tạp ghép từ hàng chục nguồn mã (Ví dụ: Thư viện Đám mây AWS, Thư viện Máy in Thermal, FreeRTOS, LwIP, Code Nội bộ), việc trùng tên biến hoặc tên hàm (`init()`, `config`, `state`) là không thể tránh khỏi. C xử lý việc này bằng cách đặt tên hàm rất dài (`Thermal_Printer_Init()`), còn C++ có `namespace`. 
Đặc biệt hơn, C++ có "Siêu năng lực" gọi là Templates (Khuôn mẫu) & `constexpr` giúp MCU không cần chạy code... vì máy tính (PC) đã chạy xong xuôi hết lúc Build! 

---

## 1. Namespace & Lỗi Vi Phạm Phân Vùng "Using namespace"
**Tại Sao Có Lỗi Bất Ngờ?**
Giả sử thư viện UART của bạn có hàm `max()`, và thư viện `<algorithm>` của C++ cũng có `std::max()`.
Nếu bạn viết `#include <algorithm>` và lười biếng viết `using namespace std;` lên ĐẦU FILE `.h`. 
Thảm họa xảy ra: Toàn bộ các file `.cpp` khác lỡ include file `.h` của bạn sẽ bị "lây nhiễm" cái `std::` đó. Mọi hàm `max()` trong các file kia bị xáo trộn, gây ra vô vàn lỗi `Ambiguous call`.
- **Luật C++ Nhúng**: CẤM 100% sử dụng `using namespace` trong file `.h`. Chỉ được phép dùng bên trong dấu ngoặc nhọn của một hàm cụ thể trong file `.cpp`.
- **Mẹo (Anonymous Namespace)**: Trong file `.cpp`, bọc các biến toàn cục nội bộ bằng `namespace { int my_var; }`. Lập tức biến đó bị niêm phong, không file nào khác có thể dùng `extern int my_var;` để móc nối và phá hoại được. Tuyệt chiêu đóng gói (Encapsulation) giấu mặt.

---

## 2. Templates (Cơ Bản) - Tránh Việc Viết Lại Code (Code Duplication)
Mảng tĩnh (Static Arrays) là xương sống của Hệ nhúng. Bạn muốn tạo một Hàng Đợi (Ring Buffer) dùng chung cho UART (1 Byte) và CAN Bus (Frame 8 Bytes).
Trong C, bạn phải viết 2 cái RingBuffer. C++ Template giải quyết bằng 1 lần viết:

```cpp
template <typename T, uint16_t SIZE>
class RingBuffer {
private:
    T buffer[SIZE]; // Phân bổ thẳng lên RAM tại thời điểm Compile
    uint16_t head, tail;
public:
    void push(const T& item);
    // ...
};

// Sử dụng:
RingBuffer<uint8_t, 256> uart_buffer;
RingBuffer<CanFrame, 32> can_buffer;
```
**Cạm Bẫy Phình Bộ Nhớ (Code Bloat)**:
Bản chất Template là Trình biên dịch sẽ "Copy & Paste" hộ bạn mã nguồn. Nếu bạn dùng 10 kiểu dữ liệu khác nhau cho cái Buffer này, trình biên dịch sinh ra 10 đoạn mã Assembly khác nhau trên Flash ROM.
- **Lời khuyên nhúng**: Dùng Template cho Khối dữ liệu (Containers) rất tốt. Nhưng đừng lạm dụng cho thuật toán nội bộ rườm rà.

---

## 3. Template Metaprogramming (TMP) - Bắt PC Tính Toán Thay Cho MCU
Đây là Kỹ thuật Tối cao làm cho C++ vượt trội hoàn toàn về TỐC ĐỘ so với C.
Hệ nhúng dùng MCU 48MHz (Chậm hơn PC 100 lần). Hãy bắt máy tính (i7 5GHz) chạy thuật toán trước khi nạp vào MCU!

### Kỹ Thuật Tính Toán Lập Trình Lúc Biên Dịch (`constexpr`)
```cpp
// Hàm này CÓ VẺ NHƯ đang tính toán nặng (Toán nổi Float)
constexpr uint32_t calc_pwm_ticks(float frequency_hz, uint32_t timer_clock) {
    return (uint32_t)(timer_clock / frequency_hz);
}

// KHAI BÁO BIẾN DƯỚI DẠNG HẰNG SỐ CONSTEXPR
// Trình biên dịch (GCC) sẽ lấy máy tính PC tính phép chia Float này ra số tĩnh.
constexpr uint32_t MOTOR_TICKS = calc_pwm_ticks(2000.5f, 72000000); 

// Lúc nạp vào MCU, lệnh này đã trở thành:
// set_timer(35991); -> MCU không cần biết Float hay Phép chia là gì! Nhanh Khủng Khiếp!
void init_motor() { set_timer(MOTOR_TICKS); } 
```

---

## 4. `static_assert` - Khắc Tinh Của Cấu Hình Phần Cứng Sai
Trong C truyền thống, nếu bạn cấu hình kích thước Bộ Đệm DMA nhỏ hơn Gói Tin Mạng, lỗi chỉ xuất hiện khi thiết bị đang chạy (Tràn bộ đệm, Khởi động lại).
Trong C++11, `static_assert` biến LỖI RUN-TIME thành LỖI COMPILE-TIME. Đỏ lè trên IDE ngay khi bạn bấm Build.

```cpp
struct DmaPacket { uint8_t header; uint32_t payload[10]; uint16_t crc; };

// Kích thước tối đa của thanh ghi DMA phần cứng là 64 bytes
// Nếu DmaPacket phình to do ai đó sửa code, Màn hình Build sẽ báo lỗi liền.
static_assert(sizeof(DmaPacket) <= 64, "NGUY HIỂM: Gói tin DMA vượt quá giới hạn phần cứng!");
```

---

## 5. Dùng Template Thay Thế C-Macro NGUY HIỂM (`#define`)
Trong C, tìm kích thước của một mảng tĩnh bằng Macro rất phổ biến nhưng ĐỘC HẠI.
```c
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

uint8_t buffer[100];
int size = ARRAY_SIZE(buffer); // Đúng (100)

void send_data(uint8_t* p_data) {
    // SAI BÉT CỰC NGUY HIỂM! p_data lúc này là Con trỏ (4 bytes), không còn là mảng.
    // Kết quả trả về = 4 / 1 = 4. Thuật toán sau đó sẽ đếm sai.
    int size2 = ARRAY_SIZE(p_data); 
}
```

**Khắc phục Triệt Để Bằng C++ Template:**
```cpp
// Template này bắt buộc đối số truyền vào phải là một Mảng cố định (tham chiếu mảng)
template <typename T, size_t N>
constexpr size_t get_array_size(T (&)[N]) {
    return N;
}

// Khi gọi get_array_size(p_data) ở trên, Compiler sẽ báo LỖI CÚ PHÁP ngay! 
// "Không thể truyền con trỏ vào hàm yêu cầu Mảng". Bảo vệ hoàn hảo!
```
