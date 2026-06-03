# Chủ Đề 10: Unit Testing (Kiểm Thử Đơn Vị) Bằng C/C++ - Bí Quyết Viết Code An Toàn

## Lời Mở Đầu: Đừng Nạp Code Mù Quáng Vào Hardware Nữa!
Lập trình viên Nhúng nghiệp dư thường có vòng lặp làm việc: Viết code -> Cắm dây cáp nạp vào Mạch (Board) -> Bật máy in lên -> In thử hỏng -> Đọc Log -> Rút dây, sửa code -> Cắm dây nạp lại... Vòng lặp này tiêu tốn hàng Phút cho mỗi lần thử. Nếu thuật toán quá rắc rối (PID, Parsers), bạn có thể gỡ lỗi đến sáng mà không xong.
Kỹ sư chuyên nghiệp (Chuẩn ISO 26262/Automotive) áp dụng **Unit Test**: Bẻ nhỏ đoạn Code Toán Học/Logic ra, đem Lên PC Chạy Thử bằng Framework Test. Thời gian mỗi lần test mất đúng 10 Mili-giây (Chớp mắt). Khi mọi Logic trên PC xanh lá (Pass), mới nạp vô phần cứng (Hardware).

---

## 1. Cấu Trúc Đóng Gói Để Test Được (Design For Testability - DfT)
**Vấn đề Tồi Tệ của Code Nhúng Cổ Điển**:
```c
uint16_t check_temp() {
    uint16_t t = I2C_Read_Reg(0x40); // Hàm này Giao tiếp với I2C của Vi điều khiển!
    if (t > 100) Turn_On_Fan();      // Lệnh này kích chân GPIO!
    return t;
}
```
Đoạn code trên KHÔNG THỂ Unit Test. Vì máy tính PC của bạn (Linux/Windows) Không Có I2C1, cũng Không Có GPIO chân Fan. Chạy trên PC sẽ báo Lỗi Dịch (Compile Error).

**Nguyên Tắc Chia Tách (Dependency Injection Dịch Sang C)**:
Phải tách phần Cứng (Hardware) khỏi phần Logic (Software).
```c
// Viết lại hàm chỉ nhận DỮ LIỆU THÔ và TRẢ VỀ LỆNH. Không đụng Phần cứng!
bool logic_check_temp(uint16_t raw_temp) {
    if (raw_temp > 100) return true; // Cần bật quạt
    return false;
}
```
Bây giờ, bạn mang hàm `logic_check_temp()` lên máy tính PC chạy bằng GCC. Truyền cho nó `105`, nó trả về `true`. Truyền `90`, nó trả về `false`. Thuật toán hoàn hảo. Môi trường Hardware sau đó chỉ việc Mớm Số cho nó.

---

## 2. Kỹ Thuật Giả Lập Phần Cứng (Hardware Mocking & Stubbing)
Nếu bắt buộc phải gọi Hàm Cấu trúc I2C, SPI bên trong hàm cần Test thì sao?
Ta dùng kỹ thuật Tạo Bản Sao (Mocking / Fakes).

Trên Hệ thống Thật, hàm `SPI_Write()` gửi lệnh ra chân Chip.
Trên Môi trường Test PC, ta tự viết một hàm `SPI_Write()` Giả Mạo:
```c
// File: mock_spi.c (Chỉ dùng khi Unit Test trên PC)
uint8_t mock_spi_tx_data[100];
int mock_spi_tx_len = 0;

void SPI_Write(uint8_t* data, int len) {
    // Không gửi ra vi điều khiển, mà Copy nó vào Mảng Trống để Soi Xem Code có gọi đúng không!
    memcpy(&mock_spi_tx_data[mock_spi_tx_len], data, len);
    mock_spi_tx_len += len;
}
```
Lúc Test, sau khi gọi Hàm Ứng dụng, ta mở mảng `mock_spi_tx_data` ra xem Ứng dụng có xuất ra đúng chuỗi Byte cần thiết (Ví dụ lệnh Khởi động cảm biến) hay không. C++ có GoogleMock (gmock) tạo tự động các hàm này siêu nhanh.

---

## 3. Kiến Trúc Của Một Bài Test Hoàn Hảo: Arrange - Act - Assert
Đây là mô hình (Pattern) 3 bước để viết bất cứ Unit Test nào.
```cpp
#include <gtest/gtest.h>

TEST(ParserTest, Nhan_BanTin_Dung_TraVe_TruyCung) {
    // 1. Arrange (Dọn cỗ) - Thiết lập dữ liệu đầu vào, Cờ giả lập.
    uint8_t mock_packet[] = { 0xAA, 0x05, 0x01, 0x02, 0x55 }; // Khung UART đúng
    Payload_t output;
    
    // 2. Act (Hành động) - Thực thi hàm cần kiểm tra.
    bool status = Parse_UART_Packet(mock_packet, 5, &output);
    
    // 3. Assert (Xác nhận) - Kẻ chỉ trích. Nó phải Xanh hoặc Đỏ!
    EXPECT_TRUE(status); // Đảm bảo Hàm trả về Báo Xong (True)
    EXPECT_EQ(output.command, 0x01); // Dữ liệu phân tích ra có chuẩn không?
}
```
Framework Test sẽ in ra Console: `[  PASSED  ] ParserTest.Nhan_BanTin_Dung_TraVe_TruyCung`

---

## 4. Test Đường Biên (Boundary Value Analysis)
Bug hệ nhúng hay xảy ra nhất ở Ranh giới Mảng, Ranh giới Số, hoặc Kích thước tối đa.
Ví dụ: Pin từ 0% đến 100%. Hàm tính % pin từ số ADC 12-bit (0-4095).
Bạn phải viết Test Case cho các trường hợp:
- Dưới biên (Dưới đáy): ADC = -1 (Nếu ép sang Uint sẽ thành số tỷ), ADC = 0.
- Giữa: ADC = 2000.
- Trên biên (Chạm trần): ADC = 4095, ADC = 4096.

Nếu bạn không Test các biên này, khi ADC Nhiễu (Đọc lên 4100), Màn hình LCD có thể in ra chữ Pin 250% phá vỡ giao diện.

---

## 5. Coverage (Độ Phủ Mã) - Tiêu Chuẩn ASIL
Công cụ Coverage (như `gcov`) cho bạn biết "Bao nhiêu dòng code của bạn đã CHẠY QUA trong lúc Test".
- Nếu Code Coverage = 30%. Tức là bạn viết Test rất tệ, 70% thuật toán còn lại chưa được sờ đến, chứa đầy bom mìn.
- **Tiêu chuẩn ISO 26262** yêu cầu MC/DC Coverage 100% cho mã an toàn. Tức là Nếu code của bạn có dòng `if (A && B)`, bạn PHẢI viết đủ 3 test cases: A ĐÚNG-B ĐÚNG; A ĐÚNG-B SAI; A SAI-B ĐÚNG để chứng minh mọi đường rẽ nhánh (Branches) đều không bị lỗi chập chờn.
