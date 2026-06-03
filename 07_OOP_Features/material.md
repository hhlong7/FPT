# Chủ Đề 7: Tính Năng OOP C++ (Kế Thừa & Đa Hình) & Sự Đánh Đổi MCU

## Lời Mở Đầu
Tiếp nối sự phức tạp của Lập trình hướng đối tượng, chủ đề này phân tích sâu hơn về việc mang Đa hình Động (Runtime Polymorphism) vào Vi điều khiển. Trong thiết kế Firmware cao cấp (Ví dụ: Kiến trúc AUTOSAR của Ô tô), việc Module A giao tiếp với Module B mà không cần biết chính xác Chip đang dùng là gì là một yêu cầu bắt buộc (Hardware Abstraction). C++ hỗ trợ điều này bằng Virtual Functions, nhưng giá phải trả về Hiệu năng (Performance) là gì?

---

## 1. Mổ Xẻ Bảng Hàm Ảo (VTable & VPtr) Dưới Lăng Kính Bộ Nhớ MCU
Khi bạn gõ từ khóa `virtual` vào một phương thức (Method) trong C++.
```cpp
class ISensor {
public:
    virtual uint16_t read() = 0; // Pure virtual (Interface)
    virtual ~ISensor() = default; // LUÔN LUÔN phải có Virtual Destructor
};
```
**Bí mật của Trình biên dịch**:
Nó sẽ âm thầm tạo ra một mảng tĩnh chứa các Con trỏ Hàm (Function Pointers) đặt trong vùng ROM/Flash. Bảng này gọi là **VTable**.
Mỗi đối tượng (Object) khi sinh ra trong RAM sẽ bị lén lút "bơm" thêm một con trỏ ẩn gọi là **vptr** trỏ vào cái VTable đó.

**Hậu Quả Trên MCU (Code Bloat & Memory Overhead)**:
1. **Lãng phí RAM**: Nếu bạn có 1000 đối tượng `Sensor` rất nhỏ (mỗi cái chứa 1 biến `id` 1 byte). Bình thường mảng 1000 cảm biến tốn 1000 Bytes RAM. Nhưng vì có `vptr` (4 Bytes trên chip ARM), mỗi đối tượng biến thành 5 Bytes (cộng thêm 3 bytes Padding = 8 Bytes). 1000 cảm biến tốn **8000 Bytes RAM**! Gấp 8 lần!
   *(Mẹo: Không bao giờ dùng Virtual trên các Dữ liệu mảng số lượng lớn).*
2. **Chi Phí Thời Gian Chạy (Execution Overhead)**: Để gọi hàm `read()`, CPU không thể nhảy thẳng đến hàm. Nó phải nạp `vptr` từ RAM -> Vào VTable ở ROM -> Tìm địa chỉ hàm `read()` thật -> Nhảy đến hàm đó. Mất thêm 3-4 lệnh máy (Assembly instructions). Trong các ngắt siêu tốc (VD: Bộ đệm DMA của ADC), độ trễ này phá hỏng Real-time.

---

## 2. RTTI (Run-Time Type Information) - Sát Thủ Bộ Nhớ
RTTI sinh ra để phục vụ cho toán tử `dynamic_cast<T>` và toán tử `typeid`.
Nó cho phép MCU "đoán" xem Đối tượng hiện tại thực sự thuộc Class nào trong lúc đang chạy (Runtime).
**Bản chất**: Trình biên dịch sẽ tự động chèn thêm RẤT NHIỀU chuỗi văn bản (Strings chứa tên các Class) và thông tin phả hệ vào vùng ROM (Flash) để lúc chạy nó có cái mà so sánh.
- Firmware của bạn tự nhiên phình to thêm hàng chục KB.
- **Tiêu chuẩn MISRA / C++ Nhúng**: BẮT BUỘC TẮT (Disable RTTI) bằng Cờ biên dịch `-fno-rtti`.
- **Nếu cần ép kiểu (Casting)**: Hãy thiết kế Hệ thống phân cấp tốt để không bao giờ phải Down-cast. Nếu thực sự cần, dùng `static_cast` (chỉ dịch trên máy tính lúc Build, không tồn tại trong MCU lúc chạy).

---

## 3. Khắc Phục Overhead Của Virtual Bằng `final` (Devirtualization)
Tính năng xuất sắc của C++11 dành cho tối ưu hóa Tốc độ.
Giả sử bạn kế thừa: `IPrinter` -> `ThermalPrinter` -> `EpsonThermalPrinter`.
Nếu bạn viết:
```cpp
class EpsonThermalPrinter final : public ThermalPrinter {
    void printLine() override { ... }
};
```
Từ khóa `final` báo cho Compiler biết: *"Đến đây là chấm dứt, không còn ai kế thừa thằng Epson này nữa đâu"*.
Compiler sẽ tự động phá hủy toàn bộ cơ chế VTable của hàm `printLine` nếu nó biết chắc đang làm việc với Epson, và tối ưu hóa hàm này thành một Lệnh Nhảy Trực Tiếp (Direct call) không tốn thêm 1 tick thời gian nào. Đây gọi là **Devirtualization**.

---

## 4. CRTP (Curiously Recurring Template Pattern) - Đa Hình Tĩnh
Làm sao để thiết kế HAL (Lớp trừu tượng) có khả năng thay đổi Class phần cứng mà Tốc độ đạt mức Max (0 Overhead, Không VTable, Compile-time Polymorphism)?
Giới nhúng sử dụng CRTP (Template lồng ngược). Kỹ thuật cực khó nhưng siêu hiệu quả.

```cpp
// Lớp Cha (Template nhận Tên Lớp Con làm tham số)
template <typename Derived>
class UARTBase {
public:
    void send_byte(uint8_t data) {
        // Ép kiểu trực tiếp lớp Cha thành lớp Con và gọi hàm. KHÔNG CẦN VTABLE!
        static_cast<Derived*>(this)->hardware_send(data);
    }
};

// Lớp Con kế thừa từ Lớp Cha (Và truyền chính nó vào)
class STM32_UART : public UARTBase<STM32_UART> {
public:
    void hardware_send(uint8_t data) {
        // Ghi trực tiếp thanh ghi (Được Inline hoàn toàn)
        USART1->DR = data;
    }
};

// Lớp Con thứ 2
class ESP32_UART : public UARTBase<ESP32_UART> { ... };
```
**Khi Nào Dùng CRTP?**
Khi bạn viết các Thư viện lõi (Core Library) cần chạy trên nhiều nền tảng (STM32, ESP32, AVR). CRTP cung cấp tính đa hình nhưng ép sự tính toán đó vào Bước Compile của Máy Tính (PC). File mã máy `.bin` xuất ra hoàn toàn không còn Dấu Vết của OOP, nó đã được tối ưu hóa như một lệnh C thuần túy!

---

## 5. Tổng Kết Đánh Giá Kiến Trúc C++ Nhúng
- **Có nên dùng Inheritance (Kế thừa)?** -> RẤT HIẾM KHI. Chỉ dùng Kế thừa Nông (1 tầng Interface) để làm HAL cho Driver Sensor, Network. Ưu tiên **Composition** (Chứa đối tượng thành phần) thay vì Kế thừa.
- **Có nên dùng Virtual?** -> CÓ, nhưng dùng trên PC (Unit Test Mocking), và dùng trong luồng Init/Config chậm rãi. KHÔNG dùng Virtual trong Hot-path (Luồng xử lý tín hiệu khắt khe Real-time).
- **Có nên dùng C++ Exceptions (Try/Catch)?** -> TUYỆT ĐỐI CẤM (`-fno-exceptions`). C++ Exception kéo theo hàng chục KB thư viện và làm thời gian thực bị sai lệch. Xử lý lỗi bằng Error Code thuần túy của C hoặc dùng cấu trúc `std::expected` (C++23) nếu Toolchain hỗ trợ.
