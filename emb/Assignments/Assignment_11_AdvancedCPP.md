C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment11> TYPE: <Embedded_CPP> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 11: C++ Nâng Cao Trên MCU - RAII, Move Semantics & Safety

### Exercise 1: Tối Ưu RAM Gói Tin Khổng Lồ Bằng Cướp Quyền Sở Hữu (Move Semantics `&&`)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Sử dụng constexpr) và Case 6 (Move Semantics - unique_ptr)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/11_Advanced_Cpp_Features/example.md) của chương.

**Ngữ cảnh**: IoT Gateway nhận Cập nhật Firmware (OTA) từng khối dữ liệu 4KB.
```cpp
class OtaChunk {
    uint8_t buffer[4096];
    // Constructor, Copy Constructor...
};
void Network_Layer(OtaChunk data) { 
    Flash_Layer_Write(data); // Hành động 2: Copy lần nữa vào lớp Ghi Flash
}
// Tại Main: Network_Layer(my_chunk); // Hành động 1: Copy 4KB vào lớp Mạng
```
**Vấn đề**: Cấu trúc kiến trúc Firmware chia nhiều Lớp (Layers). Dữ liệu 4KB bị Copy từ Lớp này sang Lớp khác. CPU mất hàng chục Mili-giây để chép mảng, Stack Memory bị vỡ tung do tốn 8KB-12KB cho các bản Copy.
**Yêu cầu**:
1. Sử dụng C++11 Move Semantics, biến `OtaChunk` thành Một Class **Move-Only** (Ngăn chặn Copy, chỉ cho phép Chuyển nhượng Con Trỏ Bộ Nhớ bên trong - Rvalue Reference `&&`).
2. Trình bày bằng mã giả (Pseudo-code) cách các Lớp Mạng gọi nhau bằng cú pháp `std::move(data)` để vận chuyển 4KB Data xuyên thủng 3 Lớp Code mà chỉ tốn đúng 4 Bytes thao tác con trỏ. Sự thần kỳ (Zero-cost overhead) của C++ hiện đại nằm ở đây.

### Exercise 2: RAII - Trái Tim An Toàn Của Tài Nguyên Nhúng
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (RAII Lock Guard) và Case 4 (Cấp Phát Tĩnh độc quyền unique_ptr)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/11_Advanced_Cpp_Features/example.md) của chương.

**Ngữ cảnh**: Hàm điều khiển Giao thức SPI (Rất nhạy cảm với việc Mở/Khóa chip).
```cpp
void read_sensor_spi() {
    GPIO_SetPin(CS_PIN, LOW); // Select Chip (Khóa kênh SPI)
    
    if (SPI_Transmit(CMD_READ) == ERR) {
        // NGHIÊM TRỌNG: Kỹ sư lập trình lỡ Quên set chân CS_PIN lên HIGH ở đây
        return; 
    }
    
    // Đọc data...
    GPIO_SetPin(CS_PIN, HIGH); // Unselect Chip (Nhả kênh SPI)
}
```
**Vấn đề**: Bất cứ khi nào hàm `return` sớm do lỗi, kênh SPI sẽ bị kẹt vĩnh viễn (Deadlock phần cứng).
**Yêu cầu**:
Thiết kế một C++ Class tên là `SpiLockGuard` tuân theo chuẩn **RAII** (Resource Acquisition Is Initialization). 
Sử dụng Constructor để tự động Select Chip (LOW), và Destructor để tự động Unselect Chip (HIGH).
Trình bày cách sử dụng `SpiLockGuard` trong hàm `read_sensor_spi()` để chứng minh rằng: "Dù kỹ sư có đặt 100 câu lệnh `return` lỗi, kênh SPI vẫn ĐƯỢC MỞ KHÓA tự động một cách Thần Kỳ 100% khi thoát hàm".

### Exercise 3: An Toàn Đa Luồng Với `std::atomic` Cấp Phần Cứng & Enum Class
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (enum class), Case 5 (std::atomic) và Case 7 (Lệnh Báo Lỗi std::expected)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/11_Advanced_Cpp_Features/example.md) của chương.

**Ngữ cảnh**: Bạn có một cờ Báo hiệu Giấy Kẹt `bool paper_jam = false;`.
ISR Báo lỗi giấy chạy độc lập ngẫu nhiên và bật cờ này lên `true`.
Main Loop chạy liên tục: `if (paper_jam == true) { paper_jam = false; In_Báo_Lỗi(); }`.
**Vấn đề**: Việc Main Loop Đọc xong rồi Chỉnh biến lại thành False tốn ĐẾN 3 Lệnh Assembly. NẾU đúng lúc Main Loop Đọc xong (nhưng chưa kịp Ghi về False), ISR lặp lại ngắt bắn một Lỗi Mới Nhất vô. Sau đó Main Loop chạy tiếp lệnh Ghi False XÓA SẠCH Lỗi Mới Nhất của ISR. Hậu quả: Mất Dấu Hiệu Lỗi (Data Loss Race Condition). Cờ `volatile bool` KHÔNG THỂ BẢO VỆ ĐƯỢC CHUỖI 3 LỆNH NÀY.
**Yêu cầu**:
Trong C++11, `std::atomic<bool>` ép CPU sử dụng một Tác vụ Cấp Phần Cứng (Hardware atomic exchange instruction) để vón cục 3 lệnh Đọc-Sửa-Ghi thành 1 Khối Bất Khả Xâm Phạm.
Viết mã giả C++ dùng `std::atomic` và hàm `exchange()` để thực hiện Đọc Cờ và Xóa Cờ trong đúng 1 Tác Vụ An Toàn tuyệt đối.
Đồng bộ hóa các mã lỗi kiểu dữ liệu an toàn bằng cách dùng `enum class` và cấu trúc trả về mã lỗi expected.

### Exercise 4: Từ Khối Exceptions (Cấm kỵ) Chuyển Sang `std::optional`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 8 (Range-based for loops) và Case 9 (auto keyword)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/11_Advanced_Cpp_Features/example.md) của chương.

**Ngữ cảnh**: Hệ nhúng MISRA CẤM TẮT TỊT khối lệnh `try { ... } catch()`.
Hàm đọc cấu hình Wifi từ EEPROM, có thể bị lỗi do Thẻ nhớ hỏng.
Nếu bạn trả về Mảng Byte `uint8_t*`, làm sao biết được Hàm chạy Thành Công và cấp Mảng Byte, hay Hàm bị Lỗi và Mảng Rỗng? (Truyền thêm tham số trạng thái ra ngoài kiểu C rất rườm rà).
**Yêu cầu**:
Sử dụng C++17 `std::optional` (Trình bao bọc Dữ Liệu Tùy Chọn - Cực Nhẹ). Khai báo Hàm trả về `std::optional<WifiConfig>`. Trình bày cách gọi Hàm và Check (Kiểm tra) xem Hàm có Dữ liệu Thành Công (Has value) hay bị Lỗi (Empty) một cách sạch sẽ và Type-Safe (An Toàn Kiểu).

### Exercise 5: Cắt Bỏ Mớ Bòng Bong Của Trình Biên Dịch Bằng `noexcept`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 10 (noexcept - Triệt tiêu Exception Tracking)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/11_Advanced_Cpp_Features/example.md) của chương.

**Ngữ cảnh**: Firmware của bạn có 500 hàm `Getter` nhỏ lẻ. VD: `int getStatus() { return status; }`.
Mặc dù bạn đã tắt Compiler Cờ Exception (`-fno-exceptions`). Trình biên dịch C++ thỉnh thoảng vẫn nhét các đoạn mã theo dõi Luồng nhảy ẩn (Stack unwinding tracking code) vào quanh các hàm này làm tốn ROM và chậm tốc độ nhảy (Jump Overhead).
**Yêu cầu**:
1. Từ khóa `noexcept` đặt ở Cuối Hàm có tác dụng Lời Thề Bằng Máu (Promise) gì đối với Trình biên dịch C++?
2. Khi Compiler nhìn thấy `noexcept`, nó sẽ lập tức dọn dẹp và cắt bỏ Cấu trúc Bảo Vệ (Overhead Guard) nào?
3. Viết lại hàm `getStatus()` ở trên bằng chuẩn C++ Tối ưu Tận Cùng cho MCU.
