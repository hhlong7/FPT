C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.MiniProject> TYPE: <Embedded_System_Design> LOC: <N/A> DURATION: <480 minutes> |
| --- | --- |

## Final Mini-Project: Kiến Trúc Cốt Lõi Trạm Gateway IoT (Bare-Metal/RTOS System)

### Mục Tiêu Tổng Quan
Xây dựng một hệ thống Nhúng thực tế, đáp ứng các tiêu chí gắt gao của môi trường Vi điều khiển (Ví dụ ARM Cortex-M4, RAM 64KB, Flash 256KB): Không cấp phát động (`no malloc/new`), sử dụng Không gian tĩnh, xử lý Thời gian thực với ngắt, và có hệ thống Đóng gói OOP theo chuẩn MISRA C/C++.
Dự án: **Trạm Quản Lý Mạng Lưới Cảm Biến Công Nghiệp & Màn Hình (IoT Sensor Gateway & Local HMI)**.

### Thành Phần 1: Giao Tiếp Ngắt & Cấu Trúc Đệm Vòng Tĩnh (Ring Buffer)
**Ngữ cảnh**: Mạng lưới các Cảm biến Bụi, Nhiệt độ (Node) liên tục truyền hàng nghìn Khung dữ liệu (Frames) qua cổng UART/CAN về Gateway.
**Yêu cầu Kỹ Thuật (Data Structures & C_Pointers)**:
1. Bạn phải tự thiết kế một lớp (Class) C++ (Hoặc Struct C) `StaticRingBuffer`. Bắt buộc kích thước phải là `256` hoặc `512` Elements, Khai báo bằng Mảng Tĩnh (Array `[]`), Không sử dụng `std::vector` hay `std::queue`.
2. Hàm Ngắt Cứng (Hardware ISR) nhận từng Byte đẩy vào Mảng. Hàm Push phải Tối ưu Tốc Độ Bằng Toán Tử Bitwise (`head = (head + 1) & MASK`).
3. Khung truyền Frame dài cố định 8 Bytes (Header, ID Cảm biến, Dữ liệu, Checksum). Viết Hàm Phân Tích (Parser) ở Tầng Main Loop Lấy Gói Tin ra khỏi Buffer, Kiểm tra mã Checksum, Ép Kiểu Con trỏ An Toàn (Strict Aliasing) để tách dữ liệu.

### Thành Phần 2: Quản Lý Bộ Nhớ Tĩnh Nhóm Đối Tượng Cảm Biến (Object Lifetime & OOP)
**Ngữ cảnh**: Gateway quản lý danh sách tối đa `50 Cảm Biến`. (Không thêm bớt).
**Yêu cầu Kỹ Thuật**:
1. Thiết kế Lớp `SensorNode`. Đóng gói Toàn bộ Biến `private` (Mức Pin, Lần cập nhật cuối `timestamp`, Dữ liệu hiện tại). Cấm Copy Constructor (Mỗi Sensor là tài nguyên Duy Nhất).
2. Xây dựng một Mảng Cố Định `std::array<SensorNode, 50>` (Nếu dùng C++) hoặc Mảng Struct tĩnh ở Tầng Giao Diện Lõi (Core App). 
3. Thuật Toán Lọc EMA (Exponential Moving Average): Bên trong Class `SensorNode`, giá trị cảm biến mới nhận được phải được Đẩy qua Bộ Lọc Số Nguyên EMA để khử Nhiễu môi trường trước khi Lưu trạng thái nội bộ.

### Thành Phần 3: Kiến Trúc Mẫu Quan Sát (Observer) Để Xử Lý Báo Động (Alarms)
**Ngữ cảnh**: Khi `SensorNode` đo Nhiệt Độ Vượt Mức > 100 độ C. Màn Hình LCD phải nháy Đỏ, Bơm nước Chiller phải bật, Modul Wifi phải gửi Gói Cảnh Báo MQTT lên Cloud AWS.
**Yêu cầu Cấu Trúc Phần Mềm (Design Patterns)**:
1. Áp dụng Observer Pattern (Mẫu Quan Sát / Callback). 
2. `SensorNode` hoàn toàn Tuyệt Giao (Không được `#include` hay biết gì về LCD, Bơm, Wifi). Nó chỉ gọi 1 Hàm Function Pointer Cảnh Báo đã được các Modul Ngoài Tự Đăng Ký (Subscribe) vào Mảng Danh Sách Hàm Lắng Nghe Của Nó.
3. Khi Quá nhiệt, Cảm biến vòng lặp Duyệt Danh Sách và Bắn Lệnh (Trigger) Toàn bộ Hệ Thống phản ứng đồng loạt. 

### Thành Phần 4: Hardware Abstraction Layer (HAL) Để Thay Đổi Màn Hình (Polymorphism)
**Ngữ cảnh**: Công ty xuất xưởng 2 phiên bản Gateway. Bản giá rẻ dùng Màn hình Chữ LCD 16x2 (Giao tiếp I2C). Bản Cao cấp dùng Màn Hình Đồ Họa TFT (Giao tiếp SPI). Mã Ứng Dụng Tính Toán Bắt Buộc Phải Dùng Chung Không Sửa Đổi 1 Chữ!
**Yêu cầu Kỹ Thuật (Inheritance / VTable)**:
1. Xây dựng một Abstract Interface (Lớp Trừu Tượng Hàm Ảo) tên là `IDisplay`. Khai báo Hàm thuần Ảo (Pure Virtual) `virtual void Show_Warning(uint8_t node_id) = 0;`.
2. Tạo 2 Lớp con kế thừa `Lcd16x2_Driver` và `TFT_SPI_Driver` Override lại cấu trúc lệnh Hiển thị phù hợp phần cứng.
3. Lớp Cốt lõi (App Core) khi Boot (Khởi động máy) sẽ Khởi tạo Lớp Giao Diện Màn hình dạng Con Trỏ Tĩnh dựa trên Một Cờ Biến `BOARD_VERSION_PIN`.
4. Nếu dùng C Thuần: Triển Khai Bài Này Bằng Cấu trúc Mảng Các Con Trỏ Hàm C-Style Vtable (`struct IDisplay { void (*Show_Warning)(uint8_t); }`).

### Báo Cáo Kỹ Thuật Cuối Khóa (Technical Documentation Report)
Thay vì nộp Code, bạn phải nộp một Tài Liệu Kiến Trúc (Architecture Doc).
Trong tài liệu:
1. Vẽ Sơ đồ Tổ chức Lớp Bộ Nhớ (Memory Map / Object Dependency). 
2. Chỉ ra Rõ Các Khối Nào Là Biến Tĩnh Vĩnh Cửu (Static/BSS Allocation). 
3. Phân tích Các Khối Nào Chạy Trong Tần Số CPU Nhanh (Luồng Ngắt - ISR Fast Path) và Khối nào Chạy Luồng Chậm (Main Loop Slow Path).
4. Chứng Minh Bạn Đã Tuân Thủ Tuyệt Đối Không Xảy Ra Phân Mảnh Heap `(No Malloc/New)` và Bảo Vệ Vùng Tới Hạn (Critical Sections) Tại các Điểm Nối Rủi Ro Chạm Chéo Dữ Liệu (Ví Dụ: Giữa Buffer Ngắt và Code Cập Nhật Màn Hình).
5. Bạn Sẽ Áp Dụng Công cụ Unit Test Nào Lên Lớp Lọc (Filter) và Lớp Chẩn Đoán (Parser) Độc Lập Khỏi Code Driver I2C/SPI Thực Tế? Trình Bày Pseudo-code Của Bài Test (Arrange-Act-Assert).
