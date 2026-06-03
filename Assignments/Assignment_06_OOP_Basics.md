C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment06> TYPE: <Embedded_CPP> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 6: OOP Basics on MCU - Encapsulation & Lifetime

### Exercise 1: Đóng Gói (Encapsulation) Trạng Thái Hệ Thống
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 5 (Encapsulation with Getter/Setter) và Case 9 (Strict const methods)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) của chương.

**Ngữ cảnh**: Firmware nhúng cũ viết bằng C quản lý một Van Nước (Valve) của máy in chất lỏng.
```c
// Code C kiểu cũ (Kém an toàn)
typedef struct {
    uint8_t pin;
    bool is_open;
    uint32_t total_opened_count;
} Valve_t;

Valve_t valve_1 = {PORTA_PIN_5, false, 0};
// Bất kỳ ai ở file nào cũng có thể gõ: valve_1.total_opened_count = 0; // Hủy hoại dữ liệu bảo hành!
```
**Yêu cầu**:
1. Chuyển đổi thiết kế trên sang Class C++. Biến các thuộc tính thành `private`.
2. Tạo Constructor nhận tham số là `pin`. Khởi tạo `is_open = false` và `count = 0`.
3. Cung cấp hàm `open()`, `close()`. Khi `open()` chạy, biến đếm tự động tăng. Cung cấp hàm đọc bộ đếm, nhưng cấm tuyệt đối việc sửa biến đếm từ bên ngoài.
4. Việc dùng C++ Class ở đây đã giải quyết lỗ hổng bảo mật dữ liệu của struct C như thế nào?

### Exercise 2: Rủi Ro Khi Constructor Cấu Hình Hardware (Two-Stage Init)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (Two-Stage Init) và Case 7 (Static Members for Interrupts)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) của chương.

**Ngữ cảnh**: Cần một Object điều khiển giao tiếp SPI với Màn hình cảm ứng.
Kỹ sư dùng OOP trên PC sang làm Nhúng viết như sau:
```cpp
class SpiDisplay {
public:
    SpiDisplay() {
        // Bên trong Constructor: Gửi hàng chục lệnh cấu hình ra chân SPI
        Hardware_SPI_Init(); 
        Hardware_SPI_Send(0x01); // Lệnh Reset màn hình
    }
};

// Khai báo Object Toàn cục (Global scope)
SpiDisplay main_display;

int main() {
    System_Clock_Init(); // Bật xung nhịp hệ thống (Crystal Oscillator)
    // Các logic khác...
}
```
**Vấn đề**: Firmware vừa chạy lên là bị Hard Fault treo cứng, không bao giờ vào tới hàm `main()`.
**Yêu cầu**:
1. Dựa vào kiến thức về quá trình Boot (C++ Startup Code) của Vi điều khiển, giải thích tại sao Constructor của `main_display` (Khai báo dạng Global) lại chạy TRƯỚC khi cấu hình xung nhịp `System_Clock_Init()`. Sự kiện này gây ra hậu quả gì cho ngoại vi SPI?
2. Viết lại cấu trúc Class `SpiDisplay` theo Pattern **Two-Stage Initialization** (Khởi tạo 2 giai đoạn) kinh điển của C++ Nhúng. Tách phần tạo Object khỏi phần cấu hình Hardware phần cứng.

### Exercise 3: Ngăn Chặn Copy Hành Động Chết Người (Non-Copyable Objects)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Non-copyable Object)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) của chương.

**Ngữ cảnh**: Lớp `HardwareTimer` nắm giữ độc quyền Timer số 1 của MCU để điều khiển động cơ quay chính xác từng Micro-giây.
```cpp
class HardwareTimer {
public:
    HardwareTimer() { Start_Timer1(); }
    ~HardwareTimer() { Stop_Timer1(); }
};

void run_motor_process() {
    HardwareTimer t1; // Bật Timer1
    
    // Lập trình viên vô ý Copy
    HardwareTimer t2 = t1; 
    
    // Khi thoát hàm, chuyện tồi tệ xảy ra!
}
```
**Vấn đề**: Khi hàm `run_motor_process` kết thúc, C++ sẽ tự động gọi Destructor của cả `t1` và `t2`. Quá trình copy đã tạo ra 2 "chủ sở hữu" ảo của cùng 1 phần cứng vật lý.
**Yêu cầu**:
1. According to ISO 26262, các Đối tượng quản lý trực tiếp tài nguyên phần cứng vật lý phải mang tính Độc Quyền (Exclusive Ownership). Hãy sửa Class `HardwareTimer` sử dụng tính năng của C++11 (`= delete`) để ngăn chặn việc Copy (Ngăn Copy Constructor và Copy Assignment).
2. Khi chặn copy, nếu một hàm khác BẮT BUỘC phải cần dùng Object `HardwareTimer` này để đọc thời gian, bạn phải truyền đối tượng này vào hàm con thông qua cơ chế nào của C++? (Tham chiếu `&` hay Truyền theo Giá trị Value?).

### Exercise 4: Nguyên Tắc Trách Nhiệm Đơn Lẻ (SRP - Single Responsibility Principle)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Dependency Injection), Case 4 (Single Responsibility Principle) và Case 10 (Hardware Abstraction Layer - HAL)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) của chương.

**Ngữ cảnh**: Kỹ sư phần mềm tạo ra một "God Object" (Lớp Thần Thánh) cho Máy in:
```cpp
class PrinterGod {
public:
    void parse_gcode_from_wifi();
    void heat_extruder(int temp);
    void move_xyz_motors(int x, int y, int z);
    void check_bed_level();
    void draw_ui_to_lcd();
};
```
**Vấn đề**: File class này dài 5000 dòng. Cứ hễ thay đổi thiết kế Màn hình LCD, nhóm G-code cũng bị phàn nàn là code bị lỗi. Việc Unit Test là bất khả thi vì muốn Test hàm Tính nhiệt độ lại bị dính tới Wifi.
**Yêu cầu**:
Hãy đập bỏ class `PrinterGod` và thiết kế lại Kiến trúc hệ thống bằng kỹ thuật **Composition** (Quan hệ "Có Một" / Has-A). Tạo ra các Class nhỏ đóng vai trò Đơn lẻ (GCodeParser, HeaterController, MotionController, UIManager) và nhúng chúng thành Thành viên (Members) bên trong một lớp `PrinterCoordinator` chính.

### Exercise 5: Constructor Mặc Định (Default Initialization) Trong Array
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 6 (RAII Resource Management) và Case 8 (Object Lifetime Management)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/06_OOP_Basics/example.md) của chương.

**Ngữ cảnh**: Bạn cần một mảng 100 cảm biến quang điện.
```cpp
class OpticalSensor {
private:
    uint8_t pin;
    uint16_t threshold;
public:
    OpticalSensor(uint8_t p) : pin(p), threshold(100) {}
};

// Cố gắng tạo mảng 100 cảm biến trên C++
OpticalSensor array_sensors[100]; // LỖI COMPILER BÁO ĐỎ!
```
**Vấn đề**: C++ báo lỗi `no default constructor exists for class OpticalSensor`. Trong C, bạn có thể tạo mảng struct tẹt ga, nhưng trong OOP, C++ buộc mọi phần tử phải được Khởi tạo đúng đắn.
**Yêu cầu**:
1. Tại sao C++ không cho phép tạo mảng tĩnh này? Giải thích khái niệm "Default Constructor" (Constructor Không Tham Số).
2. Có 2 cách để khắc phục: (1) Thêm Default Constructor (Gán pin ảo) và cấu hình sau bằng hàm `setPin(p)`. (2) Khởi tạo trực tiếp từng phần tử mảng nếu mảng nhỏ (Bằng danh sách `{OpticalSensor(1), OpticalSensor(2)}`).
Viết lại mã khắc phục theo cách 1. Tại sao cách 2 lại phi thực tế với mảng 100 phần tử?
