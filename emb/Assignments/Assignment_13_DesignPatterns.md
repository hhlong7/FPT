C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment13> TYPE: <Embedded_DesignPatterns> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 13: Design Patterns For MCU (Không Dùng RAM Cấp Phát Động)

### Exercise 1: State Machine (Máy Trạng Thái) Bằng C-Style Function Pointers
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (State Machine dùng hàm chuyển đổi) và Case 10 (State Pattern bằng mảng con trỏ hàm)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) của chương.

**Ngữ cảnh**: Máy in 3D của bạn có Hệ thống Vận Hành (Operation System). Nó đi qua các trạng thái tuần tự: `IDLE` (Nghỉ), `HEATING` (Nung nóng), `PRINTING` (Đang in), `FAULT` (Kẹt/Lỗi).
**Bad Practice**: 1 hàm `main` khổng lồ với lệnh `switch-case` lồng 4 tầng kiểm tra cờ (flags) chằng chịt.
**Yêu cầu (Pattern Cấp Thấp)**:
Viết 4 hàm (Functions) độc lập đại diện cho 4 Trạng thái: `void State_Idle()`, `void State_Heating()`, `void State_Printing()`, `void State_Fault()`.
1. Tạo một Mảng các Con trỏ Hàm (Function Pointer Array) chứa địa chỉ 4 hàm này. (Ví dụ `StateTable[]`).
2. Định nghĩa một biến Toàn cục (Hoặc Static) `uint8_t current_state_index = 0;`.
3. Viết Vòng Lặp Vô Tận (Super-loop) ở hàm `main()` để GỌI nhảy thẳng tới Hàm Trạng Thái tương ứng bằng 1 dòng code duy nhất thông qua Mảng `StateTable`. Vòng lặp này siêu gọn, không có `if-else` nào.

### Exercise 2: Cắt Đứt Sự Chằng Chịt Bằng Mẫu Observer (Publish-Subscribe)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Tránh dùng biến toàn cục bằng Singleton), Case 4 (Observer Pattern cho nút khẩn cấp) và Case 6 (Publish/Subscribe trên FreeRTOS)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) của chương.

**Ngữ cảnh**: Cảm biến Nhiệt Độ Báo Lỗi "QUÁ NHIỆT" (Over-temp).
1. Màn hình cần Chớp nháy thông báo.
2. Quạt phải Quay 100% công suất.
3. Động cơ trục X Y Z phải lập tức Dừng Khẩn Cấp.
**Bad Practice**: Trong file `TempSensor.c`, bạn `include` file Lcd.h, Fan.h, Motor.h và gọi các hàm của chúng. Hàm Sensor biến thành Một Đống rác liên kết, Cảm biến biết quá nhiều việc không liên quan.
**Yêu cầu (Observer Pattern)**:
Thiết kế "Bảng Tin Đăng Ký" (Event Broker).
1. File `TempSensor.c` cung cấp 1 hàm `void Dang_Ky_Ham_Callback( void (*func)(void) );`. Sensor tự động giữ 1 mảng Mảng các Lời Cầu Nhắn (Callbacks Array).
2. Khi Khởi động Hệ thống, Module Màn Hình tự gọi: `Dang_Ky_Ham_Callback(Lcd_Blink_Error)`. Các Module khác cũng tự nộp Hàm của mình vào Bảng.
3. Khi "Quá Nhiệt" Xảy ra. Sensor chỉ làm 1 việc duy nhất: Quét Vòng Lặp Bảng Đăng Ký, Bắn (Gọi) toàn bộ các Hàm đó. Sự Tách Rời (Decoupling) Module Lõi và Mở Rộng Ứng dụng đã hoàn hảo 100%. Viết đoạn mã C cấu trúc mô hình này.

### Exercise 3: HAL / Strategy Pattern Để Sống Sót Qua Cơn Thiếu Chip
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Tách lập ứng dụng khỏi phần cứng - HAL Pattern), Case 8 (Facade Pattern cho API cấp cao) và Case 9 (Adapter / Wrapper Pattern)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) của chương.

**Ngữ cảnh**: Công ty sản xuất Máy In dùng chip Motor điều khiển A4988. Đứt gãy chuỗi cung ứng, Công ty phải mua Chip Motor TMC2209. Hai con chip có cách giao tiếp Giao thức VẬT LÝ hoàn toàn khác nhau.
**Yêu cầu (Strategy / Interface Adapter)**:
Để Tầng Ứng dụng Máy In (Thuật toán Tính Góc Quay) KHÔNG BAO GIỜ cần Sửa 1 chữ nào, bạn xây dựng Lớp Đệm Giao Tiếp (Hardware Abstraction Layer).
1. Viết một C-Struct (Struct chứa Con trỏ Hàm) đại diện cho Mẫu `IMotorDriver`. Bên trong có Cổng kết nối (Interface): `void (*Step_Forward)()`, `void (*Set_Current)(int mA)`.
2. Tạo File `A4988.c`. Viết Hàm giao tiếp Thật, và gán Hàm Giao Tiếp Thật đó vào Cấu Trúc Struct Interface `IMotorDriver`.
3. Tầng Ứng Dụng (App Core) chỉ truyền cái Biến `IMotorDriver* my_driver` đó và ra Lệnh Quay. Tính đa hình và Thay Đổi Chiến Lược (Strategy Change) đã cứu bộ Code Máy in như thế nào?

### Exercise 4: Command Pattern Và Cỗ Máy Xử Lý Lệnh Xếp Hàng
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 5 (Command Pattern)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) của chương.

**Ngữ cảnh**: Wifi nhận 50 Lệnh Máy in qua Mạng (Di chuyển, Đổi Nhiệt độ, Hút Nhựa). Wifi chép vào Ngắt (ISR) quá nhanh, Động cơ chưa chạy xong lệnh 1.
**Yêu cầu**: Ném Lệnh vào Quá khứ để Xử lý Sau (Deferred Execution).
1. Đóng gói Mọi loại Lệnh khác nhau vào chung MỘT (1) Cấu trúc Dữ Liệu Đồng Nhất có chứa `uint8_t command_id` và Mảng Byte Parameter `uint8_t payload[8]`. Đặt tên là `struct PrintCommand`.
2. Mạng ném `PrintCommand` vào Hàng Đợi (Ring Buffer Queue).
3. Task/Thread Motor chậm rãi lấy (Pop) Từng `PrintCommand` ra từ Queue, Phân giải bằng Switch-case cái `command_id` rồi mới kích Motor. Thiết kế Kiến Trúc Dòng Chảy Lệnh (Command Pipeline) Cực Kì Cơ Bản của ngành Nhúng này giúp hệ thống Real-time không bị quá tải. Viết mã giả (Pseudo-code) cho đường đi của Queue này.

### Exercise 5: Single-Threaded Active Object (Tránh Khóa Chéo Deadlock RTOS)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 7 (Active Object - Luồng dữ liệu an toàn)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/13_DesignPatterns/example.md) của chương.

**Ngữ cảnh**: Trạm Gateway IoT (Chạy hệ điều hành FreeRTOS). Biến `global_ip_config` được 3 Task (Wifi Task, Lcd Task, Button Task) lao vào Lấy, Đọc, và Sửa.
**Bad Practice**: Lập trình viên Căng Khóa (Mutex Lock) vào mọi thứ. Task Wifi Xích Mutex, Task Lcd phải đứng im chờ, hệ thống giật lag, dễ xảy ra Chờ chéo nhau Khóa chết hệ thống (Deadlock).
**Yêu cầu (Active Object Pattern - Lập trình Bất Đồng Bộ)**:
Đập bỏ hoàn toàn Mutex! Biến `global_ip_config` bây giờ trở thành `private` và CHỈ ĐƯỢC PHÉP TRUY CẬP BỞI MỘT TASK DUY NHẤT LÀ `Config_Task`.
If Wifi Task muốn Sửa IP, Nó PHẢI NÉM 1 MESSAGE (Gói Thư Dữ Liệu) Tới Cái Mailbox (Queue) của `Config_Task`. `Config_Task` có Vòng Lặp Vô Tận nhận Thư, mở thư ra tự tay sửa Biến Private IP. Không có 2 Task nào thọc tay vào cùng một Cục Nhớ Cùng Một Lúc. Hiện tượng Deadlock Mutex Đã Bốc Hơi. Phân tích lợi ích to lớn của Active Object Architecture trên Vi Điều Khiển Lõi Kép (Dual-core ESP32/STM32).
