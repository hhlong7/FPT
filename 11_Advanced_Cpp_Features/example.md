# Advanced C++ Features - 10 Case Studies (Thực tế Embedded / IoT / Robotics)

## Case 1: Sử Dụng `constexpr` Cho Bảng Tra Cứu (Lookup Tables)
**Ngữ cảnh**: Cần một bảng cấu hình tốc độ cho Stepper Motor (Robot).
**Bad Practice**: Khởi tạo mảng trong Runtime, tốn thời gian lúc khởi động máy.
**Good Practice**: Dùng `constexpr` C++14/17 để tạo mảng tĩnh ngay lúc biên dịch.
```cpp
constexpr auto generate_speed_profile() {
    std::array<uint16_t, 100> profile = {};
    for (int i = 0; i < 100; ++i) {
        profile[i] = (i * i) / 2; // Tính toán phức tạp
    }
    return profile;
}

// Bảng này được nhúng thẳng vào Flash ROM, CPU không tốn 1 chu kỳ nào để tính!
constexpr auto SPEED_PROFILE = generate_speed_profile();
```

## Case 2: RAII Lock Guard Khắc Phục Lỗi Quên Mở Khóa (Deadlock)
**Ngữ cảnh**: RTOS Task cập nhật một hàng đợi chung (Queue) và dùng Mutex.
**Vấn đề**: Hàm dài, có nhiều câu lệnh `if (error) return;`. Lập trình viên quên gõ `xSemaphoreGive()` trước mỗi chữ `return`. Task khác bị kẹt vĩnh viễn (Deadlock).
**Giải pháp**:
```cpp
class MutexLock {
    SemaphoreHandle_t mutex_;
public:
    MutexLock(SemaphoreHandle_t m) : mutex_(m) { xSemaphoreTake(mutex_, portMAX_DELAY); }
    ~MutexLock() { xSemaphoreGive(mutex_); } // Tự động chạy khi hàm kết thúc (Bất kể Return hay văng Exception)
};

void update_queue() {
    MutexLock lock(my_mutex);
    if (full) return; // Rất an toàn, tự động Unlock!
    // Làm việc...
}
```

## Case 3: `enum class` Thay Cho `#define` Constants
**Ngữ cảnh**: Mã lỗi của máy in nhiệt (Thermal Printer).
**Bad Practice (C)**:
```c
#define ERR_PAPER_OUT 1
#define ERR_TEMP_HIGH 2
void handle_error(int error_code); 
handle_error(100); // Trình biên dịch không cản, dù số 100 không tồn tại!
```
**Good Practice (C++11)**:
```cpp
enum class PrintError : uint8_t {
    PAPER_OUT = 1,
    TEMP_HIGH = 2
};
void handle_error(PrintError error_code);
// handle_error(100); // LỖI BIÊN DỊCH! Rất an toàn.
```

## Case 4: Cấp Phát Tĩnh (Static Array) Cho `std::span` (C++20)
**Ngữ cảnh**: Giao tiếp UART nhận payload có độ dài thay đổi.
**Bad Practice**: Truyền cặp biến rời rạc `void parse(uint8_t* data, size_t len)`. Dễ bị mất đồng bộ giữa con trỏ và chiều dài.
**Good Practice**:
```cpp
#include <span>
void parse(std::span<const uint8_t> payload) {
    if (payload.size() < 4) return;
    // std::span bao bọc cả con trỏ và chiều dài thành 1 Object siêu nhẹ
    uint8_t header = payload[0]; 
}
```

## Case 5: `std::atomic` Thay Cho `volatile` Giữa ISR và Task
**Ngữ cảnh**: Ngắt UART báo hiệu có dữ liệu mới cho Main Loop.
**Bad Practice**: `volatile bool data_ready = false;` -> Không an toàn Thread-safe hoàn toàn nếu Main Loop thực hiện chuỗi "Đọc-rồi-Xóa". Dễ xảy ra Race Condition.
**Good Practice**: 
```cpp
#include <atomic>
std::atomic<bool> data_ready{false};

// Trong ISR
void UART_ISR() { data_ready.store(true, std::memory_order_relaxed); }

// Trong Main Loop
void loop() {
    // Atomically đọc biến và gán lại thành false trong 1 lệnh duy nhất!
    if (data_ready.exchange(false, std::memory_order_acquire)) {
        process_data();
    }
}
```

## Case 6: Move Semantics Để Tối Ưu Hóa Gói Tin (Network Packets)
**Ngữ cảnh**: Xử lý gói tin MQTT dung lượng 1024 bytes qua nhiều tầng (Layer): Hardware -> MAC -> IP -> MQTT.
**Vấn đề**: Copy gói tin 4 lần làm cạn kiệt RAM và CPU.
**Giải pháp**: Sử dụng `std::move` để chuyển quyền sở hữu (Ownership) con trỏ bộ đệm mà không sao chép dữ liệu.
```cpp
class Packet {
    uint8_t* buffer;
public:
    Packet(Packet&& other) noexcept : buffer(other.buffer) {
        other.buffer = nullptr; // Chuyển quyền sở hữu
    }
};
void send(Packet p);
// Gọi hàm: send(std::move(my_packet));
```

## Case 7: Lệnh Báo Lỗi Compile-Time (`static_assert`)
**Ngữ cảnh**: Code phụ thuộc vào một phần cứng có bộ đệm DMA 256 bytes.
**Vấn đề**: Kỹ sư khác mở rộng Struct cấu hình lên thành 300 bytes, nạp code gây lỗi Buffer Overflow bí ẩn.
**Giải pháp**: Báo lỗi từ lúc nhấn nút Build.
```cpp
struct DmaPayload { ... };
static_assert(sizeof(DmaPayload) <= 256, "DmaPayload exceeds hardware limits!");
```

## Case 8: Range-based `for` Cho Thanh Ghi
**Ngữ cảnh**: Reset hàng loạt chân GPIO trên MCU.
**Bad Practice**: Quản lý biến đếm thủ công `for(int i=0; i<10; i++)`, rủi ro gõ nhầm `<` thành `<=`.
**Good Practice**:
```cpp
std::array<uint8_t, 4> reset_pins = {PIN_1, PIN_5, PIN_7, PIN_9};
for (const auto pin : reset_pins) {
    GPIO_Clear(PORTA, pin); // Dễ đọc, an toàn tuyệt đối
}
```

## Case 9: Keyword `auto` Cho Type Inference
**Ngữ cảnh**: Trích xuất phần tử từ cấu trúc lồng nhau (Nested structs).
**Bad Practice**: Phải gõ lại toàn bộ kiểu dữ liệu siêu dài của C++.
`std::array<std::pair<int, float>, 10>::iterator it = map.begin();`
**Good Practice**:
`auto it = map.begin();` -> Nhanh, giảm gõ chữ. **Khuyến cáo**: Chỉ dùng `auto` khi kiểu dữ liệu quá rõ ràng hoặc phức tạp, không lạm dụng `auto` làm mất đi tính minh bạch của kiểu cơ bản (như `uint8_t`).

## Case 10: `noexcept` - Tối Ưu Hóa Trình Biên Dịch
**Ngữ cảnh**: Bạn có các hàm cơ bản không bao giờ văng lỗi.
**Practice**: Thêm từ khóa `noexcept`.
```cpp
uint8_t get_status() const noexcept { return status_; }
```
**Tác dụng**: C++ Compiler luôn tạo ra các đường ống (Exceptions handling overhead) ngầm đề phòng hàm có thể `throw`. Khi bạn cam kết `noexcept`, Compiler sẽ cắt bỏ toàn bộ mớ lệnh bảo vệ ngầm đó, giúp file nhị phân nhỏ hơn và chạy nhanh hơn.
