# OOP Features - 10 Case Studies (Thực tế Embedded / IoT / Robotics)

## Case 1: Hardware Abstraction Layer (HAL) với Interface
**Ngữ cảnh**: Firmware của máy in hỗ trợ 2 loại đầu in: Đầu in nhiệt (Thermal) điều khiển qua SPI, và Đầu in phun (Inkjet) điều khiển qua UART.
**Vấn đề**: Module `PrintManager` (Tầng Application) không nên chứa các lệnh `if (type == THERMAL) SPI_Send() else UART_Send()`. Rất khó bảo trì khi thêm loại đầu in thứ 3.
**Giải pháp**:
```cpp
// 1. Định nghĩa Interface chung
class IPrintHead {
public:
    virtual void printLine(const uint8_t* data, uint16_t len) = 0;
    virtual bool isReady() const = 0;
    virtual ~IPrintHead() = default;
};

// 2. Các class con triển khai chi tiết phần cứng
class ThermalHead : public IPrintHead {
public:
    void printLine(const uint8_t* data, uint16_t len) override { SPI_Write(data, len); }
    bool isReady() const override { return GPIO_Read(PIN_READY); }
};

class InkjetHead : public IPrintHead {
public:
    void printLine(const uint8_t* data, uint16_t len) override { UART_Write(data, len); }
    bool isReady() const override { return UART_CheckStatus(); }
};

// 3. Application chỉ dùng Interface
void PrintManager::processJob(IPrintHead& head, const uint8_t* buffer) {
    if (head.isReady()) {
        head.printLine(buffer, 128); // Đa hình (Polymorphism) hoạt động tại đây
    }
}
```

## Case 2: Factory Pattern Không Dùng Heap (Dynamic Allocation)
**Ngữ cảnh**: Cần chọn Driver cảm biến dựa trên tín hiệu phần cứng lúc khởi động (Board Variant).
**Bad Practice**: `ISensor* sensor = new I2CSensor();` (Sử dụng `new` cấp phát động trên Heap dễ gây phân mảnh bộ nhớ - Memory Fragmentation).
**Good Practice**: Khởi tạo tĩnh (Static allocation) toàn bộ object và trả về con trỏ/tham chiếu.
```cpp
// Khởi tạo sẵn trên vùng nhớ tĩnh (.bss / .data)
static I2CSensor i2c_sensor;
static SpiSensor spi_sensor;

ISensor& SensorFactory::getSensor(BoardType type) {
    if (type == BOARD_V1) return i2c_sensor;
    return spi_sensor;
}
```

## Case 3: Hàm ảo thuần túy trong Unit Test (Mocking)
**Ngữ cảnh**: Bạn viết code trên PC để test `PrintManager` nhưng không có phần cứng đầu in thực tế.
**Giải pháp**: Nhờ kiến trúc Interface ở Case 1, bạn có thể dễ dàng tạo một `MockPrintHead` bằng framework như GoogleTest/GoogleMock.
```cpp
class MockPrintHead : public IPrintHead {
public:
    void printLine(const uint8_t* data, uint16_t len) override { 
        // Thay vì giao tiếp SPI/UART, chỉ ghi log hoặc đếm số lần gọi hàm để Test
        lines_printed++; 
    }
    bool isReady() const override { return true; } // Luôn giả lập trạng thái sẵn sàng
};
```

## Case 4: Chi phí của Virtual Function trong Hot Path (Vòng lặp siêu tốc)
**Ngữ cảnh**: Robot thăng bằng có vòng lặp PID Control 10,000 lần/giây (100us mỗi chu kỳ).
**Vấn đề**: Gọi hàm ảo `virtual void compute()` bên trong vòng lặp này sẽ tốn chi phí tra cứu bảng `vtable` liên tục, gây hao phí CPU Cycle không đáng có.
**Giải pháp**: Giữ hàm tính toán PID là hàm thường (Non-virtual) hoặc `inline`. Chỉ dùng `virtual` ở các tác vụ cấu hình rảnh rỗi (Ví dụ: `virtual void loadConfig()`).

## Case 5: Kế thừa quá sâu (Deep Inheritance) - Lỗi kiến trúc phổ biến
**Ngữ cảnh**: Kiến trúc Robot có cây kế thừa: `BaseDevice -> MovingDevice -> Actuator -> Motor -> DcMotor`.
**Vấn đề**: Khi có bug xảy ra trong hàm `init()`, lập trình viên phải mở 5 file khác nhau để truy xuất luồng thực thi (luật gọi `super::init()`). Tốn Flash do nhiều vtable sinh ra.
**Giải pháp**: Rút gọn tối đa còn 2 tầng (Ví dụ: `IMotor -> DcMotor`). Sử dụng **Composition** để quản lý các tính năng khác như Di chuyển, Báo lỗi.

## Case 6: CRTP (Curiously Recurring Template Pattern) - Đa hình lúc biên dịch
**Ngữ cảnh**: Cần tính đa hình (Polymorphism) cho tốc độ cực cao, không chịu được độ trễ của `vtable`.
**Giải pháp**: Sử dụng Template của C++ để "Đa hình tĩnh".
```cpp
template <typename T>
class MotorBase {
public:
    void step() {
        static_cast<T*>(this)->hardwareStep(); // Gọi trực tiếp hàm của class con lúc Compile
    }
};

class FastStepper : public MotorBase<FastStepper> {
public:
    void hardwareStep() { GPIO_Toggle(PIN_STEP); } // Không có vtable, được inline!
};
```
*Lưu ý*: CRTP làm code khó đọc và phình to (Code Bloat). Chỉ dùng khi có yêu cầu tối ưu thời gian thực siêu khắt khe.

## Case 7: Lạm Dụng `dynamic_cast` / RTTI
**Ngữ cảnh**: Hệ thống nhận một danh sách các thiết bị.
**Bad Practice**: 
```cpp
void resetDevice(IDevice* dev) {
    if (auto* m = dynamic_cast<Motor*>(dev)) { m->resetEncoder(); }
    else if (auto* s = dynamic_cast<Sensor*>(dev)) { s->recalibrate(); }
}
```
**Giải thích**: Hàm `dynamic_cast` buộc bật tính năng RTTI, tiêu tốn rất nhiều ROM/Flash để lưu trữ thông tin tên các Class (Metadata). Theo chuẩn nhúng, thiết kế kiểu này là tồi.
**Good Practice**: Thêm hàm `virtual void reset() = 0;` vào `IDevice` và bắt `Motor`, `Sensor` tự triển khai logic reset của riêng chúng.

## Case 8: Function Pointer Table (Đa Hình Kiểu C)
**Ngữ cảnh**: Phải viết code chuẩn C (vì Compiler không hỗ trợ C++) hoặc cần tối ưu kích thước RAM tối đa nhưng vẫn cần đổi Driver lúc Runtime.
**Giải pháp C-Style HAL**:
```c
// Bảng con trỏ hàm (Tương đương VTable tự chế)
typedef struct {
    void (*write)(const uint8_t* data, uint16_t len);
    bool (*is_ready)(void);
} UartDriver_Ops_t;

void App_Log(UartDriver_Ops_t* driver, const char* msg) {
    if (driver->is_ready()) { driver->write((uint8_t*)msg, strlen(msg)); }
}
```

## Case 9: Sử dụng `final` Để Tối Ưu Hóa Trình Biên Dịch (Devirtualization)
**Ngữ cảnh**: Bạn có một class `Stm32Uart` kế thừa từ `IUart`. Bạn chắc chắn 100% sẽ không có class nào kế thừa tiếp từ `Stm32Uart`.
**Giải pháp**: Thêm từ khóa `final`.
```cpp
class Stm32Uart final : public IUart {
    void send() override { /* ... */ }
};
```
**Lợi ích**: Trình biên dịch (GCC/Clang) khi gặp `final` sẽ hiểu rằng không cần tra cứu qua `vtable` nữa nếu kiểu tĩnh đã rõ ràng, nó sẽ tự động biến lời gọi hàm thành hàm tĩnh (Devirtualization), giúp chạy nhanh y hệt hàm thường.

## Case 10: Interface Segregation Principle (Chia nhỏ Interface)
**Ngữ cảnh**: Một giao diện Giao tiếp Không dây.
**Bad Practice**:
```cpp
class IWireless {
    virtual void connectWifi() = 0;
    virtual void connectBluetooth() = 0; // Bắt buộc module Wifi cũng phải triển khai Bluetooth?
};
```
**Good Practice**: Chia nhỏ thành `IWifi` và `IBluetooth`. Nếu một chip ESP32 hỗ trợ cả hai, nó sẽ đa kế thừa (Multiple Inheritance):
```cpp
class Esp32Combo : public IWifi, public IBluetooth {
    // Triển khai tính năng cho cả 2 giao diện
};
```
