# Design Patterns (Pattern, MCU Base) - 10 Case Studies

## Case 1: Tách Lớp Ứng Dụng Khỏi Phần Cứng (HAL Pattern)
**Ngữ cảnh**: Firmware cho máy in. Driver điều khiển động cơ bước (Stepper Motor).
**Bad Practice**: Code logic in nhúng đầy các lệnh `GPIOA->ODR = 1;`. Khi đem code này chạy Unit Test trên PC hoặc port qua chip khác, nó lỗi ngay lập tức.
**Good Practice (C-style HAL)**:
```c
// Interface định nghĩa các hàm, giấu đi chi tiết chip
typedef struct {
    void (*step_forward)(void);
    void (*set_speed)(uint16_t rpm);
} IMotor_t;

// Application Layer (Module Máy in) nhận vào một Interface
void Printer_MovePaper(IMotor_t* motor, uint32_t steps) {
    for(uint32_t i=0; i<steps; i++) {
        motor->step_forward();
        delay_ms(10);
    }
}
```

## Case 2: Tránh Dùng Global Variable Bằng C++ Singleton (An Toàn)
**Ngữ cảnh**: Cần một module quản lý Hệ thống Lỗi (Fault Manager) ghi log vào EEPROM. Module này được dùng ở mọi nơi.
**Bad Practice**: Biến toàn cục `FaultManager g_fault_manager;`.
**Good Practice (Singleton - Khởi tạo tĩnh C++11)**:
```cpp
class FaultManager {
private:
    FaultManager() {} // Khóa Constructor
public:
    static FaultManager& getInstance() {
        // C++11 đảm bảo việc khởi tạo biến static này là Thread-safe
        static FaultManager instance;
        return instance;
    }
    void reportFault(uint8_t fault_code) { /* Ghi EEPROM */ }
};

// Sử dụng ở bất cứ đâu
FaultManager::getInstance().reportFault(ERR_MOTOR_JAM);
```

## Case 3: State Machine (Máy Trạng Thái) Dùng Hàm Chuyển Đổi
**Ngữ cảnh**: Màn hình HMI của máy in (Standby -> Print -> Pause -> Error).
**Practice (Function-based FSM)**:
```c
typedef enum { STATE_IDLE, STATE_PRINTING, STATE_ERROR } State_t;
State_t current_state = STATE_IDLE;

void handle_idle() { if(button_pressed) current_state = STATE_PRINTING; }
void handle_printing() { if(paper_jam) current_state = STATE_ERROR; }

void fsm_run() {
    switch(current_state) {
        case STATE_IDLE: handle_idle(); break;
        case STATE_PRINTING: handle_printing(); break;
        case STATE_ERROR: /* ... */ break;
    }
}
```

## Case 4: Observer Pattern Cho Nút Bấm Khẩn Cấp (E-Stop)
**Ngữ cảnh**: Khi Robot chạm chướng ngại vật, cần dừng Motor, tắt đèn Laser, và gửi cảnh báo về máy chủ.
**Vấn đề**: Cảm biến Tiệm cận không được phép gọi trực tiếp code của Motor, Laser, Wifi (Phá vỡ tính độc lập).
**Giải pháp**:
```cpp
class IObservation {
public:
    virtual void onObstacleDetected() = 0;
};

// Cảm biến quản lý danh sách các Observers
class ObstacleSensor {
    IObservation* observers[5]; // Mảng tĩnh để lưu danh sách đăng ký
    uint8_t count = 0;
public:
    void attach(IObservation* obs) { if (count < 5) observers[count++] = obs; }
    void trigger() {
        for(uint8_t i=0; i<count; i++) observers[i]->onObstacleDetected();
    }
};
```

## Case 5: Command Pattern Đóng Gói Job Cho Máy In
**Ngữ cảnh**: Giao tiếp qua mạng, PC gửi xuống hàng ngàn tọa độ (X, Y) cần in. MCU không thể xử lý ngay lập tức.
**Giải pháp**:
Mỗi tọa độ (G-Code) là một "Command" đẩy vào Ring Buffer.
```c
typedef struct {
    uint8_t command_type; // VD: CMD_MOVE, CMD_HEAT
    uint32_t param1;
    uint32_t param2;
} CommandPacket_t;

RingBuffer_Push(&cmd_queue, new_command); // Interrupt mạng
// Main Loop thong thả xử lý từng lệnh
if (RingBuffer_Pop(&cmd_queue, &cmd)) { Execute_Command(&cmd); }
```

## Case 6: Publish/Subscribe Trên FreeRTOS
**Ngữ cảnh**: IoT Gateway có nhiều Cảm biến Nhiệt độ, Độ ẩm. Giao diện Màn hình và Bộ giao tiếp Mạng đều cần số liệu này.
**Giải pháp Thực Tế (Message Broker)**:
Sử dụng 1 FreeRTOS Queue chung đóng vai trò là "Data Broker".
Các Task Cảm biến gửi (Publish) cấu trúc Dữ liệu kèm ID Cảm biến vào Queue.
Các Task Tiêu thụ (Network, Display) chờ (Subscribe) ở Queue và tự động lọc ra ID dữ liệu nó cần. Giải quyết dứt điểm ma trận gọi hàm chéo.

## Case 7: Active Object (Luồng Dữ Liệu An Toàn)
**Ngữ cảnh**: Tránh Deadlock và Race Condition khi chia sẻ biến giữa các RTOS Tasks.
**Giải pháp (Kiến trúc chuẩn chỉ)**:
Thay vì tạo Mutex bảo vệ biến dùng chung (Dễ gây Block Task). Mỗi Object chỉ có duy nhất 1 Luồng (Thread) quản lý nó.
Nếu Task A muốn sửa biến của Task B, Task A KHÔNG được sửa trực tiếp. Task A phải đẩy 1 Message (Chứa yêu cầu sửa đổi) vào Mailbox/Queue của Task B. Task B đọc Message và TỰ sửa biến của chính nó. Không cần Mutex!

## Case 8: Facade Pattern Dành Cho API Cấp Cao
**Ngữ cảnh**: Muốn kết nối máy in vào hệ thống Wifi mạng nội bộ, yêu cầu phải thiết lập IP, DHCP, Mật khẩu WPA2, khởi động socket. Thao tác tốn đến 20 dòng code LwIP rất phức tạp.
**Giải pháp**: Viết một class `WifiFacade` chỉ cung cấp đúng 1 hàm API duy nhất: `bool Connect(char* ssid, char* pass);`. Mọi độ phức tạp của LwIP bị giấu kỹ dưới nắp capo.

## Case 9: Adapter / Wrapper Pattern (Kết nối Code Cũ)
**Ngữ cảnh**: Firmware của bạn được viết theo kiểu C++ hiện đại. Sếp yêu cầu tích hợp một thư viện Giải nén Ảnh định dạng JPEG do bên thứ 3 viết (100% C-code, dùng callback hàm tĩnh).
**Giải pháp**: Tạo một class `JpegAdapter`. Class này giao tiếp bằng chuẩn C++ ra bên ngoài (Tầng Application của bạn), nhưng bên trong class đó, nó sẽ chịu trách nhiệm chuyển đổi cấu trúc C++, ép kiểu con trỏ thành C-struct để nạp cho thư viện gốc. Không phá vỡ hệ sinh thái C++ của bạn.

## Case 10: State Pattern Bằng Mảng Con Trỏ Hàm (C-Style Tốc Độ Cao)
**Ngữ cảnh**: Protocol Mạng phức tạp của IoT đòi hỏi chuyển trạng thái hàng ngàn lần mỗi giây, việc dùng lệnh `switch-case` khổng lồ (20 cases) quá chậm và xấu.
**Giải pháp (Transition Table)**:
```c
typedef void (*StateFunc)(void);
void state_idle() { /*...*/ }
void state_sync() { /*...*/ }

// Mảng 2 hàm. Truy cập O(1). Cực nhanh, cực nguy hiểm nếu tràn mảng.
const StateFunc StateMachine_Table[] = {
    state_idle,
    state_sync
};

void run_state(uint8_t state_id) {
    StateMachine_Table[state_id](); // Thực thi nhảy hàm siêu tốc
}
```