# Chủ Đề 6: Lập Trình Hướng Đối Tượng (OOP) - Góc Nhìn Chuyên Sâu Từ C Lên C++

## Lời Mở Đầu: Có OOP Trong Ngôn Ngữ C Thuần Túy (C-Standard) Không?
Ngôn ngữ C không có từ khóa `class`, `public`, `private` hay `virtual`. Nhưng "Lập trình hướng đối tượng" (OOP) là một Tư Duy Kiến Trúc, không phải là tính năng bắt buộc của ngôn ngữ. Trong Linux Kernel, AUTOSAR, Hệ điều hành FreeRTOS, lập trình viên sử dụng 100% ngôn ngữ C nhưng code theo phong cách OOP cực kỳ chặt chẽ.

Hiểu cách C thực thi OOP sẽ giúp bạn vỡ lẽ được toàn bộ "Phép thuật" (Magic) ẩn giấu bên dưới C++ và tối ưu hóa bộ nhớ MCU đến từng Byte.

---

## 1. Đóng Gói (Encapsulation) Trong C - Kỹ Thuật Opaque Pointer
**Vấn đề**: Trong C, cấu trúc `struct` mặc nhiên phơi bày tất cả mọi thứ ra công chúng (như `public` trong C++). Bất kỳ ai cũng có thể sửa biến trong struct của bạn.
```c
struct Motor { uint8_t speed; }; // Kém an toàn, ai cũng sửa được speed.
```

**Giải pháp Chuyên Sâu (Opaque Pointer - Con trỏ mờ)**:
- File `motor.h`: Chỉ KHAI BÁO TÊN cấu trúc, không khai báo nội dung. Cung cấp các hàm (API) điều khiển.
```c
// File: motor.h
typedef struct Motor_t Motor_t; // Tiền khai báo (Forward Declaration). Không lộ ruột!

Motor_t* Motor_Create(void);          // Hàm khởi tạo (Constructor)
void Motor_SetSpeed(Motor_t* me, uint8_t spd); // Hàm Getter/Setter
```

- File `motor.c`: Nơi đây mới ĐỊNH NGHĨA cấu trúc thật.
```c
// File: motor.c
struct Motor_t {
    uint8_t speed;    // Thuộc tính này hoàn toàn "Vô hình" đối với các file khác!
    uint32_t address;
};

void Motor_SetSpeed(Motor_t* me, uint8_t spd) {
    if (me != NULL && spd <= 100) me->speed = spd; // Kiểm soát an toàn (Encapsulation)
}
```
Lợi ích cực lớn: Bất kỳ lập trình viên nào include `motor.h` mà cố tình gõ `my_motor->speed = 200;` thì Trình biên dịch sẽ báo lỗi *Incomplete Type*. Biến `speed` đã được bảo vệ hoàn hảo bằng C!

---

## 2. Kế Thừa (Inheritance) Bằng C Struct Embedding
Làm sao để tạo một `DcMotor` (Động cơ DC) kế thừa các tính năng của một `BaseMotor` (Động cơ cơ sở)?
**Kỹ Thuật**: Nhúng Struct Cha lên VỊ TRÍ ĐẦU TIÊN của Struct Con.

```c
// Lớp Cha (Base)
typedef struct {
    uint32_t id;
    bool is_running;
} BaseMotor_t;

// Lớp Con (Derived)
typedef struct {
    BaseMotor_t base; // BẮT BUỘC NẰM Ở ĐẦU (Offset = 0)
    uint8_t pwm_channel;
} DcMotor_t;

DcMotor_t my_motor;
// Con truy cập thuộc tính Cha
my_motor.base.is_running = true; 
```
**Bản chất Bộ nhớ (Casting Trick)**: Vì `base` nằm ở Byte số 0 (Offset 0) của `DcMotor_t`. Bạn hoàn toàn có thể Ép kiểu con trỏ một cách an toàn:
`BaseMotor_t* p_base = (BaseMotor_t*)&my_motor;`
Kỹ thuật này được dùng ở cốt lõi của các thư viện Đồ họa nhúng như LVGL (Light and Versatile Graphics Library).

---

## 3. Đa Hình (Polymorphism) Trong C Với Bảng Con Trỏ Hàm (Function Pointer Table)
Làm sao để gọi hàm `TurnOn()` của 1 thiết bị, mà nếu thiết bị đó là Motor thì quay, thiết bị là LED thì sáng? (Runtime Polymorphism).
Đây chính là HAL (Hardware Abstraction Layer).

**Kỹ Thuật: Bảng Phương Thức (VTable trong C)**
```c
// 1. Định nghĩa Interface (Giao diện API)
typedef struct {
    void (*turn_on)(void* instance);
    void (*turn_off)(void* instance);
} IDevice_VTable_t;

// 2. Struct thiết bị chứa Con trỏ tới Bảng Interface
typedef struct {
    const IDevice_VTable_t* vtable; // Con trỏ tới Bảng Hàm Ảo
    void* instance_data;            // Dữ liệu nội bộ
} IDevice_t;

// 3. Hàm kích hoạt Đa hình (Polymorphism)
void Device_TurnOn(IDevice_t* dev) {
    if (dev != NULL && dev->vtable->turn_on != NULL) {
        dev->vtable->turn_on(dev->instance_data); // GỌI NHẢY HÀM TRỰC TIẾP
    }
}
```

---

## 4. Chuyển Đổi Lên C++: Đánh Đổi Lớn (Trade-Offs) Trong Hệ Nhúng
C++ làm các việc ở Mục 1, 2, 3 bằng các từ khóa có sẵn (`private`, `class`, `virtual`). Cực kỳ nhanh, cú pháp gọn.
Nhưng tại sao Code lõi Nhúng (Linux, Hệ điều hành nhúng) vẫn hay viết bằng C?

### A. Từ Khóa `virtual` (Chi Phí Của Bảng Hàm Ảo - VTable)
Mỗi class C++ dùng `virtual` sẽ âm thầm tốn RAM/ROM giống hệt cách C làm ở Mục 3:
- Tốn Flash lưu cái Bảng (`vtable`).
- Tốn RAM thêm 4 bytes vào MỌI Object (Lưu cái `vptr` trỏ tới Bảng).
- Lệnh gọi hàm `virtual` sẽ bắt CPU thực hiện 2 lần Nhảy bộ nhớ (Indirection/Pointer Chasing), làm Pipeline của ARM gián đoạn, **làm chậm tốc độ thực thi**. Cấm dùng `virtual` trong các vòng lặp tính toán khắt khe (VD: PID Control Motor 10kHz).

### B. Hàm Khởi Tạo Ẩn Dật (Implicit Constructor Execution)
Trong C, bạn khởi tạo mọi thứ khi bạn gọi `Init_Hàm()`. Rất rõ ràng trình tự.
Trong C++, nếu bạn khai báo Object ở biến Global: `Led my_led;`
Hàm khởi tạo (Constructor) của `my_led` sẽ chạy TRƯỚC KHI HÀM `main()` BẮT ĐẦU. Nếu trong Constructor bạn cấu hình Thanh ghi GPIO, MCU sẽ sập ngay vì Lúc đó xung nhịp (System Clock) chưa được bật!
**Khắc phục ở C++ Nhúng**: Constructor KHÔNG BAO GIỜ cấu hình phần cứng. Phải tạo thêm 1 hàm `my_led.begin()` và tự gọi trong `main()`.

### C. Con Trỏ `this` - Khách Không Mời
Mọi Method trong C++ (`void Motor::setSpeed(int s)`) thực chất là một hàm C ẩn chứa một tham số `this`:
`void Motor_setSpeed(Motor* this, int s)`.
Điều này khiến bạn KHÔNG THỂ quăng thẳng một Method của Object C++ vào các Hàm gọi lại ngắt (Interrupt ISR) vì Ngắt phần cứng không biết truyền tham số `this` nào. Bắt buộc phải dùng hàm `static` hoặc bao bọc rất phức tạp.

---

## Tổng Kết
- C++ cung cấp cú pháp đóng gói và kế thừa 0 đồng (Zero-overhead). Rất tuyệt vời để chia Component.
- Nhưng sự Đa hình (Polymorphism) và Quản lý Vòng đời (Lifetime) của C++ tiềm ẩn chi phí ngầm. Trong hệ thống tài nguyên siêu nhỏ, kỹ sư giỏi sẽ viết bằng C++ nhưng kiểm soát bộ nhớ thủ công và tinh gọn bằng con trỏ hàm theo phong cách C (C-Style Object Orientation).
