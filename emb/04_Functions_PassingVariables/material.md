# Chủ Đề 4: Hàm (Functions) & Truyền Biến (Passing Variables) Chuyên Sâu

## Lời Mở Đầu: Đằng Sau Lời Gọi Hàm (Function Call Overhead)
Khi bạn gọi một hàm `tinh_toan(a, b);`, CPU không tự nhiên dịch chuyển đến hàm đó. Nó phải:
1. Lưu lại vị trí lệnh hiện tại (Return Address) vào Stack.
2. Lưu các biến `a`, `b` vào các thanh ghi CPU (Register) hoặc tống lên Stack.
3. Nhảy đến địa chỉ của hàm `tinh_toan()`.
4. Khi hàm xong, ném kết quả vào thanh ghi, dọn dẹp Stack, và Nhảy ngược trở về.

Quá trình này gọi là **Context Switch / Overhead**. Trong các hệ nhúng chạy vi điều khiển chậm (ví dụ 16MHz), nếu bạn chia nhỏ logic thành quá nhiều hàm bé tí và gọi liên tục trong vòng lặp siêu tốc, thời gian (Overhead) để gọi hàm còn lớn hơn thời gian chạy tính toán!

---

## 1. Cơ Chế Truyền Biến Của ARM Cortex-M (ABI)
ABI (Application Binary Interface) là bộ luật quy định cách CPU truyền biến.
Trên họ vi điều khiển phổ biến nhất thế giới (ARM Cortex-M):
- **4 tham số đầu tiên** (`arg1` đến `arg4`) sẽ được truyền Cực Nhanh qua các thanh ghi lõi của CPU (`R0`, `R1`, `R2`, `R3`).
- **Từ tham số thứ 5 trở đi**, CPU bắt buộc phải đẩy chúng vào RAM (Stack), và hàm con phải móc từ RAM ra để đọc. Rất chậm!

**Luật Tối Ưu (Embedded Rule)**:
- Hạn chế tối đa số lượng tham số của một hàm (Không vượt quá 4).
- Nếu hàm có 10 tham số (VD: Cấu hình Động cơ), hãy gom chúng lại thành 1 `struct` (Struct Cấu trúc), sau đó truyền Con trỏ (`*`) hoặc Tham chiếu (`&`) của Struct đó vào. Hàm chỉ mất đúng 1 thanh ghi (`R0`) để chứa địa chỉ con trỏ!

```c
// CỰC KỲ TỆ: Nhồi nhét 6 tham số. Tham số t5, t6 bị đẩy xuống Stack (RAM).
void init_motor(int speed, int accel, int kp, int ki, int kd, int timeout);

// RẤT TỐT: Gom vào Struct và truyền Con Trỏ. Chỉ mất 1 thanh ghi.
typedef struct {
    int speed; int accel; int kp; int ki; int kd; int timeout;
} MotorCfg_t;

void init_motor(const MotorCfg_t* cfg); // 'const' bảo vệ struct khỏi bị hàm con sửa lén.
```

---

## 2. Truyền Biến: Value vs Pointer vs Reference

### Truyền Bằng Giá Trị (Pass by Value)
- Tạo ra 1 BẢN SAO (Copy) của biến đặt lên Stack. Hàm con sửa bản sao, biến gốc không đổi.
- Dùng cho: Số nguyên nhỏ (`uint8_t`, `int32_t`, `float`, `bool`).
- **Cấm**: KHÔNG truyền Struct lớn (VD: Struct cấu hình 200 bytes) bằng Value! Việc chép 200 bytes lên Stack sẽ làm cạn kiệt Stack và tốn thời gian CPU copy dữ liệu.

### Truyền Bằng Con Trỏ (Pass by Pointer `*` - C/C++)
- Hàm chỉ nhận "Địa chỉ" của biến. Hàm con lấy địa chỉ này, thay đổi trực tiếp dữ liệu gốc (In-place modification).
- Dùng cho: Truyền mảng (Array), trả về nhiều giá trị, hoặc giao tiếp với Hardware Buffer.
- **Rủi ro C**: PHẢI luôn kiểm tra `if (ptr == NULL)`. Nếu quên, MCU có thể sập khi chạy.

### Truyền Bằng Tham Chiếu (Pass by Reference `&` - Chỉ có ở C++)
- Y hệt như Con trỏ về mặt hiệu năng (Không tốn RAM copy), nhưng Cú pháp như biến thường.
- Cực kỳ an toàn: Tham chiếu trong C++ **KHÔNG THỂ NULL**. Bạn không cần viết lệnh kiểm tra NULL rườm rà. Lập trình viên C++ Nhúng luôn ưu tiên Reference thay cho Pointer (trừ khi tương tác mảng).

---

## 3. Khái Niệm Hàm Tái Nhập (Reentrant Functions) - Tử Huyệt RTOS
Một hàm được coi là **Tái Nhập (Reentrant)** nếu nó có thể bị Ngắt (Interrupt) cắt ngang khi đang chạy dở dang, một Task/Ngắt khác gọi lại chính hàm đó, mà mọi thứ vẫn trả về kết quả đúng đắn, không dẫm đạp dữ liệu lên nhau.

**Hàm KHÔNG TÁI NHẬP (Non-Reentrant - Rất Nguy Hiểm)**:
1. Hàm sử dụng biến Toàn cục (Global) hoặc biến Tĩnh (Static Local) mà không tắt ngắt bảo vệ.
2. Hàm gọi tới phần cứng (Ví dụ: `printf()` gọi UART).

```c
// HÀM KHÔNG TÁI NHẬP (Cực kỳ lỗi trong RTOS)
char* format_time(uint32_t unix_sec) {
    static char buffer[32]; // Dùng biến STATIC! Sống vĩnh viễn ở .bss
    sprintf(buffer, "Time: %d", unix_sec);
    return buffer;
}
```
**Giải thích thảm họa**: Luồng Main gọi `format_time(100)`, hàm chạy được nửa dòng `sprintf`, chữ "Time: 1" vừa ghi vào `buffer`. Bỗng nhiên Ngắt Timer xảy ra! Trong Ngắt Timer gọi lại `format_time(200)`, hàm lại lấy cái `buffer` tĩnh đó xài và ghi đè chữ "Time: 200". Hết ngắt, CPU quay lại luồng Main, chữ ở luồng Main biến thành rác "Time: 200".

**Cách Khắc Phục (Luôn Đẩy Trách Nhiệm Cấp Phát Cho Kẻ Gọi Hàm)**:
```c
// Hàm này là Reentrant 100% an toàn. Mỗi luồng tự truyền buffer của riêng nó vào.
void format_time(uint32_t unix_sec, char* out_buffer, size_t max_len) {
    snprintf(out_buffer, max_len, "Time: %d", unix_sec);
}
```

---

## 4. Con Trỏ Hàm (Function Pointers) - "Callback" Kiến Trúc C
Trong C, không có Class và Đa hình (Polymorphism) như C++. Kỹ thuật Con Trỏ Hàm (Lưu địa chỉ của 1 hàm vào 1 biến) là cốt lõi để tạo ra Sự Lập Lịch (Scheduler), State Machine (Máy trạng thái), và Event-Driven (Sự kiện).

```c
// Định nghĩa một kiểu dữ liệu mới: Con trỏ trỏ tới hàm void không tham số
typedef void (*ButtonCallback_t)(void);

ButtonCallback_t on_btn_press = NULL;

// Hàm đăng ký sự kiện (Lớp Ứng dụng gọi)
void register_button_event(ButtonCallback_t cb) {
    on_btn_press = cb;
}

// ISR Xử lý Ngắt Nút Bấm (Lớp Phần cứng - Nằm sâu bên dưới)
void EXTI0_IRQHandler() {
    if (on_btn_press != NULL) {
        on_btn_press(); // Cực kỳ linh hoạt, phần cứng không cần biết nó đang gọi hàm gì của Ứng dụng!
    }
}
```

---

## 5. Inline Functions (Tối Ưu Hóa Tốc Độ Thay Cho MACRO)
Trong C cũ, lập trình viên rất thích dùng `#define MAX(a,b) ((a)>(b)?(a):(b))` để tránh Context Switch của hàm. Tuy nhiên, MACRO cực kỳ nguy hiểm, nó không kiểm tra kiểu dữ liệu, nếu bạn gọi `MAX(i++, j++)` nó sẽ cộng `i` lên 2 lần!

**Giải pháp C/C++ chuẩn**: Từ khóa `inline`.
Khi đặt `inline` trước hàm, Trình biên dịch sẽ copy toàn bộ ruột hàm dán đè lên nơi gọi nó (Y hệt MACRO), loại bỏ hoàn toàn Overhead.

```cpp
// Vừa type-safe (an toàn kiểu), vừa KHÔNG tốn chi phí gọi hàm.
inline uint16_t get_max(uint16_t a, uint16_t b) {
    return (a > b) ? a : b;
}
```
**Cảnh báo**: Chỉ dùng `inline` cho hàm CỰC NGẮN (1-3 dòng). Nếu dùng `inline` cho hàm dài, và gọi ở 100 nơi, mã máy (Flash .bin) sẽ phình to gấp 100 lần (Code Bloat).

---

## 6. Lệnh Đệ Quy (Recursion) - "Cấm Địa" Hệ Nhúng
Đệ quy là hàm gọi lại chính nó (VD: Tính giai thừa, Duyệt cây thư mục SD Card).
- **Luật MISRA (Bắt Buộc)**: TUYỆT ĐỐI CẤM ĐỆ QUY.
- **Lý do**: Mỗi lần hàm gọi lại chính nó, hệ thống phải cấp thêm bộ nhớ Stack mới cho tham số và biến cục bộ. Trong khi PC có RAM vô hạn, vi điều khiển chỉ có 2KB Stack. Đệ quy sẽ làm Stack Overflow cắm phập vào `.bss` và giết chết hệ thống một cách im lặng. Hãy luôn chuyển đổi thuật toán Đệ quy thành Vòng lặp `while/for` (Iterative Algorithm).
