# OOP Basics - 10 Case Studies (Thực tế Embedded / IoT / Robotics)

## Case 1: Xóa Bỏ Tính Năng Copy (Non-copyable Object)
**Ngữ cảnh**: Class `HardwareTimer` quản lý Timer1 của MCU.
**Vấn đề**: Nếu copy `HardwareTimer t2 = t1;`, cả 2 object sẽ cùng gọi destructor và tắt Timer1, gây treo hệ thống.
**Giải pháp**: Xóa copy constructor và copy assignment.
```cpp
class HardwareTimer {
public:
    HardwareTimer() { /* Bật Timer */ }
    ~HardwareTimer() { /* Tắt Timer */ }
    
    // ISO 26262: Ngăn chặn lỗi chia sẻ tài nguyên phần cứng vô tình
    HardwareTimer(const HardwareTimer&) = delete;
    HardwareTimer& operator=(const HardwareTimer&) = delete;
};
```

## Case 2: Tránh Code Cứng (Hardcoding) Bằng Dependency Injection (DI)
**Ngữ cảnh**: Cảm biến nhiệt độ cần đọc qua I2C.
**Bad Practice**: Class `TemperatureSensor` tự khởi tạo và gọi cứng `I2C1_Read()`. Không thể Unit Test (vì Unit Test chạy trên PC, không có I2C1).
**Good Practice**: Truyền Interface I2C vào qua Constructor (Pass by Reference).
```cpp
// Constructor nhận vào một object I2CBus đã được khởi tạo
TemperatureSensor(I2CBus& i2c_bus) : bus_(i2c_bus) {}

void read() {
    bus_.readBytes(0x40, buffer, 2); // Có thể truyền MockI2CBus khi Unit Test!
}
```

## Case 3: Constructor KHÔNG Làm Tác Vụ Nặng (Two-Stage Init)
**Ngữ cảnh**: Khởi tạo chip Wifi ESP32 trong class `WifiManager`.
**Bad Practice**: Constructor gửi tập lệnh AT commands qua UART. (Gây delay nhiều giây trước khi vào `main()`).
**Good Practice**: 
```cpp
class WifiManager {
public:
    WifiManager() : is_connected_(false) {} // Stage 1: Rất nhanh, an toàn
    
    bool begin() { // Stage 2: Được gọi chủ động trong luồng chính (Task)
        return send_at_command("AT+RST"); 
    }
};
```

## Case 4: Trách Nhiệm Đơn Lẻ (Single Responsibility Principle)
**Ngữ cảnh**: Quản lý đầu in (Printhead) của máy in 3D.
**Bad Practice**: `PrintheadManager` vừa điều khiển động cơ XYZ, vừa điều khiển nhiệt độ, vừa đọc file G-Code.
**Good Practice**: Tách ra làm 3 class: `MotionController`, `HeaterControl`, `GCodeParser`. Lớp `PrintheadManager` chỉ giữ vai trò điều phối (Coordinator) gọi các class kia.

## Case 5: Ẩn Biến Trạng Thái (Encapsulation) Bằng Getter/Setter
**Ngữ cảnh**: Quản lý trạng thái Pin (Battery) của Robot.
**Bad Practice**: `public: uint8_t battery_level;` (Bất kỳ hàm nào cũng có thể sửa pin thành 200%).
**Good Practice**: 
```cpp
private: 
    uint8_t battery_level_;
public:
    uint8_t getLevel() const { return battery_level_; }
    void updateLevel(uint8_t raw_adc) {
        battery_level_ = calculate_percentage(raw_adc); // Logic giới hạn 0-100% nằm ở đây
    }
```

## Case 6: Quản Lý Resource An Toàn với RAII (Resource Acquisition Is Initialization)
**Ngữ cảnh**: Cần Lock một Mutex (RTOS) để truy cập biến dùng chung, sau đó phải Unlock.
**Vấn đề**: Nếu hàm có nhiều câu lệnh `return` hoặc văng Exception, lập trình viên rất dễ quên `Unlock`, gây Deadlock.
**Giải pháp**: C++ RAII Lock Guard.
```cpp
class MutexGuard {
public:
    MutexGuard(Mutex& m) : mutex_(m) { mutex_.lock(); }
    ~MutexGuard() { mutex_.unlock(); } // Tự động chạy khi ra khỏi { } scope!
private:
    Mutex& mutex_;
};

void process_data() {
    MutexGuard guard(my_mutex); // Tự động Lock
    if (error) return; // Tự động Unlock khi return!!! Cực kỳ an toàn.
}
```

## Case 7: Static Members Cho Interrupts (ISR)
**Ngữ cảnh**: Hardware Interrupt của UART báo có dữ liệu. ISR phải đưa dữ liệu vào mảng trong class `UartDriver`.
**Vấn đề**: Hàm ISR của C không thể gọi `my_uart.receive()`.
**Giải pháp**: Dùng `static` pointer trỏ tới đối tượng toàn cục (Singleton pattern đơn giản).
```cpp
class UartDriver {
public:
    static UartDriver* instance;
    void receive() { /* ... */ }
};

UartDriver* UartDriver::instance = nullptr;

extern "C" void UART_RX_ISR() {
    if (UartDriver::instance) UartDriver::instance->receive();
}
```

## Case 8: Quản Lý Vòng Đời Object (Object Lifetime)
**Ngữ cảnh**: Tạo một luồng (Task) trong FreeRTOS sử dụng Object cục bộ.
**Bad Practice**: Tạo Object `Sensor` bên trong một hàm, truyền con trỏ của nó cho Task, rồi hàm kết thúc (Object bị xóa khỏi Stack). Task tiếp tục chạy và truy cập vào con trỏ "Ma" (Dangling pointer).
**Good Practice**: Các Object lõi của hệ thống nhúng nên được cấp phát `static` toàn cục hoặc nằm trong file `main()`, tồn tại vĩnh viễn trong suốt thời gian chạy.

## Case 9: Sử Dụng `const` Methods Chặt Chẽ
**Ngữ cảnh**: Viết hàm lấy cấu hình in (Printer Resolution).
**Practice**: Các hàm chỉ Đọc (Read-only) phải có từ khóa `const` ở cuối.
```cpp
class PrinterConfig {
public:
    uint16_t getDpi() const { return dpi_; } // Compiler sẽ báo lỗi nếu hàm này lỡ sửa biến dpi_
};
```
**Lợi ích**: Tăng tính an toàn (ISO 26262), giúp người gọi biết chắc chắn hàm này không gây ra "Side Effect" làm đổi trạng thái hệ thống.

## Case 10: Tổ chức Hardware Abstraction Layer (HAL)
**Ngữ cảnh**: Viết code điều khiển đèn LED, nay mai có thể đổi chip từ STM32 sang NXP.
**Practice**: 
- `Led` class không nên gọi thẳng `GPIOA->ODR = 1;`.
- `Led` class nên gọi `Hal_Gpio_Write(port, pin, HIGH)`.
- Hàm `Hal_Gpio_Write` được viết bằng C và sẽ được thay đổi tùy theo chip.
Cách này giúp class `Led` của C++ giữ nguyên 100% logic khi Porting sang dòng vi điều khiển khác.
