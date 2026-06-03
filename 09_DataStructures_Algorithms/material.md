# Chủ Đề 9: Cấu Trúc Dữ Liệu & Thuật Toán (CTDL & TT) - Giải Pháp Tĩnh (Static Base) Cho Hệ Nhúng

## Lời Mở Đầu: Sự Kìm Kẹp Của Bộ Nhớ (Memory Constraints)
Trên máy tính, thuật toán thường đo bằng độ phức tạp Thời gian (Big O). Việc Cấp phát (Allocate) 1 Triệu Node của Cây (Tree) hoặc Danh sách liên kết (Linked List) chỉ tốn vài chục phần nghìn giây.
Trên Vi điều khiển (Ví dụ: STM32F103 RAM 20KB), KHÔNG CÓ KHÁI NIỆM MUA THÊM RAM! Thuật toán của bạn bị đo lường cực gắt bởi Độ phức tạp Không gian (Space Complexity). Bạn gọi `malloc()` 100 lần, RAM bị Phân mảnh (Fragmentation), khối liên tục bị phá vỡ, máy chết đứng. Do đó, CTDL Nhúng đi theo triết lý riêng: **KHÔNG CẤP PHÁT ĐỘNG, MỌI THỨ PHẢI TĨNH (STATICALLY ALLOCATED)**.

---

## 1. Mảng Tĩnh (Static Arrays) & Bộ Nhớ Vòng (Ring Buffer / Circular Queue)
### Tại sao lại cấm `std::vector` và Danh sách Liên kết (Linked List)?
- `std::vector` tự động nhân đôi kích thước bằng `new`/`malloc` khi đầy. Trên vi điều khiển, cấp phát 10KB rồi lại nới lên 20KB sẽ làm bộ nhớ nhảy múa lung tung và crash do Hết RAM mảng lớn liên tục.
- Linked List: Cần con trỏ `next`. Mỗi cục dữ liệu 4 byte lại tốn thêm 4 byte cho con trỏ (Lãng phí 50% RAM). Các node phân tán khắp nơi làm hỏng bộ nhớ đệm (Cache miss).
- Mảng tĩnh (Static Arrays) truy cập `O(1)`, bộ nhớ liên tục. Là CTDL mặc định của 95% firmware.

### Cứu Tinh Của Giao Tiếp Phần Cứng: The Ring Buffer
Nếu ngắt UART bắn dữ liệu vào mảng nhanh hơn tốc độ Main Loop xử lý, mảng sẽ tràn. Nếu đẩy toàn bộ phần tử mảng lùi về sau (Shift array) sẽ tốn cực kỳ nhiều chu kỳ CPU (CPU Cycles).
**Cơ chế Ring Buffer**: 
- Dùng 1 mảng tĩnh cố định. Dùng 2 chỉ số: `head` (nơi ngắt ghi dữ liệu) và `tail` (nơi Main đọc dữ liệu).
- Khi `head` chạm mép mảng, nó Vòng Lại (Wrap around) về số 0.

**Tối ưu Hóa Toán Học Đỉnh Cao (Power of 2)**:
```c
// Chậm (Phép chia % tốn chục nhịp máy do không có Hardware Divider)
uint16_t next_head = (head + 1) % 100; 

// Siêu Tốc Bằng Toán Tử Bitwise (Chỉ dùng được nếu Kích Thước = Lũy Thừa của 2. Ví dụ: 128, 256)
#define BUFFER_SIZE 256
// 256 - 1 = 255. Nhị phân: 0000 0000 1111 1111. 
// Phép AND bitwise chỉ tốn 1 nhịp máy. Tự động Roll-over về 0 cực chuẩn!
uint16_t next_head = (head + 1) & (BUFFER_SIZE - 1); 
```

---

## 2. Tìm Kiếm Nhị Phân (Binary Search) Khắc Phục Thuật Toán Tuyến Tính
Có một thiết bị báo 300 mã lỗi (Error Codes) không theo thứ tự liên tục. Cần tra mã lỗi ra Chuỗi văn bản để hiển thị lên LCD.
**Lỗi phổ biến**: Duyệt tuyến tính (Linear Search `for (i=0 to 300)`) mất $O(N)$. Nếu quét quá lâu, Màn hình LCD hoặc Motor sẽ bị giật/lag.
**Tiêu chuẩn**: Đưa dữ liệu thành Struct Array (Mảng cấu trúc) trên ROM (Dùng `const`), sắp xếp theo ID Mã Lỗi lúc viết code. Sau đó dùng Binary Search $O(\log N)$. Tra cứu 300 mã lỗi chỉ tốn MẤT 8 BƯỚC.

---

## 3. Thuật Toán Lọc Tín Hiệu Số (Digital Filtering) Sống Còn Trong Môi Trường Nhiễu
Trong Robot hoặc Dữ liệu Cảm biến IoT, sóng nhiễu (Noise) cực lớn. Dữ liệu thô (Raw) không bao giờ dùng được ngay. Phải dùng CTDL và Toán học để lọc.

### Lọc Trung Bình Cộng (Moving Average)
Dùng 1 cái Ring Buffer lưu 10 mẫu gần nhất. Cộng lại chia 10.
- Khuyết điểm: Tốn RAM (Lưu 10 mẫu). Tốn CPU (Cộng 10 lần mỗi lúc lấy mẫu). 

### Lọc EMA (Exponential Moving Average) - "Phép Màu" Toán Học
Bỏ hoàn toàn mảng RAM. Chỉ dùng 1 biến lưu trạng thái lịch sử. Sử dụng Phép toán Toán Dấu Phẩy Tĩnh (Fixed-Point Math) để né dấu phẩy động (Float).
```c
// Trọng số Alpha: Bao nhiêu phần trăm tin vào Mẫu Mới.
// Chuyển Float 0.1 sang số nguyên 10 (trong tổng 100).
int32_t filtered_value = 0; // Biến duy nhất

void filter_adc(int32_t new_sample) {
    // 10% Tin vào cái mới, 90% Tin vào lịch sử (Quá khứ)
    // Phép nhân chia số nguyên, siêu nhanh!
    filtered_value = (new_sample * 10 + filtered_value * 90) / 100;
}
```

---

## 4. Tránh Xa Thư Viện Toán Học Nổi (Floating Point Math)
Hầu hết các vi điều khiển nhỏ (ARM Cortex-M0/M3) không có FPU (Lõi tính toán số thực). Hàm `sin()`, `cos()`, `sqrt()` sẽ tốn vài ngàn chu kỳ CPU. Một thuật toán Kinematics của tay máy Robot có thể làm sập luôn thời gian thực.
**Giải pháp thay thế Bắt Buộc**: Bảng Tra Cứu (Lookup Tables - LUT)
- Hãy mở Microsoft Excel hoặc Python, tính sẵn 360 giá trị của `sin()` từ độ 0 tới 359.
- Nhân tất cả với 1000 để làm mất Dấu Phẩy. (Vd: `0.5` thành `500`).
- Ghi vào Firmware thành `const int16_t SIN_TABLE[360] = {0, 17, 34, ...}` (Nằm trên ROM).
- Lúc chạy, `x = radius * SIN_TABLE[angle] / 1000`. Tốc độ nhanh gấp HÀNG TRĂM LẦN so với thư viện Math.

---

## 5. Danh Sách Liên Kết Xâm Nhập (Intrusive Linked List)
Bạn cần một Danh sách để quản lý các Tác vụ hẹn giờ (Timer Task). Nếu dùng Con trỏ như C++ thường, bạn phải `malloc()` ra các Node mới.
Hệ điều hành FreeRTOS giải quyết bằng **Intrusive List**:
Thay vì Tạo cái Hộp Đựng Dữ Liệu. Bạn nhét luôn 2 con trỏ `*next` và `*prev` vào thẳng cấu trúc `Task_t` của bạn. 
Lúc này, các Object được "móc nối" vào nhau bằng các con trỏ nằm rải rác. Mọi thứ Đã Có Sẵn Trải Dài Trên RAM Tĩnh. Không có 1 byte nào cần Cấp phát thêm! Đó là vẻ đẹp của Thuật toán Tĩnh (Static Algorithms).
