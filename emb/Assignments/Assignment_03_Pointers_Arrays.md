C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment03> TYPE: <Embedded_C> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 3: Arrays, Pointers & Reference

### Exercise 1: Vượt Quá Giới Hạn Mảng (Buffer Overflow) Trọng Yếu
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (UART Receive Buffer Bounds Checking) và Case 9 (Pointer Decay in String comparison)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) của chương.

**Ngữ cảnh**: Nhận dữ liệu cài đặt G-Code cho máy in qua cổng UART. Bộ đệm nhận (Receive Buffer) chỉ dài 64 bytes.
```c
uint8_t rx_buffer[64];
uint16_t rx_index = 0;

// Hàm này chạy trong Ngắt (ISR) mỗi khi có 1 byte đến
void UART_Receive_ISR() {
    uint8_t incoming_byte = UART_Read_Data_Register();
    rx_buffer[rx_index] = incoming_byte;
    rx_index++;
    
    if (incoming_byte == '\n') {
        process_gcode();
        rx_index = 0; // Reset
    }
}
```
**Vấn đề**: Kẻ tấn công (Hoặc máy tính bị lỗi) gửi liên tục 100 bytes mà không có ký tự ngắt dòng `\n`. Hệ thống in bị treo cứng.
**Yêu cầu**:
1. Trình bày chi tiết cơ chế bộ nhớ khiến hệ thống treo (Chuyện gì xảy ra khi `rx_index` đạt tới giá trị 64 và tiếp tục tăng?).
2. Sửa lại đoạn code trên để an toàn 100% (Phòng chống tràn bộ đệm). Nếu buffer đầy mà chưa nhận được `\n`, bạn sẽ xử lý thế nào cho hợp lý nhất trên Hệ nhúng?

### Exercise 2: Ép Kiểu Trực Tiếp Mạng IoT (Strict Aliasing & Alignment)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 6 (Strict Aliasing & Alignment) và Case 7 (Volatile Peripheral Pointers)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) của chương.

**Ngữ cảnh**: Cảm biến đo Nhiệt độ và Độ Ẩm đóng gói Dữ liệu thành chuỗi Byte (Dạng Little-Endian).
Gateway nhận được mảng: `uint8_t payload[8]`. Biết rằng 4 byte đầu là số thực `float` nhiệt độ, 4 byte sau là số nguyên `uint32_t` áp suất.
Kỹ sư mới viết đoạn phân tích (Parser) như sau:
```c
void parse_payload(uint8_t* payload) {
    // Dùng kỹ thuật Ép kiểu Con trỏ trực tiếp
    float temp = *(float*)&payload[0];
    uint32_t pres = *(uint32_t*)&payload[4];
}
```
**Vấn đề**: Đoạn code vi phạm chuẩn MISRA C và gây ra Hard Fault trên chip ARM Cortex-M0 nếu mảng `payload` nằm ở địa chỉ lẻ trong RAM.
**Yêu cầu**:
1. Giải thích Lỗi Alignment (Căn lề) của Hardware là gì và tại sao việc ép kiểu con trỏ này lại gây sập máy.
2. Viết lại hàm `parse_payload` một cách an toàn bằng hàm copy bộ nhớ tiêu chuẩn (đảm bảo Compiler sẽ tự động tối ưu hóa lệnh máy một cách chuẩn Alignment).

### Exercise 3: Double Buffering Cho Luồng Dữ Liệu Tốc Độ Cao
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (Double Buffering for DMA) và Case 10 (Memory Aliasing & restrict)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) của chương.

**Ngữ cảnh**: Một ADC (Analog-to-Digital Converter) đọc tín hiệu âm thanh cực nhanh (100kHz). Nó lưu mẫu qua DMA vào một mảng liên tục. Nếu Main Loop lấy mảng đó ra xử lý (mất 5ms), ADC/DMA sẽ ghi đè lên dữ liệu cũ đang được xử lý, tạo ra âm thanh rè.
**Yêu cầu thiết kế**:
Áp dụng kỹ thuật **Double Buffering** (Bộ đệm kép) và Ownership Pointers.
Viết mã giả (Pseudo-code C) mô phỏng:
1. Định nghĩa mảng đệm kép và 2 con trỏ `p_write` (thuộc về DMA) và `p_read` (thuộc về Main Loop).
2. Xử lý sự kiện Ngắt DMA Hoàn thành (DMA Transfer Complete ISR). Trong ngắt này cần tráo đổi (Swap) quyền sở hữu của 2 con trỏ như thế nào để Main Loop có thể thong thả xử lý đệm cũ?

### Exercise 4: Từ Con Trỏ Sang Tham Chiếu C++ (C++ References)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (C vs C++ Array Management) và Case 4 (Passing Struct by Reference)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) của chương.

**Ngữ cảnh**: Khởi tạo Cấu hình mạng Wifi.
Code C chuẩn cũ đang dùng con trỏ để có thể sửa đổi dữ liệu bên trong hàm:
```c
typedef struct { char ssid[32]; char pass[32]; uint8_t channel; } WifiConfig;

void load_wifi_defaults(WifiConfig* p_cfg) {
    if (p_cfg == NULL) return; // Bảo vệ
    strcpy(p_cfg->ssid, "DefaultNet");
    p_cfg->channel = 6;
}
```
**Yêu cầu**:
1. Chuyển đổi mã C trên sang chuẩn C++ sử dụng Tham chiếu (Reference `&`).
2. Giải thích lợi ích vượt trội của Tham chiếu trong trường hợp này so với Con trỏ (Nêu ít nhất 2 ưu điểm về an toàn và cú pháp).
3. Khi nào thì Bắt Buộc phải dùng Con trỏ trong C++ thay vì dùng Tham chiếu? (Nêu 1 ví dụ cụ thể liên quan đến vòng đời biến hoặc giao tiếp Hardware).

### Exercise 5: Con Trỏ Lủng Lẳng (Dangling Pointer) - Use-After-Free Nội Bộ
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 5 (Dangling Pointer) và Case 8 (Lookup Table)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/03_Arrays_Pointers_Reference/example.md) của chương.

**Ngữ cảnh**: Một hàm trong ứng dụng In ấn trả về chuỗi Cảnh báo.
```c
char* get_printer_status(int code) {
    char status_str[50];
    if (code == 0) strcpy(status_str, "Ready");
    else strcpy(status_str, "Error");
    
    return status_str; 
}

void update_ui() {
    char* current_status = get_printer_status(0);
    lcd_print(current_status); // Màn hình in ra toàn ký tự rác!
}
```
**Yêu cầu**:
1. Phân tích chi tiết vòng đời (Lifetime) của mảng `status_str` nằm trên Ngăn xếp (Stack). Vì sao lúc in ra LCD nó biến thành rác? Lỗi này có tên khoa học là gì?
2. Có 2 cách cơ bản trong lập trình nhúng để sửa lỗi này (Không dùng Cấp phát động). Hãy viết ra 2 phiên bản sửa lỗi của hàm `get_printer_status`. (Gợi ý: Dùng `static` hoặc Truyền Buffer qua tham số).
