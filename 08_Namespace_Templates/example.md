# Namespace & Templates - 10 Case Studies (Thực tế Embedded / IoT / Robotics)

## Case 1: Xung đột tên thư viện (Name Collision)
**Ngữ cảnh**: Firmware IoT sử dụng thư viện `LwIP` cho Ethernet và thư viện `ESP_Wifi` cho Wifi. Cả 2 thư viện C này đều định nghĩa một cấu trúc tên là `struct netif`.
**Vấn đề**: Khi Include cả 2 thư viện vào cùng một file C++, trình biên dịch sẽ báo lỗi "Redefinition of 'netif'".
**Giải pháp**: Bọc các Header C bằng Namespace (Nếu có thể sửa mã nguồn) hoặc dùng kỹ thuật Wrapper/PIMPL (Pointer to Implementation) để ẩn các định nghĩa C này đi. Trong code C++ tự viết, hãy luôn đặt trong Namespace (Ví dụ: `namespace IoT_Gateway { ... }`) để tránh lặp lại lỗi này.

## Case 2: Anonymous Namespace Ẩn Giấu Biến Cục Bộ
**Ngữ cảnh**: Bạn viết file `motor_control.cpp`. Có một số biến trạng thái bạn không muốn bất kỳ file nào khác (dù dùng `extern`) truy cập được.
**Bad Practice**: `int motor_state = 0;` (Biến toàn cục, file khác có thể `extern int motor_state;` và sửa lén).
**Good Practice**:
```cpp
// motor_control.cpp
namespace {
    int motor_state = 0; // Tương đương với 'static int motor_state = 0;' trong C
}
```

## Case 3: Tránh Vi Phạm "Using Namespace"
**Ngữ cảnh**: File `Utils.h` khai báo `using namespace std;`.
**Vấn đề**: File `main.cpp` include `Utils.h` và thư viện `<algorithm>`. Trong `<algorithm>` có sẵn hàm `std::max`. Trong `main.cpp` bạn lại vô tình định nghĩa một hàm tên là `max`. Do `using namespace std;` làm tràn scope, trình biên dịch sẽ báo lỗi Ambiguous (Mơ hồ không biết dùng `max` nào).
**MISRA Rule**: Cấm hoàn toàn `using namespace` trong file `.h`. Trong file `.cpp`, chỉ dùng bên trong scope của hàm nếu cần thiết.

## Case 4: Template Hạn Chế Trùng Lặp Code
**Ngữ cảnh**: Cần viết hàm Clamp (Giới hạn giá trị trong khoảng Min-Max) cho Motor PWM (`uint8_t`), cho Góc Servo (`int16_t`), và Nhiệt độ (`float`).
**Bad Practice**: Viết 3 hàm `clamp_u8`, `clamp_i16`, `clamp_f`.
**Good Practice (Template)**:
```cpp
template <typename T>
inline T clamp(T val, T min_val, T max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}
```

## Case 5: Code Bloat Từ Template (Phình to ROM)
**Ngữ cảnh**: Sử dụng `std::vector<T>` hoặc viết Template Class quá phức tạp trong MCU chỉ có 32KB Flash.
**Vấn đề**: Nếu bạn gọi `clamp<uint8_t>`, `clamp<int8_t>`, `clamp<uint16_t>`, `clamp<int16_t>`, `clamp<uint32_t>`, `clamp<int32_t>`, `clamp<float>`, `clamp<double>`. Trình biên dịch sẽ âm thầm tạo ra **8 hàm riêng biệt** chiếm dung lượng ROM.
**Kinh nghiệm**: Chỉ dùng Template khi số lượng kiểu dữ liệu (Types) thực tế được gọi là rất ít (khoảng 2-3 kiểu). Nếu không, hãy ép kiểu (Casting) về chung một kiểu lớn nhất (như `int32_t`) và xử lý bằng hàm thông thường.

## Case 6: Generic RingBuffer (Hàng đợi vòng)
**Ngữ cảnh**: Bạn cần RingBuffer cho UART (Lưu `uint8_t`), RingBuffer cho CAN Bus (Lưu struct `CanFrame_t`), và RingBuffer cho Log (Lưu con trỏ `char*`).
**Giải pháp cực mạnh**: 
```cpp
template <typename T, uint16_t SIZE>
class RingBuffer {
private:
    T buffer_[SIZE];
    uint16_t head_ = 0;
    uint16_t tail_ = 0;
public:
    bool push(const T& item) { /* ... */ }
    bool pop(T& item) { /* ... */ }
};

// Khởi tạo không tốn overhead cấp phát động (Heap)
RingBuffer<uint8_t, 256> uart_rx_buffer;
RingBuffer<CanFrame_t, 16> can_tx_buffer;
```

## Case 7: Lấy số phần tử mảng bằng Template (Thay thế C Macro)
**Ngữ cảnh**: Trong C, ta hay dùng `#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))`.
**Vấn đề**: Nếu truyền một con trỏ (`uint8_t* ptr`) vào Macro này, nó sẽ âm thầm tính sai bét (Lỗi rùng rợn!).
**Good Practice C++**:
```cpp
// Template này sẽ BÁO LỖI BIÊN DỊCH nếu bạn truyền vào con trỏ! An toàn 100%.
template <typename T, size_t N>
constexpr size_t array_size(T (&)[N]) {
    return N;
}
```

## Case 8: `static_assert` bảo vệ tính toàn vẹn phần cứng
**Ngữ cảnh**: Giao thức truyền thông yêu cầu Struct Payload phải nhỏ hơn hoặc bằng 64 bytes để vừa vặn bộ đệm RF (Radio Frequency).
**Practice**:
```cpp
struct RFPayload {
    uint32_t id;
    float data[10];
    uint8_t flags;
};

// Đảm bảo không ai thêm trường mới làm tràn bộ đệm phần cứng!
static_assert(sizeof(RFPayload) <= 64, "RFPayload is too large for Hardware Buffer!");
```

## Case 9: Template Metaprogramming Để Đổi Đơn Vị (Unit Conversion)
**Ngữ cảnh**: System Clock 72MHz. Cần tính toán số "Ticks" cho Timer để tạo Delay 1ms.
**Bad Practice**: `uint32_t ticks = 1 * 72000;` (Nhập số thủ công dễ sai sót).
**Good Practice**: 
```cpp
// Hàm constexpr sẽ được tính toán ngay trên PC lúc Build code.
constexpr uint32_t ms_to_ticks(uint32_t ms) {
    return ms * (72000000 / 1000); 
}

// Lúc chạy trên MCU, giá trị đã là hằng số tính sẵn. Không tốn 1 nano giây nào!
void delay_1ms() { start_timer(ms_to_ticks(1)); }
```

## Case 10: State Machine Compile-Time
**Ngữ cảnh**: Điều khiển các State của máy in (Khởi động -> Chờ -> In -> Lỗi).
Với hệ thống cực kỳ giới hạn bộ nhớ, việc dùng Đa hình động (Virtual Class) cho mỗi State tốn vtable. Người ta dùng `Variant` (C++17) hoặc `Template` để giải quyết việc chuyển trạng thái ngay trong thời gian biên dịch, tuy nhiên kỹ thuật này khá nâng cao và khó Debug.
**Lời khuyên Nhúng**: Trong 90% trường hợp, `switch-case` thông thường kết hợp với Enum Class là đủ an toàn và dễ Debug hơn rất nhiều so với nhồi nhét Template Metaprogramming vào State Machine.
