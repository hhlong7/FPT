C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment09> TYPE: <Embedded_DataStruct> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 9: Data Structures & Algorithms in Constrained MCU

### Exercise 1: Vì Sao `std::vector` Bị Cấm Ở Nhúng Core Control (Control ECU)?
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (Tối ưu phép cấp phát tĩnh), Case 5 (Kỹ thuật Bits packing) và Case 10 (Xử lý ngắt)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) của chương.

**Ngữ cảnh**: Bạn được giao phát triển firmware cho một Module Máy In điều khiển Trục Z. Một bạn thực tập sinh C++ viết code lưu lại lịch sử Tọa độ Mũi in:
```cpp
#include <vector>
std::vector<int> z_history;

void on_move_z(int z_pos) {
    z_history.push_back(z_pos); // Vector tự động phình to nếu hết chỗ
}
```
**Vấn đề**: Đoạn code này bị Trưởng nhóm (Senior) xóa ngay lập tức vì vi phạm nghiêm trọng Tiêu chuẩn MISRA và gây Crash tiềm tàng sau vài giờ chạy. (Chip RAM chỉ có 32KB).
**Yêu cầu**:
1. Phân tích nguyên lý hoạt động của hàm `push_back()` khi mảng Vector đầy. Nó gọi Cấp Phát Động (`new/malloc`), Sao Chép dữ liệu cũ, rồi Giải Phóng mảng cũ (`delete/free`) như thế nào?
2. Tại sao thao tác Cấp phát/Giải phóng Mảng lớn liên tục trên một cục RAM 32KB không có Thu gom Rác (No Garbage Collector) lại gây ra hiện tượng Phân Mảnh Bộ Nhớ (Memory Fragmentation)?
3. Thiết kế lại bằng một Class C++ Fixed-Capacity (Mảng Tĩnh có giới hạn), khi mảng đầy thì từ chối nạp thêm (Return false).

### Exercise 2: Ring Buffer Thần Thánh Bằng Toán Tử Bitwise
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Static Array Circular Buffer) và Case 7 (Ring Buffer Data Integrity)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) của chương.

**Ngữ cảnh**: UART có tốc độ đến 1 Mbytes/sec. Hàm Ngắt nhận (ISR) chỉ có 1 Microgiây để lấy dữ liệu bỏ vào Buffer. Bất kỳ phép Toán học nặng nào (như Phép chia Modulo `%`) cũng làm ngắt sụp đổ.
**Yêu cầu**:
1. Khai báo một Struct (C) hoặc Class (C++) tên là `RingBuffer_Fast`. Bắt buộc kích thước của Buffer phải là 256 (Lũy thừa của 2).
2. Viết hàm `void Push(uint8_t data)`. Để tính toán Vị trí Ghi Kế tiếp (Head index wrap around), tuyệt đối KHÔNG dùng phép Chia Lấy Dư `(head + 1) % 256`. Hãy sử dụng Phép AND Bitwise (`&`).
3. Giải thích tại sao Phép Toán Bitwise AND lại chỉ tốn đúng 1 Lệnh Máy CPU (1 CPU Cycle) thay vì 30 Lệnh Máy như Phép Chia phần cứng.

### Exercise 3: Exponential Moving Average (EMA) - Lọc Nhiễu Thần Tốc
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 3 (Thuật toán lọc trung bình EMA) và Case 9 (Double Buffer Stream Filtering)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) của chương.

**Ngữ cảnh**: Cảm biến Độ Ẩm biến thiên liên tục do Môi trường nhiễu. Cần một Bộ Lọc (Filter).
- Lọc Trung bình cộng (Moving Average thông thường) yêu cầu 1 mảng lưu 50 mẫu quá khứ (Tốn RAM). Và vòng lặp cộng 50 số (Tốn CPU).
- Kỹ sư Cấp cao đề xuất dùng Bộ lọc IIR (EMA) siêu nhẹ.
**Công thức EMA**:
`Giá Trị Lọc_Mới = (Giá Trị Mới * Trọng_Số) + (Giá Trị Lọc_Cũ * (1 - Trọng_Số))`
**Yêu cầu**:
1. Giả sử `Trọng_Số` là `0.1` (Tức là 10% tin tưởng vào giá trị mới, 90% giữ lịch sử). Để TRÁNH dùng Toán Thực (Float Math), hãy biến đổi công thức trên thành Toán Dấu Phẩy Tĩnh (Sử dụng Số Nguyên nhân với 100).
2. Viết hàm C `int32_t EMA_Filter(int32_t new_sample)` sử dụng Toán Số Nguyên hoàn toàn. Đánh giá sự tiết kiệm RAM (Chỉ tốn đúng 1 biến lưu trạng thái) so với Lọc mảng.

### Exercise 4: Thuật Toán Tìm Kiếm Nhị Phân (Binary Search) Bảng ROM
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 4 (Binary Search on Calibration Table) và Case 8 (Lookup Table for Fast Math)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) của chương.

**Ngữ cảnh**: Giao thức UDS (Chẩn đoán lỗi) của máy có hơn 1000 Mã lỗi (DTC - Diagnostic Trouble Codes). Mỗi lỗi có một mã ID `uint16_t`.
Dữ liệu này được nhét chết vào Flash ROM:
```c
struct DTC_Code { uint16_t id; const char* msg; };
const DTC_Code dtc_table[1000] = { {0x0001, "VoltLow"}, {0x0020, "TempHi"}, /*...*/ {0xFFFF, "Fatal"} };
// Mảng này ĐÃ ĐƯỢC SẮP XẾP TĂNG DẦN theo id.
```
**Vấn đề**: Hàm hiển thị mã lỗi đang duyệt Mảng bằng Vòng lặp Tuyến Tính (Linear Search `for i=0 to 1000`). Mất O(N) thời gian, làm Màn hình LCD khựng lại.
**Yêu cầu**:
1. Viết một hàm `const char* Search_DTC(uint16_t search_id)` bằng Thuật Toán Tìm Kiếm Nhị Phân (Binary Search).
2. Tính xem trong trường hợp Tồi tệ nhất (Worst-case), Vòng lặp Nhị Phân này sẽ chạy tối đa bao nhiêu lần cho 1000 phần tử? Tốc độ được cải thiện O(log N) như thế nào?

### Exercise 5: Memory Pool Allocator Cho Hệ Thống Network
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 6 (Memory Pool Allocator)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) của chương.

**Ngữ cảnh**: Hệ thống Cổng Mạng (IoT Gateway) LwIP liên tục nhận các Packet có kích thước khác nhau (Tối đa 128 Bytes/Packet). Do cấm dùng Heap (`malloc`), bạn phải cấp phát RAM bằng Static.
Nhưng nếu dùng 1 mảng tĩnh cố định thì không đáp ứng được nhiều Gói tin đến cùng lúc.
**Giải pháp: Mẫu Thiết Kế Memory Pool (Hồ Chứa)**
1. Mảng Tĩnh To Bự: Khai báo 1 mảng tĩnh chia sẵn thành 10 Blocks. Mỗi Block cố định 128 Bytes.
2. Quản Lý Block: Dùng một mảng Trạng thái `bool is_used[10]` để đánh dấu Block nào đang rảnh (Free), Block nào đang xài.
**Yêu cầu Thực hành**:
Viết mã giả (Pseudo-code) cho 2 hàm của Memory Pool:
- `uint8_t* Pool_Alloc()`: Quét tìm Block đang Free đầu tiên, đánh dấu nó thành Used, và trả về con trỏ trỏ tới đầu Block đó.
- `void Pool_Free(uint8_t* ptr)`: Nhận con trỏ, tìm ra số Index của Block tương ứng, đánh dấu nó trở lại thành Free.
