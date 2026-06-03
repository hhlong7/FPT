# Chủ Đề 11: C++ Nâng Cao Trong Môi Trường Nhúng (Advanced Embedded C++)

## Lời Mở Đầu: Triết Lý Zero-Overhead Của Bjarne Stroustrup
Bjarne (Cha đẻ C++) có câu nói nổi tiếng: *"What you don't use, you don't pay for"* (Cái gì bạn không dùng, bạn không bị tính phí dung lượng). C++ không chậm hơn C. Nó chỉ chậm nếu Lập trình viên sử dụng các Cấu trúc Trừu tượng (Abstraction) NẶNG NỀ mà không hiểu bản chất bên dưới.
Nếu sử dụng đúng đắn, C++ thậm chí còn TỐI ƯU HÓA tạo ra File Binary (.bin) GỌN HƠN C nhờ cơ chế `constexpr`, `inline` và Template mạnh mẽ.

---

## 1. Move Semantics (`&&`) Trong Việc Quản Lý Bộ Nhớ Hạn Hẹp
Sự thay đổi vĩ đại nhất của C++11 là Move Semantics (Ngữ nghĩa Di chuyển). 
**Vấn đề Cũ**: Trong C hoặc C++ cổ điển, nếu bạn có một gói tin UART lớn (1024 Bytes), và bạn truyền nó từ Hàm Gửi (Receive Layer) sang Lớp Network (Network Layer), Mảng đó bị Copy (Sao chép từng byte). Máy tính RAM vài Gigabyte thì không sao, Hệ nhúng tốn vài Mili-giây để chép 1KB và ngốn mất 1KB RAM dự trữ.
**Kỹ thuật Move (Cướp quyền sở hữu)**:
```cpp
class BigDataBuffer {
    uint8_t* ptr_data; // Con trỏ trỏ tới vùng đệm
public:
    // Move Constructor (Dấu && chỉ định rvalue - đối tượng tạm)
    BigDataBuffer(BigDataBuffer&& victim) noexcept {
        this->ptr_data = victim.ptr_data; // CƯỚP ĐỊA CHỈ CON TRỎ! (Chỉ mất 4 bytes, SIÊU NHANH)
        victim.ptr_data = nullptr;        // Phá bỏ sự liên kết của nạn nhân để chống Dangling.
    }
};
// Cách gọi: NetworkLayer.Process( std::move(my_large_buffer) );
```
Với `std::move`, ta chuyển giao 1024 bytes Xuyên qua 10 Lớp Kiến Trúc phần mềm mà KHÔNG MỘT VÒNG LẶP COPY NÀO XẢY RA! Quản lý siêu sạch, không rò rỉ RAM (No Memory Leak).

---

## 2. Các Tính Năng BỊ CẤM / HẠN CHẾ KHẮT KHE (MISRA/AUTOSAR)
Bạn mang Kiến trúc Nhúng đi Phỏng vấn, hãy mạnh dạn tuyên bố Bỏ Đi các tính năng C++ sau:

### A. Ngại Lệ (Exceptions - `try`, `catch`, `throw`)
- **Tại sao cấm?**: 
  1. Exception làm đảo lộn hoàn toàn dòng chảy tuyến tính. Bạn không thể dự đoán được Maximum Execution Time (Thời gian Tối đa Hàm Chạy) => Hủy hoại Hệ Thống Thời Gian Thực (RTOS).
  2. Bật Exceptions làm Trình Biên Dịch âm thầm gắn thêm hàng loạt Bảng Unwind (Xử lý khi văng lỗi) vào Flash ROM. Kích thước Code có thể tăng thêm 10-30% hoàn toàn Vô ích.
- **Sử Dụng Gì Thay Thế?**: Return mã lỗi C-Style (`int status`) hoặc sử dụng bao bọc như `std::optional` (C++17) và `std::expected` (C++23) có cơ chế xử lý lỗi an toàn ngay tại dòng lệnh không có Overhead.

### B. Standard Template Library (STL) Gây Cấp Phát Động
- Những thứ như `std::vector`, `std::string`, `std::map`.
- Bên trong hàm sinh ra của chúng gọi lệnh `new` và `delete`. Điều này làm phân mảnh (Fragmentation) Heap của Hệ Nhúng. Khiến máy Crash sau vài tiếng chạy.
- **Thay Thế Bằng**: Các thư viện "Static Allocation STL" như **ETL (Embedded Template Library)**. Nó cung cấp `etl::vector<int, MAX_SIZE>`, hoạt động y hệt C++ chuẩn nhưng cấu trúc Cố định 100% (No Malloc).

---

## 3. Quản Lý Ngắt Bằng `std::atomic` (An Toàn Xuyên Mạch)
Học hệ nhúng cơ bản, bạn dùng biến Toàn cục (Global) rồi gắn `volatile` để 2 luồng (ISR và Main) nói chuyện với nhau. Vẫn có rủi ro nếu thao tác đọc/ghi mất nhiều xung nhịp Clock.
Sang C++11, `std::atomic` giúp "Khóa cứng" hoạt động đó vào 1 Lệnh duy nhất ở cấp độ Phần cứng (Lock-free Assembly instruction).

```cpp
#include <atomic>
std::atomic<bool> is_data_ready{false};

void ISR_RX() {
    // Ép Trình biên dịch tạo Lệnh Đặt Cờ Tức Thì và Thông Báo cho mọi Luồng (Memory Order)
    is_data_ready.store(true, std::memory_order_release);
}

void Task() {
    // Vừa Kiểm Tra Vừa Xóa Cờ trong ĐÚNG 1 Lệnh Hội Thoại (Atomically), không bị cắt ngang
    if (is_data_ready.exchange(false, std::memory_order_acquire)) {
        // Đọc data...
    }
}
```

---

## 4. Range-Based `for` Loops Khắc Phục Lỗi Chặn Biên (Out-Of-Bounds)
Một nửa số Bug mảng (Array) của C là gõ nhầm điều kiện `<` thành `<=` ở vòng lặp `for(int i=0; i <= size; i++)`. Gây tràn bộ đệm (Buffer Overflow).
C++11 làm cho việc duyệt mảng an toàn 100% bằng cơ chế Mới.

```cpp
std::array<uint16_t, 5> adc_values = {100, 200, 300, 400, 500};

// Reference (&) đảm bảo KHÔNG COPY dữ liệu mảng, auto tự hiểu kiểu uint16_t
// Vòng lặp TỰ ĐỘNG CHUẨN XÁC Số Vòng bằng Đúng Độ Dài Mảng. KHÔNG BAO GIỜ TRÀN.
for (const auto& adc : adc_values) {
    print(adc); 
}
```

---

## 5. Từ Khóa `noexcept` - Tối Ưu Hóa Tận Cùng
Khi một hàm được gắn mác `noexcept`, bạn đang HỨA với Trình biên dịch rằng *"Tôi đảm bảo hàm này Không Bao Giờ Ném Ra (Throw) Các Biến Lệ Ngoại"*.
Nhờ vào lời cam kết này, Trình Biên Dịch (GCC/Clang) sẽ CẮT BỎ toàn bộ lớp Mã Lệnh Phòng Ngự (Guards Code) bao quanh lời gọi hàm đó. File Mã máy nhúng của bạn sẽ Lập Tức Nhỏ Hơn và Tốc Độ Nhảy Hàm được Tối Đa Hóa. Rất cần thiết trên Các Hàm Tiện Ích Đọc/Ghi Phần Cứng đơn giản.
