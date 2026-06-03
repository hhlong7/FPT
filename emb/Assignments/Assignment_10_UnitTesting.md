C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment10> TYPE: <Embedded_Testing> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 10: Unit Testing (C/C++) - Shift-Left Trong Hệ Nhúng

### Exercise 1: Tách Biệt Hardware Ra Khỏi Logic (Design For Testability)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Tách Biệt Logic Phần Cứng) và Case 10 (Tách Biệt Khởi Tạo HAL)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) của chương.

**Ngữ cảnh**: Hàm điều khiển Khởi động Máy In.
```c
// Code "Bẩn", bám chặt vào phần cứng, không thể Unit Test trên Máy Tính (PC)
void Printer_Init() {
    uint8_t temp = I2C_Read_Extruder_Temp(); // Hàm này gọi thanh ghi MCU!
    if (temp < 200) {
        GPIO_Set_Heater_Pin(HIGH);           // Hàm này kích điện chân chip!
        LCD_Print("Dang dun nong...");
    } else {
        LCD_Print("San sang in");
    }
}
```
**Vấn đề**: Muốn kiểm tra xem logic điều khiển LCD có đúng khi Nhiệt độ = 250 độ không. Nhưng C Code này KHÔNG THỂ Compile bằng GCC trên Máy tính Windows/Linux được vì thiếu thư viện phần cứng.
**Yêu cầu**:
Refactor (Cấu trúc lại) đoạn code trên bằng phương pháp Dependency Injection (Truyền tham số / Truyền Interface). Tách hoàn toàn Logic Xét điều kiện và In chuỗi ra khỏi hàm đọc Phần Cứng. Viết lại hàm `Printer_Logic(uint8_t current_temp)` sao cho nó trả về Dữ liệu/Lệnh thay vì tương tác chân Chip trực tiếp. Hàm này phải compile được trên PC 100%.

### Exercise 2: Kịch Bản Mocking (Làm Giả) Bằng GTest / CMock
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (Sử Dụng C++ Mocking) và Case 7 (Test Mạng / Giao Tiếp)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) của chương.

**Ngữ cảnh**: Bạn có một Hệ thống Ghi Log EEPROM. Lớp `Logger` (Lớp Ứng dụng) dùng đối tượng `I_EEPROM` (Lớp Interface Giao tiếp SPI phần cứng) để ghi dữ liệu.
**Yêu cầu**:
1. Trong môi trường Unit Test, người ta tạo ra một Đối Tượng Cấu Khách (Mock Object) tên là `MockEEPROM`. Nó kế thừa từ `I_EEPROM`. Nhiệm vụ của Mock Object là gì? Tại sao nó không ghi ra File mà chỉ lưu dữ liệu vào RAM?
2. Trình bày Khái niệm Expectation (Sự Kỳ vọng) trong Unit Test. Ví dụ: Khi Hàm Ứng dụng `Logger->WriteError(0x05)` chạy, Test Framework sẽ KHẲNG ĐỊNH (Assert) hàm `MockEEPROM->SPI_Write(0x05)` PHẢI ĐƯỢC GỌI đúng MỘT LẦN. Nếu không được gọi, bài Test BÁO ĐỎ (Failed). Cơ chế này giúp ích gì cho Lập trình viên?

### Exercise 3: Kiểm Thử Luồng Chuyển Trạng Thái (State Machine Test)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 4 (Test State Machine) và Case 9 (Giả Lập Mạng/NVM)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) của chương.

**Ngữ cảnh**: Cần kiểm thử máy trạng thái hoạt động của robot/máy in.
**Yêu cầu**:
1. Trình bày cách thiết kế test case để kiểm tra tính đúng đắn khi chuyển trạng thái (State Transition Table).
2. Viết mã giả mô phỏng một bài test kiểm tra trạng thái chuyển từ `READY` sang `ACTIVE` khi nhận lệnh in, và chuyển sang `FAULT` nếu cảm biến nhiệt báo quá giới hạn an toàn.

### Exercise 4: Phân Tích Đường Biên (Boundary Value Analysis - BVA) & Tiêm Lỗi
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Test Biên) và Case 6 (Kiểm Tra Mã Lỗi - Fault Injection)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) của chương.

**Ngữ cảnh**: Cảm biến Dải Laser đo khoảng cách từ `0` đến `1000` mm.
Hàm phân tích:
```c
int get_distance_status(uint16_t dist_mm) {
    if (dist_mm < 10) return CRITICAL_CLOSE;
    if (dist_mm <= 100) return WARNING_CLOSE;
    if (dist_mm <= 1000) return NORMAL;
    return SENSOR_ERROR_OUT_OF_RANGE;
}
```
**Yêu cầu**:
Áp dụng BVA (Kỹ thuật thiết kế kiểm thử bắt buộc trong ISO 26262), bạn không thể Test toàn bộ 1000 giá trị. Bạn chỉ Test các Ranh Giới (Vùng chuyển đổi trạng thái) nơi Bug rất hay trú ngụ.
Hãy liệt kê TẤT CẢ các cặp giá trị đầu vào (`dist_mm`) tối thiểu cần Test để quét sạch mọi Lỗi (Off-by-one error) của 4 nhánh `if` trên. (Ví dụ: Test giá trị 9 và 10; 100 và 101...).
Mô tả phương pháp kiểm thử tiêm lỗi (Fault Injection) khi truyền mã lỗi CRC hỏng để xác thực phản ứng của hệ thống.

### Exercise 5: Kiểm Thử Thời Gian Bằng Fake Clock & Tối Ưu Độ Phủ MC/DC
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 5 (Giả Lập Thời Gian) và Case 8 (Code Coverage & MC/DC)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/10_UnitTest/example.md) của chương.

**Ngữ cảnh**: Quyết định Kích hoạt Còi Báo Động (Alarm).
Còi báo khi: `(Nhiệt_độ > 100) VÀ (Chế_Độ_Bảo_Trì == FALSE)`.
Ký hiệu logic: `if (A && !B)`
**Yêu cầu**:
1. Trình bày phương pháp dùng Fake Time Source để tua nhanh thời gian hệ thống trong unit test nhằm kiểm tra các tác vụ timeout mà không cần chờ đợi thời gian thật.
2. Nếu bạn chỉ viết 1 Test Case với `A = True` và `B = False`, Code Coverage Statement (Độ phủ dòng lệnh) báo 100% (Vì hàm nhảy vào bên trong lệnh `if`). MC/DC Coverage sẽ báo FAILED đỏ chót. Tại sao 1 Test Case là không đủ để chứng minh độ an toàn của Phép toán Logic `&&` này?
3. Hãy liệt kê BẢNG CHÂN TRỊ (Truth Table) các Test Cases bắt buộc phải bổ sung để đạt 100% MC/DC cho lệnh `A && !B` trên. Giải thích sự cần thiết của Test Tình Huống Sai (Negative Testing).
