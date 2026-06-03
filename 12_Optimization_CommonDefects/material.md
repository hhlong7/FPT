# Chủ Đề 12: Tối Ưu Hóa & Lỗi Thường Gặp Cốt Tử Của Hệ Nhúng (Optimization & Defects)

## Lời Mở Đầu: Tội Đồ Rò Rỉ Hiệu Năng
Trong Vi xử lý máy tính, Code tồi có thể giấu đi bằng RAM khổng lồ (Nhiều GB) và Tần số xung nhịp cao (GHz). 
Nhưng trong Vi điều khiển (MCU) ARM Cortex-M0/M3 chỉ có 32KB RAM và xung nhịp 48MHz: Viết sai cấu trúc Struct làm rò rỉ nửa số lượng RAM; Dùng một phép chia số thực Float ngớ ngẩn làm treo thời gian thực của máy in. Đây là Tinh túy nhất của C Nhúng!

---

## 1. Rò Rỉ RAM do "Nhồi Nhét" Cấu Trúc (Struct Padding & Alignment)
Một kỹ sư Nhúng Khởi nghiệp viết code:
```c
struct GSensorConfig {
    uint8_t   id;           // 1 byte
    uint32_t  timestamp;    // 4 bytes
    uint16_t  value;        // 2 bytes
    uint8_t   status;       // 1 byte
};
```
Kỹ sư nghĩ Struct này tốn: 1 + 4 + 2 + 1 = **8 Bytes**. Kỹ sư tạo mảng `1000` cái.
**SỰ THẬT ĐÁNG SỢ**: MCU ARM đòi hỏi các biến 32-bit (như `uint32_t`) PHẢI được xếp ở những Địa Chỉ Bộ Nhớ chia hết cho 4 (Căn Lề - Alignment). 
Để thỏa mãn điều kiện phần cứng này, Trình biên dịch C (GCC) tự động Nhét Các Byte Trống (Padding) vào:
- Sau `id` nhét **3 bytes trống** để `timestamp` bắt đầu ở byte số 4.
- Sau `status` nhét **2 bytes trống** để tổng Struct là bội số của 4.
-> Tổng size = 1 + (3) + 4 + 2 + 1 + (1) = **12 Bytes**! Mảng 1000 phần tử lãng phí vô ích **4000 Bytes RAM**.

**Khắc Phục (Best Practice)**: Hãy luôn Sắp Xếp Biến Từ To Xuống Nhỏ.
```c
struct GSensorConfig_Optimized {
    uint32_t  timestamp;    // 4 bytes
    uint16_t  value;        // 2 bytes
    uint8_t   id;           // 1 byte
    uint8_t   status;       // 1 byte
}; // TỔNG: ĐÚNG 8 BYTES CHUẨN XÁC! KHÔNG CÓ PADDING.
```

---

## 2. Kẻ Thù Số Thực (Floating-Point) và Đỉnh Cao Toán Dấu Phẩy Tĩnh (Fixed-Point)
MCU giá rẻ Không Có Lõi Toán Số Thực Bằng Phần Cứng (FPU). Khi bạn gõ lệnh `float a = 2.5 * 3.1;`, Trình Biên Dịch âm thầm móc nối một hàm Cồng Kềnh Hàng Trăm Dòng (Software Emulation) để cố gắng làm phép toán này. Một phép chia Float tốn Khoảng 1000 Chu Kỳ Máy (CPU Cycles). Quá Đắt Đỏ!

**Bí Kíp Kỹ Sư Nhúng**: **Toán Dấu Phẩy Tĩnh (Fixed-Point Math)**.
Di chuyển Dấu Phẩy bằng cách Nhân mọi số với một Lũy thừa của 10 (Hoặc tốt hơn là Lũy thừa của 2).
*Bài toán tính Tiền điện (3.14V nhân với 0.5A):*
```c
// Cách Tối ưu bằng Số Nguyên
uint32_t voltage_mV = 3140; // mV
uint32_t current_mA = 500;  // mA
// Quyết định Đơn vị: V * A = W. Do đó mV * mA = uW (Micro-Watt).
uint32_t power_uW = voltage_mV * current_mA; 
// Kết quả 1,570,000 uW. Nếu muốn in ra màn hình:
printf("%lu.%03lu W", power_uW / 1000000, (power_uW / 1000) % 1000);
```
Không tốn bất kỳ một biến Float nào. Tính toán chỉ bằng 1 Nhịp Máy Nhân (Multiplication Instruction) siêu mượt!

---

## 3. Cạnh Tranh Dữ Liệu Lỗi Chập Chờn (Race Conditions & Heisenbug)
- Một Lỗi được mệnh danh là "Ác Mộng Nhúng".
- Main Loop liên tục in ra độ rộng Gói tin: `printf("Len: %d", packet_len);`
- Nhưng `packet_len` là biến kiểu `uint16_t` bị thay đổi liên tục bên trong Ngắt Nhận UART (ISR).
**Bản Chất Gây Lỗi**:
Trên chip 8-bit. Việc Cập nhật `packet_len = 1000` (Gồm Byte thấp và Byte Cao) tốn 2 lệnh Máy (Assembly). Giả sử Ngắt bắn vào đúng lúc CPU vừa đọc xong cái Byte Thấp, chưa kịp đọc Byte Cao. Ngắt đã chép Số Mới Đè Lên RAM. Ngắt thoát ra, CPU luồng Main bốc nốt cái Byte Cao MỚI ghép với Byte Thấp CŨ. Kết quả: Một Số Lạ Hoắc Khổng Lồ ra đời làm sập Trình Hiển Thị.

**Khắc Phục: Critical Sections (Vùng Tới Hạn / Khóa Phẫu Thuật)**
Khi tương tác với biến chung (Shared resource) lớn hơn Độ rộng thanh ghi (như Float, uint32 trên chip 8/16-bit, Struct lớn), Phải Bịt Mắt Interrupt Lại!
```c
void read_len_safe() {
    uint16_t temp_len;
    __disable_irq();     // Kéo Rào cấm Interrupt
    temp_len = packet_len; // Thao tác Xong
    __enable_irq();      // Mở rào ra
    // Dùng cái temp_len cục bộ thoải mái
}
```

---

## 4. Tối Ưu Bằng Bitwise Nhấn Chìm Các Lệnh Toán
- Thay vì Dùng Phép Chia (Chiếm 10-30 Nhịp máy): `a = b / 8;`
- Hãy Dịch Bit Phải (Chiếm 1 Nhịp Máy duy nhất): `a = b >> 3;` (2 mũ 3 = 8).
- Thay Phép lấy Dư (Modulo `%` - Cực kỳ Chậm) bằng Lệnh AND `&` (Nếu số bị chia là Lũy thừa 2):
  `a = count % 64;` biến thành `a = count & 63;`
  
---

## 5. Hiện Tượng Chết Im Lặng (Watchdog Thất Bại do Vòng Lặp Phù Thủy)
Kỹ sư cắm cờ Polling (Chờ Đợi Khẩn Cấp):
```c
void Wait_For_Flash_Erase() {
    while (SPI_FLASH_IS_BUSY()); // Vòng lặp chờ Hardware xóa xong Flash 
}
```
Việc xóa Flash của IC bên ngoài tốn 1 Giây! Nhưng Hardware Watchdog Timer (Cơ chế chống treo máy của Hệ Nhúng) đang đặt ngưỡng Reset là 500ms. CPU bị nhốt trong lệnh `while` đó nửa giây, Watchdog nghĩ máy đã treo và Đá Nút Nguồn Cứng Đơ! 
**Bài Học Xương Máu**:
Tất cả các lệnh `while()` tương tác phần cứng hoặc chờ tín hiệu Phải Luôn có Biến Timeout. Thoát ra Lập Tức nếu qua X Mili-giây để bảo toàn Mạng Sống Hệ Thống.
