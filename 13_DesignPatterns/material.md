# Chủ Đề 13: Mẫu Thiết Kế (Design Patterns) Cho Bare-metal & MCU Base

## Lời Mở Đầu: Lối Thoát Cho Mớ Mã Hỗn Độn
Khi dự án mới bắt đầu, 1 file `main.c` 200 dòng rất gọn gàng. Nhưng khi tính năng Máy In / IoT tăng lên (Nhận Mạng, Quản Lý Động Cơ, Đọc Thẻ SD, Lọc Nhiệt Độ, Cập Nhật Màn Hình, Lỗi Giấy, ...), đoạn code biến thành một Quái Vật Spaghetti (Spaghetti Code). Sửa nút bấm lại làm hỏng Động cơ.
Design Patterns giải cứu dự án. Tuy nhiên, các Pattern (Gang of Four) truyền thống áp dụng trên PC (Nhờ dùng `new/malloc` vô tội vạ) CẦN ĐƯỢC CHẾ BIẾN LẠI (Adaptation) cho Hệ Nhúng (Không Cấp phát Động, Bộ nhớ siêu khắt khe).

---

## 1. Mẫu Kiến Trúc Dòng Chảy Sự Kiện (Event-Driven Architecture / Queue Base)
**Đừng Nhồi Nhét Vào Ngắt (ISR) Của Bạn!**
Khi ngắt nhận Gói tin Wifi báo về: "Mở đèn Laser, Còi hú còi, Động Cơ Chạy!". Lập trình viên thiếu kinh nghiệm bỏ lệnh điều khiển vào ngay Hàm Ngắt (ISR). CPU bị kẹt trong ISR, bỏ qua Ngắt Cứu Hỏa Kế Tiếp, Toàn bộ Thời Gian Thực sập đổ.

**Pattern Chuẩn Nhúng**:
1. ISR chỉ Gắn Nhãn Sự Kiện (Tag Event) và ném vào một Hàng Đợi (Ring Buffer Queue). Trả CPU lại ngay (Vài Micro-giây).
2. Hàm Main (Hoặc Task RTOS) chạy Vòng lặp lấy Sự kiện ra:
```c
// Vòng lặp Siêu Mẫu (Super-loop / Event-Driven)
while (1) {
    if (Queue_Pop(&event_msg)) {
        switch(event_msg.id) {
            case EVT_WIFI_CMD: Process_Cmd(); break;
            case EVT_BTN_PUSH: Stop_Motor(); break;
        }
    }
}
```

---

## 2. Máy Trạng Thái Cấp Thấp Bằng Bảng Hàm (Function-Pointer State Machine)
Các Máy In / Robot lớn có hàng chục trạng thái (Khởi động -> Dò Kim -> Bơm mực -> Đang in -> Lỗi Kẹt Giấy -> In tiếp). Dùng Lệnh `switch-case` lồng 4 tầng làm code dài ngàn dòng không thể bảo trì.

**Thiết Kế Cực Đỉnh Bằng Con Trỏ Hàm C-Style**:
```c
// Mảng Danh Sách Trạng Thái: O(1) Tốc độ Truy cập, Gọn nhẹ Tuyệt Đối
typedef void (*StateFunc_t)(void);

const StateFunc_t SystemStates[] = {
    State_Boot,     // ID = 0
    State_WarmUp,   // ID = 1
    State_Printing, // ID = 2
    State_Fault     // ID = 3
};

uint8_t current_state_id = 0;

void App_Run() {
    // Gọi thẳng Hàm Xử lý của Trạng Thái Đang Hiện Tại (Tốc Độ 1 Cú Nhảy Nhịp Máy!)
    SystemStates[current_state_id](); 
}
```

---

## 3. Observer Pattern Phân Tách Trách Nhiệm (Decoupling Modules)
Mô đun `Cảm biến Pin` đo pin thấy Dưới 10%. Nó cần Màn Hình báo chớp Nháy, Còi báo Tít Tít, Động cơ Giảm tốc.
**Thiết Kế Sai**: File Pin gọi hàm `ManHinh_Blink()`, `Coi_Keu()`. Sự phụ thuộc chằng chịt, file Pin không thể mang qua Dự án khác xài.
**Observer Pattern Bằng Function Pointers/Callback**:
- File Pin Cấp một Bảng Đăng Ký (Subscribe Board).
- Còi, Động Cơ tự gọi hàm `Pin_DangKyCanhBao(Hàm_Báo_Động_Của_Tao)`.
- Khi Pin Thấp, File Pin duyệt Vòng Lặp Bảng Đăng Ký và Gọi Bắn Ra mọi hàm. Module Pin SẠCH SẼ 100%, Không Cần Include File Màn Hình!

---

## 4. Hardware Abstraction Layer (HAL) Bằng Strategy Pattern
Dự án IoT đang xài Cảm biến Nhiệt độ Hãng A. Đột nhiên Đứt Chuỗi Cung Ứng, phải đổi qua Cảm biến Hãng B.
Nếu code dính chặt với thanh ghi I2C của Hãng A, Bạn Phải Bỏ Toàn Bộ Ứng Dụng Làm Lại!
**Pattern Cứu Hỏa Mạng Lưới Nhúng**:
Thiết kế Tầng Ứng dụng (Luật tính nhiệt độ trung bình) Giao Tiếp với 1 Interface (C Struct Pointers hoặc C++ Virtual Class) gọi là HAL.

```c
// Định Nghĩa Lớp Đệm Giao Tiếp (Interface)
typedef struct {
    int16_t (*read_temp)(void); // Con trỏ hàm trả về Số đo
} ITempSensor_t;

// Cài Đặt Thực Tế Cho Sensor Hãng A
int16_t A_Sensor_Read() { return I2C1->DR; }
ITempSensor_t Sensor_A_Driver = { .read_temp = A_Sensor_Read };

// Hàm Lõi Ứng Dụng (Không bao giờ thay đổi)
void Tinh_Toan_Loi(ITempSensor_t* my_sensor) {
    int16_t val = my_sensor->read_temp(); // Gọi vô danh, đa hình
}
```
Lúc Boot lên, Bạn Truyền cái Địa Chỉ `Sensor_A_Driver` vô Hàm. Ngày mai đứt hàng, Mua Chip Hãng B. Viết 1 file mới tạo `Sensor_B_Driver`. Lõi Ứng Dụng (App Core) Không Cần Đụng Vào 1 Dòng Chữ Nào.

---

## 5. Command Pattern (Đóng Gói Nhiệm Vụ Gửi Vào Dĩ Vãng)
Giao tiếp G-Code của Mạng gửi xuống hàng nghìn điểm Tọa Độ Mũi Khoan CNC (Máy in). Bắn thẳng vào Motor? Vứt!
**Giải Pháp Nhúng Tối Thượng**: Đóng gói MỌI MỆNH LỆNH thành Khối Dữ Liệu `struct`. Ném xuống Ring Buffer Queue (SRAM). Hoặc ghi trực tiếp ra Thẻ SD (EEPROM/Flash Dĩ Vãng).
```c
typedef struct {
    uint8_t opcode; // Đi lên, xuống
    int16_t x;
    int16_t y;
} Command_t;
```
Task Xử Lý Động cơ, Rất Bình thản, Lôi Từng Cục Struct Command ra, Tính toán nội suy (Interpolation) rồi xoay Trục Motor với Timing chuẩn xác. Đây là Cốt Lõi Kiến Trúc Hoạt Động của Phần mềm 3D Printer Toàn Cầu (VD: Marlin Firmware).
