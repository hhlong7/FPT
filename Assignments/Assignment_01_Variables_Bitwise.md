C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment01> TYPE: <Embedded_C> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 1: Variables, Data Types & Bitwise in Embedded Systems

### Exercise 1: Kích Thước Biến và Mạng IoT (Endianness & Fixed-width)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Fixed-width), Case 3 (Memory Optimization) và Case 10 (Struct Padding & Network Payloads)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/01_Variables_DataTypes/example.md) của chương.

**Ngữ cảnh**: Bạn đang lập trình một cảm biến nhiệt độ (IoT Node) bằng chip 8-bit AVR. Thiết bị này đo nhiệt độ và gửi dữ liệu lên một Máy chủ Gateway chạy chip 32-bit ARM.
**Vấn đề**: Kỹ sư cũ định nghĩa gói tin giao tiếp như sau:
```c
struct SensorData {
    int temperature; 
    long timestamp;
};
```
Khi cảm biến đo được nhiệt độ `25°C`, nó gửi gói tin lên Gateway. Tuy nhiên, Gateway giải mã ra nhiệt độ là `1638425` độ C!
**Yêu cầu**:
1. Phân tích nguyên nhân lỗi dựa trên kích thước kiểu `int` và `long` giữa 2 hệ thống (8-bit vs 32-bit) và hiện tượng Endianness.
2. Sửa lại struct `SensorData` bằng cách bắt buộc sử dụng thư viện `<stdint.h>`.
3. Viết một hàm C ngắn gọn để đóng gói `temperature` (giả sử là số nguyên 16-bit) thành mảng 2 byte truyền đi sao cho không phụ thuộc vào Endianness (Ví dụ: truyền Big-Endian).

### Exercise 2: Phòng Tránh Heisenbug Bằng Từ Khóa `volatile`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Reading a Hardware Switch - volatile)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/01_Variables_DataTypes/example.md) của chương.

**Ngữ cảnh**: Trong phần mềm máy in, một ngắt Timer (ISR) sẽ tự động đếm giây và cập nhật biến `system_uptime`. Main loop sẽ chờ khi nào đủ 5 giây thì bắt đầu nung nóng đầu in.
```c
uint32_t system_uptime = 0;

void Timer_ISR() { // Chạy mỗi 1 giây
    system_uptime++;
}

void start_heating_process() {
    // Vòng lặp chờ đủ 5 giây
    while (system_uptime < 5) {
        // Đợi...
    }
    Heater_TurnOn();
}
```
**Vấn đề**: Khi biên dịch ở chế độ Debug (`-O0`), máy in chạy bình thường. Khi biên dịch để xuất xưởng chế độ Release (`-O2`), máy in bị đơ vĩnh viễn ở vòng lặp `while` và không bao giờ bật lò sưởi.
**Yêu cầu**:
1. Giải thích chi tiết tại sao Trình biên dịch (Compiler Optimizer) lại tạo ra vòng lặp vô hạn ở chế độ `-O2`.
2. Sửa mã nguồn trên chỉ bằng cách thêm ĐÚNG MỘT TỪ KHÓA. Giải thích ý nghĩa của từ khóa đó đối với phần cứng CPU.

### Exercise 3: Tiết Kiệm RAM Màn Hình Bằng Khai Báo `const`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 4 (Storing Font Data - const)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/01_Variables_DataTypes/example.md) của chương.

**Ngữ cảnh**: Màn hình máy in 3D cần hiển thị các chuỗi Cảnh báo.
```c
char msg_warning1[] = "Canh bao: Nhiet do cao!";
char msg_warning2[] = "Canh bao: Ket nhua o dau in!";
char msg_warning3[] = "Canh bao: Mat ket noi Wifi!";
```
**Yêu cầu**:
1. 3 chuỗi trên được lưu trữ ở đâu trên MCU sau khi khởi động? (Gợi ý: Section `.data`, `.bss`, `.text` hay `.rodata`?).
2. Việc sử dụng mảng ký tự không có từ khóa `const` gây lãng phí bộ nhớ nào? Vì sao?
3. Sửa lại khai báo trên để tiết kiệm tối đa dung lượng SRAM của chip. Giải thích luồng di chuyển dữ liệu lúc khởi động (Boot).

### Exercise 4: Điều Khiển Heater Của Máy In 3D Bằng Bitwise
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 6 (Bitwise Flags for System State)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/01_Variables_DataTypes/example.md) của chương.

**Ngữ cảnh**: Máy in 3D có một thanh ghi Điều khiển Trạng thái Phần cứng (Hardware Status Register) 8-bit nằm tại địa chỉ `0x40021000`.
Ý nghĩa các bit (từ 0 đến 7):
- Bit 0: Bật/Tắt quạt tản nhiệt (Fan)
- Bit 1: Bật/Tắt đầu đùn nhựa (Heater)
- Bit 2: Bật/Tắt bàn nhiệt (Bed)
- Bit 4-7: Lưu mã lỗi (Error Code từ 0 đến 15)

**Yêu cầu**:
Không sử dụng phép tính cộng/trừ/nhân/chia. Chỉ dùng toán tử Bitwise (`|`, `&`, `~`, `^`, `<<`, `>>`):
1. Viết lệnh C để CHỈ BẬT đầu đùn nhựa (Heater), giữ nguyên trạng thái các thiết bị khác.
2. Viết lệnh C để CHỈ TẮT quạt tản nhiệt, giữ nguyên các thiết bị khác.
3. Đầu in vừa phát hiện kẹt nhựa, hãy viết lệnh để Ghi Mã Lỗi số 5 (Nhị phân: 0101) vào 4 bit cao nhất của thanh ghi mà không làm tắt các quạt/heater đang chạy.
4. Viết lệnh đọc thanh ghi này ra và tách riêng (extract) mã lỗi thành một biến `uint8_t current_error`.

### Exercise 5: Tràn Số Hệ Thống Uptime & Integer Promotion
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 5 (Time/Uptime), Case 7 (ADC 12-bit), Case 8 (Magic Numbers) và Case 9 (Avoiding Signed/Unsigned Mismatch)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/01_Variables_DataTypes/example.md) của chương.

**Ngữ cảnh**: Cần tính toán bước nhảy vận tốc (Step pulse) cho động cơ bước.
```c
uint16_t pulse_a = 50000;
uint16_t pulse_b = 50000;

// Hệ thống tính tổng số xung cần chạy
uint32_t total_pulses = pulse_a + pulse_b;
```
**Vấn đề**: Đoạn code này được nạp vào một chip 16-bit (nơi kiểu `int` mặc định là 16-bit). Kết quả của biến `total_pulses` thu được là một con số sai lệch hoàn toàn so với `100000`.
**Yêu cầu**:
1. Dựa vào nguyên tắc "Integer Promotion" của C, giải thích tại sao phép cộng `pulse_a + pulse_b` lại bị tràn bộ nhớ trước khi kịp gán vào `total_pulses`.
2. Khắc phục lỗi này bằng kỹ thuật "Ép kiểu tường minh" (Explicit Casting).