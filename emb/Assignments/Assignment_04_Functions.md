C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment04> TYPE: <Embedded_C> LOC: <Lines of Code> DURATION: <240 minutes> |
| --- | --- |

## Topic 4: Functions, Parameter Passing & Reentrancy

### Exercise 1: Tối Ưu Hóa Chi Phí Gọi Hàm (ARM ABI)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 2 (Reading I2C Sensor - Out Parameter), Case 3 (Optimizing Parameter Passing - ARM Cortex-M ABI) và Case 9 (Read-Only Struct Passing)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/04_Functions_PassingVariables/example.md) của chương.

**Ngữ cảnh**: Hàm điều khiển quỹ đạo in 3D (Kinematics) tính toán rất nhiều tham số truyền vào:
```c
// Hàm tính toán 6 tọa độ, gọi 10,000 lần mỗi giây
void calc_kinematics(float x, float y, float z, float e, float feedrate, float accel) {
    // Math...
}
```
**Vấn đề**: Bộ xử lý ARM Cortex-M truyền tham số cho hàm qua 4 thanh ghi nội bộ (R0, R1, R2, R3). Từ tham số thứ 5 trở đi (`feedrate`, `accel`), CPU buộc phải ghi vào RAM (Stack) rồi hàm lại móc ra từ RAM, quá trình này tốn nhiều chu kỳ máy và làm chậm nghiêm trọng quỹ đạo chuyển động.
**Yêu cầu**:
1. Hãy thiết kế lại (Refactor) khai báo hàm này sao cho quá trình truyền dữ liệu CHỈ tiêu thụ đúng 1 thanh ghi của CPU (Sử dụng kỹ thuật truyền Struct qua Con trỏ/Tham chiếu).
2. Viết đoạn code (C hoặc C++) gọi hàm thiết kế mới của bạn. Giải thích tại sao phải dùng thêm từ khóa `const` ở tham số truyền vào.

### Exercise 2: Rủi Ro Hàm Không Tái Nhập (Non-Reentrant Function) Trong RTOS
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 4 (Danger of Global Variables vs. Parameters) và Case 5 (Reentrancy Bug in an IoT Node)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/04_Functions_PassingVariables/example.md) của chương.

**Ngữ cảnh**: Cần một hàm để tạo chuỗi thời gian (Timestamp) đính kèm vào file Log.
```c
char* format_timestamp(uint32_t seconds) {
    static char time_buf[20]; // Dùng static để chuỗi không biến mất khi thoát hàm (Tránh Dangling Pointer)
    sprintf(time_buf, "Time: %lu", seconds);
    return time_buf;
}
```
Trên một hệ điều hành thời gian thực (FreeRTOS), Task A (Độ ưu tiên thấp) gọi hàm này. Khi hàm `sprintf` chạy được một nửa, Ngắt Timer (Độ ưu tiên cực cao) xen ngang và gọi lại chính hàm `format_timestamp()` để lưu log lỗi.
**Yêu cầu**:
1. Định nghĩa khái niệm Hàm Tái Nhập (Reentrant).
2. Phân tích chi tiết hiện tượng "Ghi đè rác" xảy ra bên trong biến `time_buf` do sự xen ngang của Task ưu tiên cao (Race Condition).
3. Viết lại hàm `format_timestamp` để nó trở thành Hàm Tái Nhập 100% an toàn cho Đa luồng (Thread-safe) bằng cách loại bỏ biến `static` và đẩy trách nhiệm cung cấp Bộ đệm (Buffer) cho Hàm Gọi (Caller).

### Exercise 3: Chống Tràn Ngăn Xếp Bằng Đệ Quy (Recursion Ban)
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 10 (Stack Overflow from Recursion)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/04_Functions_PassingVariables/example.md) của chương.

**Ngữ cảnh**: Trong giao diện Màn hình LCD của Máy in có cấu trúc Menu cây đa cấp. Khi người dùng bấm nút "Quay lại" (Back), hàm tự gọi lại chính nó để vẽ lại Menu cha.
```c
void draw_menu(MenuNode_t* current_node) {
    // Hiển thị menu
    // Chờ nút bấm...
    if (btn_pressed == BACK) {
        draw_menu(current_node->parent); // Đệ quy!
    }
}
```
**Vấn đề**: Nếu người dùng bấm lùi ra vô lại liên tục 50 lần. Máy in bị sập.
**Yêu cầu**:
1. Dựa trên cơ chế Stack Frame của Lời Gọi Hàm, giải thích vì sao đệ quy phá hủy hệ nhúng có Stack giới hạn (vd 2KB).
2. Theo chuẩn MISRA C, đệ quy bị nghiêm cấm. Hãy viết lại cấu trúc điều hướng Menu trên thành Dạng Vòng Lặp Trạng Thái (Iterative State Machine / While-Loop) để mức tiêu thụ Stack luôn duy trì cố định ở mức O(1).

### Exercise 4: Con Trỏ Hàm (Callback / Function Pointers) Tạo Scheduler Siêu Nhẹ
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 1 (State Machine Update Function) và Case 6 (Callback Functions - Function Pointers)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/04_Functions_PassingVariables/example.md) của chương.

**Ngữ cảnh**: Bạn cần thiết kế một Trình Lập Lịch (Task Scheduler) rất cơ bản (Không dùng RTOS) trên C thuần. Cứ mỗi 10ms thì chạy Task A, 50ms thì chạy Task B.
```c
typedef struct {
    uint32_t interval_ms;
    uint32_t last_run_ms;
    // THIẾU: Con trỏ hàm trỏ tới Task cần thực thi
} Task_t;
```
**Yêu cầu**:
1. Khai báo bổ sung thêm 1 trường dữ liệu (field) vào `Task_t` là một Con Trỏ Hàm (Function Pointer) không trả về giá trị, không tham số đầu vào. Tên trường là `execute`.
2. Khai báo mảng 2 `Task_t` chứa 2 task tương ứng với chu kỳ 10ms và 50ms.
3. Viết một hàm Main Loop (`while(1)`) quét qua mảng Task. Nếu thời gian hiện tại (`current_ms`) đã qua đủ `interval_ms` thì GỌI Con trỏ hàm thực thi Task đó.

### Exercise 5: Hàm Nội Tuyến (Inline Functions) vs Macro `#define`
> [!TIP]
> **Hướng dẫn tự học:** Để giải quyết bài tập này hiệu quả nhất, hãy đọc kỹ và phân tích mẫu code tại **Case 7 (Inline Getter/Setter for Register Encapsulation) và Case 8 (Array Decay in Functions)** trong tệp [example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/04_Functions_PassingVariables/example.md) của chương.

**Ngữ cảnh**: Tính năng đảo bit trạng thái đèn LED được kỹ sư cũ viết bằng C-Macro:
```c
#define TOGGLE_PIN(port, pin)  (port ^= (1 << pin))
```
**Vấn đề**: Macro bỏ qua kiểm tra kiểu dữ liệu (Type-checking) và có thể gây lỗi đánh giá lặp (Multiple Evaluation) nếu biểu thức bên trong truyền vào phép tăng tự động (VD: `TOGGLE_PIN(port, i++)`).
**Yêu cầu**:
1. Viết lại Macro trên thành một hàm C++ `inline`.
2. Trình bày sự giống và khác nhau giữa `inline function` và C-Macro trong giai đoạn biên dịch. Việc dùng `inline` quá nhiều cho hàm lớn ảnh hưởng như thế nào đến kích thước File Firmware (.bin)?
