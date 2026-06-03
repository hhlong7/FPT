# Chủ Đề 1: Biến, Kiểu Dữ Liệu & Thao Tác Bit (Chuyên Sâu Cho Lập Trình Nhúng)

## Lời Mở Đầu: Sự Khác Biệt Giữa C Trên PC và C Trên Vi Điều Khiển (MCU)
Dành cho học viên mới nắm C cơ bản: Khi bạn học C trên máy tính, RAM có hàng Gigabytes, CPU chạy ở tốc độ GHz, hệ điều hành (Windows/Linux) quản lý mọi thứ. Bạn khai báo `int a = 5;`, chương trình chạy trơn tru.
Nhưng trong lập trình nhúng (Ví dụ làm Máy in, IoT Node bằng chip STM32, ESP32):
- **RAM rất nhỏ**: Chỉ từ vài KB đến vài trăm KB. Một khai báo biến lãng phí cũng có thể làm sập hệ thống.
- **Không có Hệ điều hành bảo vệ**: Viết sai địa chỉ con trỏ, CPU sẽ treo cứng (Hard Fault).
- **Làm việc trực tiếp với phần cứng**: Bạn dùng C để điều khiển trực tiếp dòng điện ở các chân chip (Pins), đọc cảm biến.

Chủ đề này sẽ đập đi xây lại nền tảng C của bạn, hướng tới chuẩn **MISRA C** (Tiêu chuẩn code an toàn cho hệ thống nhúng quan trọng như ô tô, hàng không).

---

## 1. Mối Nguy Hiểm Của Kiểu Dữ Liệu Tiêu Chuẩn (`int`, `long`)
Trong C cơ bản, bạn thường xuyên dùng `int`, `long`, `char`.
**Sự thật**: Chuẩn ngôn ngữ C không quy định `int` phải có bao nhiêu byte! Nó chỉ quy định `int` phải chứa được giá trị từ -32767 đến 32767 (ít nhất 2 bytes).
- Trên Arduino (chip 8-bit AVR): `int` là **2 bytes**.
- Trên STM32 (chip 32-bit ARM): `int` là **4 bytes**.
- Trên PC Windows 64-bit: `int` là **4 bytes**, nhưng `long` có thể là 4 hoặc 8 bytes.

**Hậu quả**: Nếu bạn viết code giao tiếp mạng IoT. Bạn gửi một biến `int` từ Arduino (2 bytes) lên Server (4 bytes). Dữ liệu sẽ bị lệch khung (misaligned), sinh ra giá trị rác.

### Giải pháp chuyên sâu: `<stdint.h>` (Bắt buộc dùng 100%)
Trong hệ nhúng, chúng ta **cấm** dùng `int`, `long`, `short`. Phải dùng kiểu dữ liệu có chiều rộng cố định (Fixed-width integers).

| Kiểu dữ liệu | Kích thước | Khoảng giá trị | Ứng dụng thực tế trong Nhúng |
| :--- | :--- | :--- | :--- |
| `uint8_t` | 1 byte (8 bits) | 0 đến 255 | Dữ liệu UART, cờ trạng thái, % PWM động cơ (0-100), mảng đệm (buffer) |
| `int8_t` | 1 byte | -128 đến 127 | Tọa độ nhỏ, độ chênh lệch nhiệt độ. |
| `uint16_t` | 2 bytes (16 bits) | 0 đến 65,535 | Đọc ADC (0-4095), đếm chu kỳ Timer, PWM độ phân giải cao. |
| `int16_t` | 2 bytes | -32,768 đến 32,767 | Dữ liệu cảm biến gia tốc, con quay hồi chuyển (MPU6050). |
| `uint32_t` | 4 bytes (32 bits) | 0 đến 4,29 tỷ | Đếm thời gian Mili-giây (uptime_ms), thanh ghi 32-bit của chip ARM. |

*Luôn thêm hậu tố `U` cho hằng số không âm*:
```c
uint32_t baud_rate = 115200U; // Chữ U báo cho compiler đây là Unsigned
```

---

## 2. Thao Tác Bit (Bitwise Operations) - "Vũ Khí" Của Lập Trình Nhúng
Trên MCU, để bật 1 bóng LED nối ở Chân số 5 (Pin 5) của Cổng A (Port A), bạn không có hàm `turn_on_led()`. Bạn phải lật bit số 5 của thanh ghi bộ nhớ về số `1`.
Các phép toán Bitwise: `&` (AND), `|` (OR), `^` (XOR), `~` (NOT), `<<` (Dịch trái), `>>` (Dịch phải).

### Kỹ thuật 1: Set Bit (Bật 1 bit lên 1, giữ nguyên các bit khác)
**Quy tắc**: Dùng phép `OR` (`|`)
```c
// Ví dụ: Bật chân số 5.
// 1U << 5 tạo ra số nhị phân: 0010 0000
PORTA_REGISTER |= (1U << 5); 
```

### Kỹ thuật 2: Clear Bit (Tắt 1 bit về 0, giữ nguyên các bit khác)
**Quy tắc**: Dùng phép `AND` (`&`) với đảo bit (`~`)
```c
// Tắt chân số 5. 
// ~(1U << 5) tạo ra số: 1101 1111. Khi AND, bit 5 sẽ về 0.
PORTA_REGISTER &= ~(1U << 5);
```

### Kỹ thuật 3: Toggle Bit (Đảo trạng thái bit, 0 thành 1, 1 thành 0)
**Quy tắc**: Dùng phép `XOR` (`^`)
```c
// Chớp nháy LED ở chân 5
PORTA_REGISTER ^= (1U << 5);
```

### Kỹ thuật 4: Check Bit (Kiểm tra xem bit đó là 0 hay 1)
**Quy tắc**: Dùng phép `AND` (`&`)
```c
// Kiểm tra cảm biến chạm ở chân số 3
if (PORTA_REGISTER & (1U << 3)) {
    // Chân 3 đang ở mức HIGH (1)
}
```

---

## 3. Từ Khóa `volatile` - Vượt Mặt Trình Biên Dịch
Học C cơ bản, bạn có thể chưa từng thấy từ khóa này. Trong nhúng, không hiểu `volatile` là nguyên nhân của 50% các lỗi "chạy chập chờn" (Heisenbugs).

**Bản chất của Trình biên dịch (Compiler Optimizer)**:
Trình biên dịch rất thông minh. Khi bạn chọn chế độ Tối ưu hóa (ví dụ `-O2`), nó cố gắng làm chương trình chạy nhanh nhất có thể bằng cách đưa dữ liệu từ RAM lên thanh ghi (Register) của CPU và giữ ở đó.

**Vấn đề**:
Giả sử bạn có biến `uint8_t flag_nhan_du_lieu = 0;`.
Ngắt UART (Hardware Interrupt) cập nhật `flag_nhan_du_lieu = 1;` khi có tin nhắn từ Wifi.
Hàm `main()` của bạn có vòng lặp chờ:
```c
while (flag_nhan_du_lieu == 0) {
    // Đợi dữ liệu...
}
```
*Lỗi xảy ra*: Trình biên dịch thấy vòng lặp `while` không làm thay đổi biến `flag_nhan_du_lieu`. Nên nó nạp giá trị `0` vào CPU 1 lần duy nhất, rồi lặp vô tận. Khi Ngắt UART đổi biến này thành `1` ngoài RAM, CPU không hề biết! Máy bị treo (Kẹt vòng lặp).

**Giải pháp**:
```c
volatile uint8_t flag_nhan_du_lieu = 0;
```
Từ khóa `volatile` hét vào mặt trình biên dịch: *"Dữ liệu này có thể bị thay đổi bởi phần cứng hoặc ngắt bất cứ lúc nào! KHÔNG ĐƯỢC TỐI ƯU HÓA. Hãy ra tận RAM để đọc nó ở mỗi vòng lặp!"*

**Khi nào PHẢI dùng `volatile`?**
1. Biến được chia sẻ giữa Ngắt (ISR) và luồng chính (Main).
2. Con trỏ trỏ tới thanh ghi phần cứng (Hardware Registers).

---

## 4. Từ Khóa `const` - RAM và ROM trong MCU
Trong C cơ bản, `const` chỉ báo lỗi nếu bạn cố sửa biến.
Trong Hệ nhúng, `const` có ý nghĩa **vật lý** liên quan đến cấu trúc bộ nhớ (Harvard Architecture).

MCU có 2 loại bộ nhớ chính:
- **Flash/ROM**: Dung lượng lớn (Ví dụ 256KB). Lưu trữ code, không bị mất khi mất điện.
- **SRAM**: Dung lượng cực nhỏ (Ví dụ 32KB). Lưu biến cục bộ, biến toàn cục, mất sạch khi mất điện.

```c
// Không có const: Chuỗi này ban đầu lưu ở Flash, nhưng khi khởi động máy, 
// nó được COPY sang RAM. Tiêu tốn 21 bytes RAM vô ích!
char welcome_msg[] = "Welcome to 3D Printer";

// Có const: Chuỗi này ở VĨNH VIỄN trong Flash ROM. CPU đọc trực tiếp từ Flash. 
// Tiết kiệm được 21 bytes RAM!
const char welcome_msg[] = "Welcome to 3D Printer";
```
**Quy tắc**: Mọi hằng số, bảng tra cứu (Lookup table), chuỗi văn bản (Strings), firmware version... BẮT BUỘC phải thêm `const`.

---

## 5. Hiện Tượng Chuyển Đổi Kiểu Tự Động (Integer Promotion) - "Cạm bẫy" C
C có một luật ngầm rất kỳ cục: **Mọi phép tính toán học với các kiểu nhỏ hơn `int` (như `char`, `uint8_t`, `uint16_t`) đều tự động bị ép kiểu lên `int` trước khi tính!**

```c
uint8_t a = 200U;
uint8_t b = 100U;
uint8_t c;

// Trong C cơ bản, bạn nghĩ: a + b = 300, vượt quá uint8_t (max 255), nên tràn bit thành 44.
// NHƯNG sự thật: C ép 'a' và 'b' thành số 'int' (32-bit), cộng lại thành 300 (không bị tràn).
// Sau đó mới gán 300 trở lại vào 'c' (uint8_t), lúc này mới cắt bỏ bit dư thành 44.
c = a + b; 
```

**Sự cố nguy hiểm (MISRA Rule Violation)**:
```c
uint16_t a = 50000U;
uint16_t b = 50000U;
uint32_t result = a + b; // EXPECTED: 100,000
```
Nếu MCU của bạn là 16-bit (như MSP430, `int` là 16-bit):
- C ép `a` và `b` lên `int` (16-bit signed).
- 50000 + 50000 = 100000 -> Tràn số `int`! Kết quả bị sai bét trước khi kịp gán vào `result` (32-bit).

**Giải pháp (Ép kiểu tường minh - Explicit Casting)**:
```c
// Ép kiểu LÊN 32-bit TRƯỚC khi thực hiện phép cộng
uint32_t result = (uint32_t)a + (uint32_t)b;
```

---

## 6. Endianness (Thứ Tự Byte)
Một biến `uint32_t` chiếm 4 bytes. Giả sử biến đó chứa giá trị `0x1A2B3C4D`. Nằm ở địa chỉ RAM `0x1000`. 4 byte này sẽ được xếp vào RAM như thế nào?

1. **Little-Endian (Hầu hết chip ARM Cortex, Intel PC)**:
   Byte nhỏ nhất (Least Significant Byte - `0x4D`) nằm ở địa chỉ nhỏ nhất (`0x1000`).
   RAM: `[0x1000]=4D`, `[0x1001]=3C`, `[0x1002]=2B`, `[0x1003]=1A`.

2. **Big-Endian (Giao thức mạng TCP/IP, một số chip PowerPC)**:
   Byte lớn nhất (Most Significant Byte - `0x1A`) nằm ở địa chỉ nhỏ nhất.
   RAM: `[0x1000]=1A`, `[0x1001]=2B`, `[0x1002]=3C`, `[0x1003]=4D`.

**Tại sao phải quan tâm?**
Khi vi điều khiển Little-Endian của bạn gửi 1 số `uint32_t` lên Server qua Internet (luôn dùng Big-Endian). Bạn phải hoán đổi vị trí các byte, nếu không Server nhận `0x4D3C2B1A` thay vì `0x1A2B3C4D`. (Sử dụng hàm `htonl()` / `ntohl()`).
