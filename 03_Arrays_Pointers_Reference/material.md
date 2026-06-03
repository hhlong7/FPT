# Chủ Đề 3: Mảng, Con Trỏ & Tham Chiếu (Arrays, Pointers & Reference)

## Lời Mở Đầu: Tinh Hoa Của C/C++ Nhúng
Nếu "Biến" và "Bộ nhớ" là thân thể của vi điều khiển, thì "Con trỏ" (Pointer) chính là hệ thần kinh. Trong lập trình nhúng, bạn dùng con trỏ để làm mọi thứ: Cấu hình thanh ghi phần cứng, dịch chuyển dữ liệu lớn qua mạng (DMA), giải mã các gói tin phức tạp. Làm chủ con trỏ là điều bắt buộc của một Kỹ sư Nhúng.

---

## 1. Mảng (Arrays) & Sự Thiếu Kiểm Soát Biên (Bounds Checking)
Mảng trong C là các khối bộ nhớ liên tục (Contiguous memory). Nhờ tính liên tục, CPU có thể nhảy đến phần tử bất kỳ `A[i]` bằng một phép nhân rất nhanh: `Địa chỉ Base + (i * Kích thước kiểu)`. Tuy nhiên, mảng C có một "điểm yếu chết người".

### Cạm Bẫy Tràn Bộ Đệm (Buffer Overflow)
Khác với Python/Java, C **KHÔNG BAO GIỜ** tự động kiểm tra xem bạn có đang viết vượt ranh giới của mảng hay không.
```c
uint8_t buffer[10]; // Mảng có 10 phần tử (chỉ số 0 đến 9)
buffer[15] = 0xFF;  // NGHIÊM TRỌNG! Lập trình viên lỡ tay ghi vào phần tử thứ 15.
```
C compiler không báo lỗi! Lệnh này sẽ âm thầm ghi đè giá trị `0xFF` vào một biến vô tội nào đó nằm kế bên `buffer` trong vùng RAM. Hậu quả là máy in của bạn đột nhiên xoay động cơ loạn xạ vì biến điều khiển tốc độ bị ghi đè rác.

### Cách Khắc Phục (Chuẩn MISRA / ISO 26262)
Mọi thao tác truy cập mảng phải có vòng bảo vệ (Guard):
```c
// Không bao giờ được quên if kiểm tra điều kiện biên!
if (index < 10) { 
    buffer[index] = data; 
}
```
**Trong C++**: Nên vứt bỏ mảng C thuần túy và chuyển sang dùng `std::array<uint8_t, 10>`. Nó là cấu trúc bọc tĩnh (Static wrapper), an toàn 100%, không tốn thêm 1 byte RAM nào so với C.

---

## 2. Con Trỏ (Pointers) Chuyên Sâu
Con trỏ là một biến đặc biệt, giá trị của nó chứa **Địa chỉ bộ nhớ** (Memory Address) của một biến khác hoặc của một Thanh ghi phần cứng.

### A. Giao Tiếp Với Phần Cứng (Memory-Mapped I/O)
Trên chip ARM STM32, thanh ghi ODR (Thanh ghi điều khiển đầu ra) của Cổng GPIOA nằm cố định ở địa chỉ `0x40020014`. Muốn bật LED ở chân số 5, bạn dùng con trỏ trỏ đúng vào địa chỉ đó.
```c
// Cấu trúc kinh điển của mọi hệ nhúng C
// Ép kiểu số nguyên vật lý thành Con trỏ tới uint32_t, thêm volatile để chống tối ưu hóa.
#define GPIOA_ODR *((volatile uint32_t*)0x40020014)

GPIOA_ODR |= (1U << 5); // Ghi dữ liệu trực tiếp vào chip!
```

### B. Con Trỏ Trống (Null Pointers & Dangling Pointers)
- **NULL Pointer**: Là con trỏ trỏ về địa chỉ `0x00000000`. Ở vi điều khiển ARM, địa chỉ 0 thường chứa Code khởi động. Nếu bạn lỡ tay viết/đọc vào con trỏ NULL, CPU báo `HardFault` và reset.
  *Luật*: **LUÔN kiểm tra NULL** trước khi dùng con trỏ! (Hoặc dùng Reference trong C++).
- **Dangling Pointer (Con trỏ lủng lẳng)**: Xảy ra khi bạn trả về (return) địa chỉ của một biến cục bộ, hoặc khi vùng nhớ (Heap) đã bị giải phóng (`free`) nhưng con trỏ vẫn còn giữ địa chỉ cũ. (Tuyệt đối cấm trong Nhúng).

---

## 3. Ép Kiểu Con Trỏ (Pointer Casting) & Alignment (Căn Lề Bộ Nhớ)
Một lỗi cực kỳ phổ biến khi xử lý Dữ liệu Mạng (IoT) hoặc Khung truyền CAN (CAN Bus Frame).

### Vấn đề Strict Aliasing (Bí danh khắt khe)
Giả sử bạn nhận được 4 bytes (tạo thành số Float đo nhiệt độ) qua UART, được lưu trong mảng `uint8_t rx_buffer[10]`.
```c
// NGHIÊM TRỌNG: Ép kiểu mảng byte thành con trỏ Float
float temp = *(float*)&rx_buffer[0]; 
```
Đoạn code trên nhìn rất tiện, nhưng vi phạm quy tắc "Strict Aliasing" của ngôn ngữ C, dẫn đến 2 lỗi chết người:
1. **Lỗi Alignment**: MCU ARM Cortex-M0/M3 yêu cầu đọc một số `float` 32-bit từ địa chỉ RAM CHIA HẾT CHO 4. Nếu `rx_buffer` nằm ở địa chỉ lẻ (ví dụ `0x20000001`), lệnh ép kiểu kia sẽ gây `UsageFault` đơ CPU ngay tức khắc.
2. **Lỗi Tối ưu hóa**: Trình biên dịch C giả định rằng một con trỏ `float*` sẽ không bao giờ trỏ vào vùng nhớ của mảng `uint8_t`. Nếu bạn gạt lừa trình biên dịch, nó sẽ xuất ra mã máy sai bét.

### Giải pháp "Thần Thánh" (Memcpy)
Để ráp 4 bytes lại thành Float an toàn tuyệt đối:
```c
float temp;
// Dùng memcpy. Compiler sẽ tự động tối ưu hóa nó thành các lệnh an toàn Alignment nhất.
memcpy(&temp, &rx_buffer[0], sizeof(float)); 
```
*Lưu ý: Bạn cũng phải xử lý Endianness (Đảo byte) nếu gửi nhận từ mạng.*

---

## 4. Con Trỏ Hằng (`const` Pointers)
Trong C, `const` kết hợp với con trỏ rất dễ nhầm lẫn. Đọc từ Phải sang Trái!

- `const uint8_t* ptr;` (Data is const): Pointer có thể trỏ đi nơi khác, nhưng không thể đổi dữ liệu nó trỏ tới. (Thường dùng cho Hàm nhận chuỗi in ra màn hình).
- `uint8_t* const ptr;` (Pointer is const): Pointer bị kẹt chết ở một địa chỉ, nhưng có thể sửa dữ liệu. (Dùng cho ánh xạ Hardware Register cố định).
- `const uint8_t* const ptr;` (Cả 2 là const): Kẹt cứng, không sửa được gì. Dành cho Bảng ROM.

---

## 5. Tham Chiếu C++ (C++ References - `&`) - Con Trỏ An Toàn
Trong C, nếu bạn viết hàm thay đổi giá trị một cấu hình lớn:
```c
void update_config(Config_t* cfg) {
    if (cfg == NULL) return; // Phải tốn công check NULL.
    cfg->baud = 115200;      // Dùng dấu mũi tên ->
}
// Gọi: update_config(&my_config); (Rất rườm rà)
```

**Sang C++**, Tham chiếu (Reference) giải quyết triệt để sự rườm rà và rủi ro này. Tham chiếu là một "Bí danh" (Alias) cho một biến đã tồn tại. Nó KHÔNG BAO GIỜ có thể bị NULL!
```cpp
void update_config(Config_t& cfg) {
    // Không cần check NULL! Trình biên dịch C++ đảm bảo nó luôn hợp lệ.
    cfg.baud = 115200; // Dùng dấu chấm '.' như biến bình thường.
}
// Gọi: update_config(my_config); (Tự nhiên và sạch sẽ)
```

**Luật C++ Nhúng**: 
- Khi truyền Object/Struct lớn cho hàm Đọc (Read-only): Truyền Tham chiếu Hằng (`const Config_t&`). Không tốn RAM copy, an toàn.
- Khi truyền Object/Struct lớn cho hàm Sửa (Modify): Truyền Tham chiếu (`Config_t&`).
- CHỈ dùng Con trỏ (Pointer `*`) khi biến đó CÓ THỂ LÀ NULL hoặc để giao tiếp với mảng (Array) và thư viện C.

---

## 6. Mảng Tiêu Biến (Array Decay)
Bạn khai báo mảng tĩnh: `uint8_t sensor_data[100];`. Bạn nghĩ hàm `sizeof(sensor_data)` sẽ trả về 100? ĐÚNG.
Nhưng khi bạn truyền mảng đó vào một hàm:
```c
void process_array(uint8_t arr[100]) {
    // BẤT NGỜ CHƯA: sizeof(arr) ở đây trả về 4 (Kích thước của con trỏ 32-bit)!
}
```
**Bản chất**: Trong C, mảng khi truyền qua hàm sẽ LẬP TỨC "suy biến" (Decay) thành một Con Trỏ (`uint8_t*`). Nó mất hoàn toàn thông tin về độ dài. Lập trình viên ngây thơ dùng `sizeof(arr)` bên trong hàm sẽ tạo ra bug nghiêm trọng.
**Quy tắc Bắt Buộc**: Luôn truyền Kèm Chiều Dài (Length).
```c
void process_array(uint8_t* arr, uint16_t length);
```
*(Trong C++, dùng `std::span` hoặc truyền Reference mảng `template <size_t N> void func(std::array<uint8_t, N>& arr)` sẽ loại bỏ hoàn toàn lỗi này).*
