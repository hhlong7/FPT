C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment07> TYPE: <Embedded_CPP> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 7: OOP Features & MCU Trade-offs (Inheritance, Polymorphism)

### Exercise 1: Thiết Kế HAL (Lớp Trừu Tượng Phần Cứng) Bằng Interface
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Abstract HAL Interface) và Case 10 (Interface Segregation Principle)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/07_OOP_Features/example.md) của chương.

**Ngữ cảnh**: Firmware cho Máy in hỗ trợ in qua Wifi (NetworkPrinter) hoặc in trực tiếp từ Thẻ nhớ (LocalPrinter). Lớp quản lý `PrintJobManager` cần đọc dữ liệu G-Code.
**Yêu cầu**:
1. Tránh sự phụ thuộc lằng nhằng bằng cách định nghĩa một Abstract Class (Interface C++) tên là `IDataSource`. Bên trong có 1 Hàm Ảo Thuần Túy (Pure Virtual Function) tên là `virtual bool getNextLine(char* buffer) = 0;`.
2. Tại sao Interface BẮT BUỘC phải có một `virtual ~IDataSource() = default;` (Virtual Destructor)? Nếu thiếu hàm hủy ảo này, hiện tượng gì sẽ xảy ra cho hệ thống nếu bạn xóa (delete) một đối tượng lớp Con thông qua con trỏ lớp Cha?
3. Viết một cấu trúc class `LocalFileSource` kế thừa từ `IDataSource` và định nghĩa đè (override) lại hàm `getNextLine`. Nhớ sử dụng từ khóa `override`.

### Exercise 2: Sát Thủ VTable Trong Hot-Path
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Dynamic Polymorphism VTable RAM Overhead), Case 3 (VTable Jump Instruction Execution Delay) và Case 9 (Devirtualization using final)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/07_OOP_Features/example.md) của chương.

**Ngữ cảnh**: Bạn ứng dụng Đa Hình (Polymorphism) để tạo các bộ Lọc Số (Digital Filter) thay đổi theo thời gian thực (LowPassFilter, HighPassFilter kế thừa IFilter). 
Thuật toán cân bằng PID của Robot gọi hàm `virtual int32_t process(int32_t sample)` 20,000 lần mỗi giây.
**Vấn đề**: Việc tính toán bị giật lag, MCU bị quá tải.
**Yêu cầu**:
1. Phân tích chi phí ẩn (Hidden Overhead) của việc gọi một Hàm Ảo (Virtual Function). Trình bày cơ chế tra cứu VTable (Bảng hàm ảo) và VPtr (Con trỏ ẩn).
2. Sự chậm trễ này không đơn thuần do tốn 2 lệnh Assembly để tìm bảng, mà nó còn phá vỡ cơ chế nào của CPU Pipeline và trình tối ưu hóa Biên dịch (Inline optimization)?
3. Đưa ra giải pháp thiết kế thay thế: Thay vì gọi Virtual 20,000 lần, ta nên thiết kế lại kiến trúc Lọc (Filter) sao cho quá trình chọn Bộ lọc chỉ diễn ra 1 lần ở bước Cấu Hình ban đầu (Initialization phase).

### Exercise 3: Devirtualization Bằng Từ Khóa `final`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 4 (Cost of Destructors) và Case 9 (Devirtualization using final)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/07_OOP_Features/example.md) của chương.

**Ngữ cảnh**: Cảm biến Gia tốc MPU6050 kết nối qua I2C. Bạn viết lớp `class MPU6050_Sensor : public ISensor` và ghi đè hàm `virtual void read_xyz() override`.
Bạn biết chắc chắn 100% rẳng toàn bộ công ty không bao giờ có kỹ sư nào lại đi kế thừa tiếp một class con từ cái class `MPU6050_Sensor` này nữa.
**Yêu cầu**:
1. Cập nhật thiết kế của Class này bằng cách thêm từ khóa `final` của chuẩn C++11 (VD: `class MPU6050_Sensor final : public ISensor`).
2. Trình bày lợi ích to lớn của từ khóa `final` trong hệ nhúng. Trình biên dịch (GCC/Clang) sẽ tối ưu hóa (Devirtualize) lời gọi hàm của class này như thế nào nếu kiểu tĩnh đã rõ ràng, giúp chạy nhanh như hàm C thuần túy?

### Exercise 4: Sự Chết Chóc Của RTTI & `dynamic_cast`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 7 (Danger of RTTI - dynamic_cast) và Case 8 (Function Pointers vs Virtual)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/07_OOP_Features/example.md) của chương.

**Ngữ cảnh**: Lớp `Diagnostics` nhận vào một danh sách các thiết bị phần cứng để tự kiểm tra lỗi (Self-Test). Danh sách chứa các con trỏ Interface lớp Cha: `IDevice* devices[10]`.
Lập trình viên muốn kiểm tra xem thiết bị nào là Cảm biến (Sensor) để chạy hàm Re-calibrate (Chỉnh lại mức 0), thiết bị nào là Motor thì chạy hàm Test vòng quay.
```cpp
// NGHIÊM CẤM TRONG EMBEDDED
void RunDiagnostics(IDevice* dev) {
    if (Sensor* s = dynamic_cast<Sensor*>(dev)) {
        s->recalibrate();
    } else if (Motor* m = dynamic_cast<Motor*>(dev)) {
        m->test_rotation();
    }
}
```
**Yêu cầu**:
1. Giải thích tại sao lệnh `dynamic_cast` bắt buộc phải sử dụng RTTI (Run-Time Type Information).
2. Việc bật Cờ (Flag) cho phép RTTI tác động tàn khốc thế nào đến bộ nhớ ROM/Flash của MCU? (Nhắc đến Meta-data tên các Class).
3. Đập bỏ hoàn toàn `dynamic_cast`. Hãy thiết kế lại Interface `IDevice` (thêm một hàm `virtual void runSelfTest() = 0;`) và áp dụng đúng triết lý Đa hình (Polymorphism) để thực hiện tính năng này mà không cần RTTI.

### Exercise 5: Đa Hình Lúc Biên Dịch Bằng Template (CRTP) - Siêu Cấp Tối Ưu
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 5 (High Inheritance Depth) và Case 6 (CRTP - Static Polymorphism)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/07_OOP_Features/example.md) của chương.

*Dành cho Kỹ sư C++ Nhúng Nâng cao.*
**Ngữ cảnh**: Việc dùng Interface (Virtual) làm chậm tín hiệu I2C. Bạn muốn thiết kế HAL cho I2C mà Tốc độ Cực đại (Zero Overhead), Lệnh Cực gọn.
Kỹ thuật **CRTP** (Curiously Recurring Template Pattern) cho phép làm đa hình ở bước Biên dịch (Compile-time).
```cpp
template <typename T>
class I2CBase {
public:
    void sendData() {
        static_cast<T*>(this)->hardware_send(); // Đa hình tĩnh!
    }
};

class STM32_I2C : public I2CBase<STM32_I2C> {
public:
    void hardware_send() { /* Ghi Thanh Ghi STM32 */ }
};
```
**Yêu cầu**:
1. Hãy tìm hiểu và trình bày ngắn gọn tại sao phương pháp CRTP trên lại KHÔNG tốn RAM cho VTable/Vptr, và có thể được Compiler Inline (Chèn thẳng ruột hàm) ra mã máy tốc độ cao nhất.
2. Nêu một Khuyết điểm (Nhược điểm) lớn nhất của việc dùng Template nhiều so với Inheritance thuần túy trong dự án hàng triệu dòng code (Liên quan đến thời gian Build và kích thước Code Bloat).
