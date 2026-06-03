# Optimization & Common Defects - 10 Case Studies (Thực tế Embedded / IoT / Robotics)

## Case 1: Struct Padding làm rò rỉ RAM (Memory Layout)
**Ngữ cảnh**: Bạn cần tạo một mảng chứa 1000 cấu hình trạm đo (IoT Node).
**Bad Practice**:
```c
struct NodeConfig {
    uint8_t id;      // 1 byte
    uint32_t ip;     // 4 bytes
    uint8_t status;  // 1 byte
}; // Lập trình viên nghĩ Struct này nặng 6 bytes.
```
**Sự thật**: Trình biên dịch sẽ nhét thêm 3 bytes "Padding" (Đệm lề trống) sau `id` và 3 bytes sau `status` để `ip` nằm ở địa chỉ chia hết cho 4 (Căn lề phần cứng 32-bit). Kích thước thực tế là 12 bytes! Mảng 1000 phần tử tốn 12KB RAM thay vì 6KB.
**Giải pháp Tối ưu**: Sắp xếp biến từ LỚN đến NHỎ.
```c
struct NodeConfig {
    uint32_t ip;     // 4 bytes
    uint8_t id;      // 1 byte
    uint8_t status;  // 1 byte
}; // Trình biên dịch chèn 2 bytes padding cuối. Tổng: 8 bytes.
```

## Case 2: Phép nhân chia Float Tàn Phá Tốc Độ (No FPU)
**Ngữ cảnh**: Cần nội suy giá trị Điện áp Pin 3.3V từ ADC 12-bit (0-4095).
**Bad Practice**: `float voltage = (adc_raw / 4095.0f) * 3.3f;`
-> Mỗi lần gọi tiêu tốn hàng trăm CPU Cycles.
**Good Practice (Fixed-Point)**:
```c
// Tính bằng đơn vị millivolt (mV). Mọi thứ là số nguyên!
uint32_t voltage_mv = (adc_raw * 3300) / 4095; 
```
Nhanh gấp hàng chục lần, chính xác tuyệt đối ở mức millivolt.

## Case 3: Race Condition Làm Nhảy Cóc Dữ Liệu
**Ngữ cảnh**: Main Loop liên tục lấy tốc độ động cơ (`uint16_t speed`) in ra UART. Ngắt Timer (ISR) liên tục cập nhật `speed`.
**Vấn đề (Race Condition)**: Nếu biến `speed` là số 16-bit, nhưng MCU là loại 8-bit (AVR/PIC), CPU phải dùng 2 lệnh máy để đọc biến này (Đọc byte Cao, rồi Đọc byte Thấp). Lỡ lúc đọc xong Byte Cao, Ngắt ISR chạy tới ghi đè giá trị mới, khi Main Loop đọc tiếp Byte Thấp -> Ghép lại thành một con số rác khổng lồ.
**Giải pháp**: Tắt ngắt tạm thời (Critical Section).
```c
uint16_t get_speed_safe() {
    uint16_t safe_speed;
    __disable_irq(); // Cấm ngắt cắt ngang
    safe_speed = speed;
    __enable_irq();  // Mở lại ngắt
    return safe_speed;
}
```

## Case 4: Treo Máy Do Quên Khởi Tạo Pointer
**Ngữ cảnh**: Viết driver I2C cho Màn hình OLED.
**Bad Practice**: 
```c
uint8_t* tx_buffer;
void I2C_Send() {
    tx_buffer[0] = 0xAA; // Lỗi: tx_buffer đang trỏ vào một vùng nhớ vô định!
}
```
**Hậu quả**: Hard Fault (Treo CPU do chạm vào vùng nhớ cấm).
**Giải pháp**: Luôn gán `= nullptr` và thêm kiểm tra `if (tx_buffer != nullptr)` trước khi thao tác.

## Case 5: Biến Tạm Khổng Lồ Gây Tràn Stack
**Ngữ cảnh**: Hàm phân tích ảnh BMP cho Máy In nhiệt.
**Bad Practice**:
```c
void process_image() {
    uint8_t line_buffer[4096]; // Cấp phát 4KB lên Stack!
    // Làm việc...
}
```
**Giải pháp**: Nếu chip chỉ có 8KB RAM, gọi hàm này chắc chắn gây Stack Overflow. Cần chuyển mảng thành mảng `static uint8_t line_buffer[4096];` để đẩy nó sang vùng `.bss`, hoặc cấp phát biến toàn cục.

## Case 6: Quên Từ Khóa `volatile` Của Trạng Thái (Heisenbug)
**Ngữ cảnh**: Task đợi một cờ (Flag) từ ISR.
**Bad Practice**:
```c
bool is_done = false;
void ISR() { is_done = true; }
void Task() {
    while (!is_done) { /* Đợi */ } // Vòng lặp bị kẹt Vĩnh Viễn!
}
```
**Giải thích**: Trình biên dịch khi Tối ưu hóa (Optimization `-O2`) thấy vòng lặp `while` chả làm gì thay đổi biến `is_done`, nó sẽ nạp biến này vào thanh ghi CPU 1 lần duy nhất rồi kiểm tra mãi thanh ghi đó. Khi ISR đổi `is_done` ngoài RAM, CPU không hề biết.
**Giải pháp**: Khai báo `volatile bool is_done = false;` để bắt CPU phải ra RAM lấy dữ liệu Mới ở MỖI vòng lặp.

## Case 7: Rò Rỉ Tài Nguyên Phần Cứng (Hardware Resource Leak)
**Ngữ cảnh**: Hàm mở một kênh Timer, xử lý lỗi, rồi thoát sớm.
**Bad Practice**:
```c
void play_sound() {
    Timer_Start();
    if (error) return; // Quên tắt Timer!
    Timer_Stop();
}
```
**Hậu quả**: Timer cứ chạy mãi, chiếm dụng ngắt và tốn pin.
**Giải pháp**: Dùng RAII C++ (Nhúng vào Class cục bộ) hoặc cấu trúc `goto cleanup;` chuẩn của Hệ nhúng C.

## Case 8: Code Cồng Kềnh Do `printf`
**Ngữ cảnh**: In log báo lỗi trong MCU nhỏ (STM32G0 32KB Flash).
**Bad Practice**: `printf("Error: %d\n", err_code);`
-> Vừa thêm hàm `printf`, Flash nhảy vọt lên 25KB, sắp cạn bộ nhớ.
**Good Practice**: Tự viết hàm `Print_Int(int val)` đơn giản, chỉ dùng phép chia `/ 10` để tách số nguyên thành chuỗi ASCII. Tiết kiệm ngay 15KB Flash.

## Case 9: Dead-Code Chặn Tối Ưu Hóa (Switch-Case vs Lập Bảng)
**Ngữ cảnh**: Tính năng trả về Text của ngày trong tuần (Mon, Tue...).
**Bad Practice**:
```c
const char* get_day(uint8_t day) {
    switch(day) {
        case 0: return "Mon";
        // ... (viết dài dòng)
    }
}
```
**Good Practice (Tốc độ và Gọn gàng)**:
```c
const char* const DAYS[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const char* get_day(uint8_t day) {
    if (day < 7) return DAYS[day];
    return "Err";
}
```

## Case 10: Vòng Lặp Trễ Vô Hạn (Infinite Blocking)
**Ngữ cảnh**: Khởi tạo cảm biến I2C. Giao tiếp phần cứng có thể bị lỗi, dây cắm bị lỏng.
**Bad Practice**:
```c
while ((I2C1->SR & READY_BIT) == 0); // Kẹt ở đây mãi mãi nếu cảm biến cháy!
```
**Good Practice**: Luôn luôn có một biến Timeout bảo vệ các vòng lặp phần cứng (Watchdog cơ bản).
```c
uint32_t timeout = 10000;
while ((I2C1->SR & READY_BIT) == 0) {
    if (--timeout == 0) return E_TIMEOUT; // Cứu hệ thống khỏi Treo cứng.
}
```
