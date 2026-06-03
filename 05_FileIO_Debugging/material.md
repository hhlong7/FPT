# Chủ Đề 5: Giao Tiếp I/O (File & Peripherals) & Gỡ Lỗi (Debugging) Chuyên Sâu

## Lời Mở Đầu: Sự Thay Thế Của `stdio.h`
Trên máy tính, `printf()` in ra màn hình Terminal, `fopen()` đọc file trên ổ cứng. Máy tính có Hệ điều hành lo toàn bộ tầng vật lý.
Trên Vi điều khiển (Bare-metal MCU), không có màn hình, không có ổ cứng. Các hàm trong `<stdio.h>` nếu gọi trực tiếp sẽ không chạy, hoặc làm phình to bộ nhớ ROM (Flash) một cách lãng phí (thêm 20KB-40KB mã máy) vì kéo theo một loạt các thư viện chuẩn nặng nề.

---

## 1. Cơ Chế I/O (Input/Output) Của Hệ Nhúng

### A. Polling (Bỏ Phiếu / Hỏi Vòng)
- **Cách hoạt động**: CPU liên tục đọc một cờ (Flag) của Thanh ghi phần cứng (Ví dụ: `while(UART_RX_FLAG == 0);`) để chờ nhận 1 byte dữ liệu.
- **Ưu điểm**: Code cực kỳ dễ viết, không cần cấu hình ngắt.
- **Khuyết điểm (Tử huyệt)**: **Blocking (Nghẽn CPU)**. Khi CPU kẹt trong vòng lặp `while` chờ dữ liệu, nó mất hoàn toàn khả năng làm việc khác (Quét nút bấm, điều khiển động cơ). Tốc độ UART 9600 baud mất khoảng 1ms để nhận 1 ký tự. Trong 1ms đó, CPU 72MHz đã lãng phí mất 72,000 chu kỳ tính toán!
- **Khi nào dùng?**: Chỉ dùng khi Khởi động máy (Boot), in dòng chữ chào mừng, hoặc trong các vòng lặp lỗi Hệ thống (HardFault) đã tắt hết mọi chức năng khác.

### B. Interrupt (Ngắt Phần Cứng)
- **Cách hoạt động**: CPU chạy công việc chính (Main Loop). Khi phần cứng (UART, SPI) nhận/gửi xong dữ liệu, nó sẽ giật một đường dây điện báo hiệu cho CPU. CPU tạm dừng Main Loop, nhảy vào hàm Xử lý ngắt (ISR - Interrupt Service Routine), thao tác rất nhanh, rồi quay lại Main Loop.
- **Ưu điểm**: Không nghẽn CPU (Non-blocking). CPU chạy song song hiệu quả. Tốc độ đáp ứng Cực Nhanh (Real-time).
- **Quy tắc Vàng (MISRA & Real-time)**: 
  - Hàm ISR BẮT BUỘC PHẢI SIÊU NGẮN! Chỉ được copy dữ liệu vào Hàng Đợi (Queue/Ring Buffer) và dựng Cờ báo hiệu (Flag). 
  - TUYỆT ĐỐI CẤM gọi `printf()`, các hàm dùng vòng lặp `delay()`, vòng lặp `while` chờ tín hiệu, hay tính toán thuật toán phức tạp bên trong ISR. Nếu ISR chạy quá lâu (hơn vài chục Microsecond), hệ thống sẽ bỏ lỡ các ngắt khác quan trọng hơn (VD: Ngắt dừng động cơ khẩn cấp).

### C. DMA (Direct Memory Access - Truy cập bộ nhớ trực tiếp)
- **Đỉnh cao của I/O Nhúng**: DMA là một lõi vi xử lý phụ (Coprossesor) chuyên làm nhiệm vụ bốc vác dữ liệu. Bạn cấu hình: "Ê DMA, hãy lấy 1024 mẫu từ cảm biến ADC chép vào Mảng RAM này, khi nào xong thì báo".
- **Ưu điểm**: CPU không tốn 1 chu kỳ nào để copy dữ liệu. CPU nằm ngủ (Sleep Mode tiết kiệm pin) hoặc làm việc khác. Dùng DMA cho Màn hình TFT LCD, Cảm biến Âm thanh (I2S).

---

## 2. Kỹ Thuật "Retarget" Hàm `printf()` (Chuyển Hướng Gỡ Lỗi)
Làm sao để `printf("Error: %d", code)` in ra cổng Serial UART (Đưa lên máy tính xem bằng Hercules/PuTTY)?
Bạn phải viết đè (Override) lại hàm Giao tiếp mức thấp nhất của Trình biên dịch (Low-level I/O).
Với GCC (ARM-GCC): Viết đè hàm `_write()`.
Với Keil ARMCC: Viết đè hàm `fputc()`.

```c
// Ví dụ trên GCC (STM32/ESP)
int _write(int file, char *ptr, int len) {
    // Gọi hàm truyền UART Polling (Blocking) hoặc truyền DMA ở đây
    for (int i = 0; i < len; i++) {
        Hardware_UART_SendByte(ptr[i]); 
    }
    return len;
}
```
**Lưu ý Khắt Khe**: Trong hệ thống quan trọng (Safety-critical), lập trình viên sẽ tự viết hàm `Log_Print()` không dùng `<stdio.h>` để tối ưu tuyệt đối bộ nhớ và kiểm soát luồng chạy (VD: `printf` dùng `malloc` ngầm trên một số thư viện, gây rò rỉ bộ nhớ).

---

## 3. Hệ Thống Tập Tin (File System) Bằng FatFs
Hệ nhúng không có ổ C:, ổ D:. Flash chip chỉ là một dải ô nhớ nhị phân liên tục. Để tạo File (`.txt`, `.csv`), bạn phải nhúng một Thư viện Hệ thống Tập tin (File System Library). Phổ biến nhất là **FatFs** (Của ChaN) hoặc **LittleFS** (Chuyên cho NOR Flash vì có Wear-leveling).

```c
// FatFs Example
FIL log_file;
if (f_open(&log_file, "log.txt", FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
    f_printf(&log_file, "Nhiet do: 35C\n");
    f_sync(&log_file); // BẮT BUỘC: Đẩy dữ liệu từ RAM xuống vật lý (SD Card/Flash). Quên hàm này, cúp điện sẽ mất file.
    f_close(&log_file);
}
```
**Nút cổ chai (Bottleneck)**: Ghi thẻ SD / Flash tốn vài Mili-giây (Vô cùng chậm so với CPU). KHÔNG BAO GIỜ được gọi hàm ghi File bên trong Ngắt (ISR) hay các Task có độ ưu tiên cao (Real-time Motor Control). Hãy đẩy Data vào một Queue, và để một Task rảnh rỗi ưu tiên thấp (Idle Task) ghi ra thẻ nhớ từ từ.

---

## 4. Các Phương Pháp Debug (Gỡ Lỗi) Tiên Tiến

### Phương pháp 1: Debug Phần Cứng (Hardware Debugger JTAG/SWD)
Đây là sự khác biệt lớn nhất giữa việc "Viết code trên giấy" và "Làm Kỹ sư Nhúng". Bạn sử dụng mạch nạp ST-Link/J-Link cắm vào MCU.
- **Breakpoints (Điểm dừng)**: Lệnh cho CPU chạy đến đúng dòng code số 45 và TẠM DỪNG cứng ngắc toàn bộ phần cứng.
- **Registers View**: Trực tiếp xem các con số trong thanh ghi R0, R1, PC (Program Counter) của Chip.
- **Call Stack (Trình theo dõi ngăn xếp)**: Nếu MCU bị rơi vào `HardFault_Handler` (Chết máy), mở cửa sổ Call Stack, trình gỡ lỗi sẽ lật ngược lại quá khứ, báo chính xác Hàm A gọi Hàm B, Hàm B gọi Hàm C, và dòng code nào trong Hàm C làm sập hệ thống (VD: Con trỏ NULL).

### Phương pháp 2: Gỡ Lỗi Qua Serial Wire Viewer (SWO / ITM)
In `printf()` qua UART rất tốn tài nguyên (Mất vài ms). CPU ARM Cortex-M có lõi ITM chuyên biệt. Nó đẩy dữ liệu `printf` thẳng qua dây nạp SWD lên máy tính với tốc độ Mega-bytes/giây, Overhead của MCU cực thấp (gần bằng 0). Đây là kỹ thuật chuyên sâu thay thế hoàn toàn cho "UART Debug".

### Phương pháp 3: GPIO Toggling + Máy Hiện Sóng (Oscilloscope / Logic Analyzer)
Khi bạn cần biết vòng lặp PID của Robot chạy hết bao nhiêu Micro-giây, `printf` không thể đo được (vì thời gian in còn lâu hơn chạy PID).
**Giải pháp Siêu Cấp Vật Lý**:
```c
void PID_Control() {
    Pin_High(PORT_A, PIN_1); // Bật chân điện áp lên 3.3V
    
    // ... Toán học PID phức tạp (Nhân, chia float) ...
    
    Pin_Low(PORT_A, PIN_1);  // Kéo chân điện áp xuống 0V
}
```
Kẹp que đo Máy Hiện Sóng vào chân A1. Bạn sẽ thấy một xung hình vuông. Đo độ rộng của xung vuông đó trên màn hình máy (VD: 54us). Con số này chính xác tuyệt đối ở mức Nano-giây. Đây là cách Test độ trễ Real-time chuẩn nhất.

---

## 5. Từ Khóa `assert()` - Bẫy Côn Trùng Lúc Viết Code
Assert dùng để kiểm tra logic của Lập trình viên, không dùng để bắt lỗi phần cứng.
- Khi điều kiện bên trong `assert` trả về `false`, nó sẽ Cố Tình Gọi Hàm `abort()` làm treo MCU và in ra dòng code bị sai.
- Rất tốt để phát hiện lỗi ngay lúc chạy thử (Unit Test, Integration).
- **Sản xuất (Release)**: Khi xuất xưởng máy in, người ta biên dịch với cờ `-DNDEBUG`, toàn bộ các lệnh `assert` trong code sẽ bốc hơi (Biến thành khoảng trắng), không tốn ROM, không làm sập máy khách hàng.

```c
void tinh_toc_do(uint8_t divisor) {
    // Nếu thằng nào gọi hàm này mà truyền số 0, giáng đòn tử hình nó ngay lúc Debug.
    assert(divisor != 0); 
    
    uint16_t speed = 1000 / divisor;
}
```