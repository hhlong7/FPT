# Arrays, Pointers & Reference - 10 Case Studies (Thực tế Embedded / IoT / Robotics)

## Case 1: UART Receive Buffer Bounds Checking (Kiểm tra giới hạn mảng)
**Ngữ cảnh**: Nối thêm các byte nhận được từ ngắt UART (ISR) vào một mảng đệm (buffer).
**Code lỗi (Bad Practice)**:
```c
uint8_t rx_buf[64];
uint8_t index = 0;

void UART_ISR() { 
    rx_buf[index++] = UDR0; // Nguy hiểm! Sẽ ghi đè bộ nhớ khi nhận byte thứ 65!
}
```
**Code chuẩn (Good Practice - Chuẩn MISRA)**:
```c
uint8_t rx_buf[64];
uint8_t index = 0;

void UART_ISR() { 
    if (index < sizeof(rx_buf)) { 
        rx_buf[index++] = UDR0; 
    } else {
        // Xử lý lỗi tràn buffer: Drop data hoặc set cờ báo lỗi (Overflow Flag)
    }
}
```
**Giải thích Code Mẫu**: 
Biến `sizeof(rx_buf)` trả về kích thước thực tế của mảng (64 bytes). Phép kiểm tra `index < sizeof(rx_buf)` đảm bảo ta không bao giờ ghi dữ liệu ra ngoài phạm vi 0-63. Nếu không có kiểm tra này, khi nhận byte thứ 65, hệ thống sẽ ghi vào biến khác nằm kế tiếp `rx_buf` trong RAM, gây ra lỗi logic cực kỳ khó debug (Hard Fault hoặc hệ thống chạy sai).

---

## Case 2: C vs C++ Trong Việc Quản Lý Mảng (So sánh chi tiết)
**Ngữ cảnh**: Cần truyền một mảng chứa dữ liệu cảm biến vào hàm xử lý.

**Trong C (Raw Array)**:
```c
void process_sensor_data(uint16_t* data, uint8_t length) {
    for(uint8_t i = 0; i < length; i++) {
        // Xử lý data[i]
    }
}
// Gọi hàm:
uint16_t sensor_readings[10];
process_sensor_data(sensor_readings, 10); // Phải truyền thủ công kích thước
```
*Nhược điểm của C*: Mảng khi truyền vào hàm bị "decay" (suy biến) thành con trỏ `uint16_t*`. Hàm hoàn toàn không biết mảng dài bao nhiêu, nên lập trình viên bắt buộc phải truyền thêm tham số `length`. Nếu truyền sai `length`, sẽ bị lỗi tràn mảng.

**Trong C++ (std::array & std::span)**:
```cpp
#include <array>
#include <span>

// Sử dụng std::span (C++20) cho phép truyền mảng an toàn không cần thêm biến length
void process_sensor_data(std::span<uint16_t> data) {
    for(auto val : data) { // Vòng lặp range-based cực kỳ an toàn
        // Xử lý val
    }
}
// Gọi hàm:
std::array<uint16_t, 10> sensor_readings;
process_sensor_data(sensor_readings); // Kích thước tự động được nội suy!
```
*Ưu điểm C++*: `std::span` (hoặc truyền tham chiếu `std::array&`) giữ nguyên được thông tin về độ dài của mảng, tự động phòng tránh các lỗi out-of-bounds. Đây là một điểm cộng rất lớn của C++ nhúng so với C truyền thống khi tuân thủ ISO 26262.

---

## Case 3: Double Buffering cho DMA (Quản lý Ownership)
**Ngữ cảnh**: Kênh DMA chuyển 1024 mẫu ADC liên tục vào RAM (Robotics/Motor Control).
**Giải pháp thực tế**: 
Sử dụng 2 mảng đệm (`buffer_A` và `buffer_B`).
- Khi DMA đang tự động ghi vào `buffer_A` (Ownership thuộc về Hardware).
- CPU (Task) được phép đọc và xử lý `buffer_B` (Ownership thuộc về Software).
- Khi DMA ghi xong A, sẽ sinh ra ngắt. Lúc này đảo ngược quyền sở hữu: DMA ghi vào B, CPU xử lý A. Cơ chế này đảm bảo CPU không bao giờ đọc phải dữ liệu rác đang bị DMA ghi dở dang.

---

## Case 4: Truyền Struct Cấu Hình Bằng Tham Chiếu (Reference)
**Ngữ cảnh**: Struct cấu hình máy in (PrinterConfig) nặng khoảng 200 bytes.
**Bad Practice**: `void apply_config(PrinterConfig cfg)`
-> Hàm này sẽ copy toàn bộ 200 bytes vào Stack. Cực kỳ tốn RAM và làm chậm MCU.
**Good Practice (C++)**: `void apply_config(const PrinterConfig& cfg)`
-> Truyền tham chiếu Hằng (`const reference`). Hàm không tạo bản copy (chỉ gửi địa chỉ ngầm định), đồng thời `const` đảm bảo hàm không thể vô tình sửa đổi dữ liệu gốc.
**Giải thích Code**: So với C (phải dùng con trỏ `const PrinterConfig* cfg`), C++ Reference an toàn hơn vì tham chiếu *không bao giờ được phép NULL*, loại bỏ hoàn toàn rủi ro Null Pointer Dereference (Lỗi truy cập con trỏ rỗng).

---

## Case 5: Con Trỏ Treo (Dangling Pointer) - "Use-After-Free" của Hệ Nhúng
**Ngữ cảnh**: Hàm trả về con trỏ trỏ tới dữ liệu cục bộ.
**Bad Practice**:
```cpp
uint8_t* get_robot_status() {
    uint8_t status[5] = {1, 2, 3, 4, 5}; // Khởi tạo mảng trên Stack
    return status; // LỖI NGHIÊM TRỌNG!
}
```
**Giải thích Code**: Khi hàm `get_robot_status` kết thúc, vùng nhớ Stack chứa mảng `status` sẽ bị thu hồi và tái sử dụng cho các hàm khác. Con trỏ trả về trỏ vào vùng nhớ không còn hợp lệ. Bất kỳ ai sử dụng con trỏ này sau đó sẽ đọc ra dữ liệu rác.
**Khắc phục**: Chuyển mảng thành `static uint8_t status[5]` (đẩy lên vùng nhớ .bss/.data sống vĩnh viễn) hoặc bắt hàm gọi (caller) truyền buffer vào: `void get_robot_status(uint8_t* out_buffer, uint8_t len)`.

---

## Case 6: Phân Tích Gói Tin Mạng An Toàn (Alignment & Strict Aliasing)
**Ngữ cảnh**: Nhận được một mảng byte (Payload CAN Bus) chứa dữ liệu Float (Nhiệt độ).
**Bad Practice**: `float temp = *(float*)payload;`
-> Việc ép kiểu trực tiếp (Casting) mảng byte sang Float vi phạm quy tắc Strict Aliasing của trình biên dịch và có thể gây lỗi Hardware Alignment (Ví dụ: CPU ARM yêu cầu Float phải nằm ở địa chỉ chia hết cho 4, nếu `payload` nằm ở địa chỉ lẻ sẽ gây `UsageFault`).
**Good Practice**:
```cpp
float temp;
// Dùng memcpy để copy byte-by-byte. Trình biên dịch sẽ tự động tối ưu thành
// các lệnh Assembly an toàn và xử lý Alignment đúng cách.
memcpy(&temp, payload, sizeof(float)); 
```

---

## Case 7: Con Trỏ `volatile` Cho Driver Ngoại Vi
**Ngữ cảnh**: Truy xuất thanh ghi GPIO của vi điều khiển.
**Code Mẫu**:
```c
// Định nghĩa kiểu dữ liệu cho ngoại vi
typedef struct {
    volatile uint32_t MODER;   // Thanh ghi chế độ (Mode Register)
    volatile uint32_t OTYPER;  // Thanh ghi loại Output
} GPIO_TypeDef;

#define GPIOA ((GPIO_TypeDef *) 0x48000000)

void set_pin_output(GPIO_TypeDef* port, uint8_t pin) {
    port->MODER |= (1 << (pin * 2)); // Cấu hình pin làm Output
}
```
**Giải thích**: Con trỏ trỏ tới địa chỉ vật lý `0x48000000`. Từ khóa `volatile` báo cho trình biên dịch: "Nội dung ở địa chỉ này bị phần cứng thay đổi, đừng bao giờ tối ưu (cache) biến này vào thanh ghi CPU".

---

## Case 8: Lookup Table (Bảng Tra Cứu) Với Mảng Const
**Ngữ cảnh**: Cần nội suy giá trị nhiệt độ từ điện trở của cảm biến NTC Thermistor. Việc dùng công thức toán học Steinhart-Hart quá chậm cho MCU.
**Good Practice**:
```c
// Lưu toàn bộ bảng tra cứu vào Flash (ROM) thay vì RAM
const uint16_t ntc_lookup_table[256] = {
    4000, 3950, 3900, // ... 256 giá trị
};

uint16_t get_temp(uint8_t adc_8bit) {
    return ntc_lookup_table[adc_8bit]; // Tra cứu mất 1 chu kỳ máy (O(1))
}
```
**Bài tập**: Làm sao để tra cứu giá trị an toàn nếu ADC là 12-bit (0-4095) nhưng bảng chỉ có 256 phần tử? (Gợi ý: Dùng phép dịch bit `adc_12bit >> 4` để giới hạn index).

---

## Case 9: Pointer Decay Khi So Sánh Chuỗi (Command Line Interface)
**Ngữ cảnh**: Viết CLI qua giao tiếp Serial.
**Lỗi phổ biến**:
```c
void process_command(char cmd[]) {
    if (cmd == "START") { // LỖI: So sánh địa chỉ con trỏ, không phải so sánh chuỗi!
        turn_on_motor();
    }
}
```
**Khắc phục**: Trong C, sử dụng `strcmp(cmd, "START") == 0`. Trong C++, nên dùng `std::string_view` (C++17) để so sánh chuỗi an toàn và cực nhanh mà không cần cấp phát động.

---

## Case 10: Tối Ưu Hóa Memory Aliasing
**Ngữ cảnh**: Viết hàm cộng 2 mảng tín hiệu (DSP cho Robot).
**Code C**:
```c
// Thêm từ khóa 'restrict' để báo cho compiler biết 'out', 'a', 'b' 
// không bao giờ trỏ tới cùng một vùng nhớ.
void vector_add(uint32_t* restrict out, const uint32_t* restrict a, const uint32_t* restrict b, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        out[i] = a[i] + b[i];
    }
}
```
**Giải thích**: Không có `restrict`, trình biên dịch sợ rằng mảng `out` có thể đè lên mảng `a` (Memory Aliasing), nên nó bắt buộc phải nạp `a[i]` và `b[i]` từ RAM vào CPU lại từ đầu ở *mỗi vòng lặp*. Có `restrict`, trình biên dịch sẽ tối ưu hóa mạnh mẽ bằng cách sử dụng thanh ghi CPU (Pipeline/SIMD) giúp tốc độ tăng gấp nhiều lần.

---

## 🛠 Bài Tập Thực Hành Chi Tiết

**Bài tập 1: Viết hàm Parse chuỗi an toàn (Chống Overflow)**
Cho một mảng `uint8_t rx_buffer[128]` chứa các gói tin Serial từ máy in. Mỗi gói tin bắt đầu bằng `0xAA`, kết thúc bằng `0x55`. Byte thứ 2 luôn là chiều dài của Payload.
- *Yêu cầu*: Viết hàm `int8_t extract_payload(const uint8_t* buffer, uint16_t max_len, uint8_t* out_payload)` thực hiện việc tách Payload. 
- *Điều kiện bắt buộc*: Phải kiểm tra an toàn `max_len`, độ dài Payload trong gói tin không được vượt quá `max_len`. Nếu thiếu Byte kết thúc `0x55`, trả về mã lỗi `-1`. Có comment giải thích chi tiết logic phòng thủ.

**Bài tập 2: Phân tích sự khác biệt C và C++ trong quản lý State**
Viết lại hàm sau từ ngôn ngữ C thuần sang chuẩn C++11 (Sử dụng References và Enum Class):
```c
// Code C
#define STATE_IDLE 0
#define STATE_RUN 1
void update_state(int* current_state) {
    if (current_state == NULL) return;
    *current_state = STATE_RUN;
}
```
*Gợi ý*: Chuyển `int*` thành tham chiếu (Reference) để bỏ qua bước check NULL. Sử dụng `enum class MachineState` để tăng tính Type-Safety (An toàn kiểu dữ liệu).

**Bài tập 3: Thiết kế Double-Buffer cho Audio I2S**
- Viết kiến trúc mã giả (Pseudo-code) minh họa cách chia 1 mảng tĩnh `uint16_t audio_buffer[2048]` thành 2 nửa (Half-Transfer và Full-Transfer) phục vụ hệ thống DMA. Ai sở hữu nửa đầu, ai sở hữu nửa sau khi xảy ra ngắt `DMA_IT_HT` (Half-Transfer Interrupt)?