# SỔ TAY BÀI TẬP VÀ ĐÁNH GIÁ CHẤT LƯỢNG (FAST-TRACK 10 NGÀY)
> **Định hướng:** Thực hành lập trình giải quyết các lỗi phần mềm kinh điển, thiết kế driver ngoại vi, quản lý bộ nhớ và áp dụng C++ Hướng đối tượng nhúng an toàn.
> **Điều chỉnh:** Cấu trúc lại theo lộ trình học rút gọn **10 ngày**. Các bài tập nâng cao (Unit Test, Move Semantics, PID Fixed-Point, MISRA Auditing) được chuyển xuống phần **Bài tập tham khảo nâng cao**.

---

## 🗓️ KHỐI BÀI TẬP CHÍNH KHÓA (10 NGÀY)

### GIAI ĐOẠN 1: NỀN TẢNG C & QUẢN TRỊ BỘ NHỚ VẬT LÝ

#### 🔴 NGÀY 1: KIỂU DỮ LIỆU & THAO TÁC BIT (TOPIC 1)

##### 📝 Bài Tập 1.1: Kích Thước Biến và Mạng IoT (Endianness & Fixed-width)
*   **Ngữ cảnh:** Bạn đang lập trình một cảm biến nhiệt độ (IoT Node) sử dụng chip 8-bit AVR. Thiết bị này đo nhiệt độ và gửi dữ liệu lên một Máy chủ Gateway chạy chip 32-bit ARM.
*   **Vấn đề:** Kỹ sư cũ định nghĩa cấu trúc gói tin giao tiếp như sau:
    ```c
    struct SensorData {
        int temperature; 
        long timestamp;
    };
    ```
    Khi cảm biến đo được nhiệt độ `25°C`, nó gửi gói tin lên Gateway. Tuy nhiên, Gateway giải mã ra nhiệt độ thu được là `1638425`°C!
*   **Yêu cầu:**
    1.  Phân tích nguyên nhân lỗi dựa trên kích thước kiểu `int` và `long` khác nhau giữa 2 hệ thống (8-bit vs 32-bit) và hiện tượng Endianness.
    2.  Sửa lại struct `SensorData` bằng cách bắt buộc sử dụng thư viện kiểu dữ liệu chuẩn `<stdint.h>`.
    3.  Viết một hàm C ngắn gọn để đóng gói `temperature` (số nguyên 16-bit) thành mảng 2 byte truyền đi sao cho không phụ thuộc vào Endianness (Ví dụ: ép truyền theo thứ tự Big-Endian).

##### 📝 Bài Tập 1.2: Điều Khiển Hệ Thống Nhiệt Máy In Bằng Bitwise
*   **Ngữ cảnh:** Máy in 3D có một thanh ghi Điều khiển Trạng thái Phần cứng (Hardware Status Register) 8-bit nằm tại địa chỉ `0x40021000`.
    *   Ý nghĩa các bit (từ 0 đến 7):
        *   Bit 0: Bật/Tắt quạt tản nhiệt (Fan)
        *   Bit 1: Bật/Tắt đầu đùn nhựa (Heater)
        *   Bit 2: Bật/Tắt bàn nhiệt (Bed)
        *   Bit 4-7: Lưu mã lỗi (Error Code từ 0 đến 15)
*   **Yêu cầu:**
    Không sử dụng các phép tính cộng/trừ/nhân/chia. Chỉ dùng toán tử Bitwise (`|`, `&`, `~`, `^`, `<<`, `>>`):
    1.  Viết lệnh C để **Chỉ Bật** đầu đùn nhựa (Heater), giữ nguyên trạng thái các thiết bị khác.
    2.  Viết lệnh C để **Chỉ Tắt** quạt tản nhiệt, giữ nguyên các thiết bị khác.
    3.  Đầu đùn vừa phát hiện kẹt nhựa, hãy viết lệnh để **Ghi Mã Lỗi số 5** (Nhị phân: `0101`) vào 4 bit cao nhất của thanh ghi mà không làm thay đổi trạng thái các quạt/heater đang chạy.
    4.  Viết lệnh đọc thanh ghi này ra và tách riêng (extract) mã lỗi thành một biến `uint8_t current_error`.

---

#### 🔴 NGÀY 2: VOLATILE, CONST & MEMORY MAP (TOPIC 1 & 2)

##### 📝 Bài Tập 2.1: Phòng Tránh Heisenbug Bằng Từ Khóa `volatile`
*   **Ngữ cảnh:** Trong firmware máy in, một ngắt Timer (ISR) tự động đếm giây và cập nhật biến `system_uptime`. Main loop sẽ chờ khi nào đủ 5 giây thì bắt đầu nung nóng đầu in.
    ```c
    uint32_t system_uptime = 0;

    void Timer_ISR() { // Chạy tự động mỗi 1 giây
        system_uptime++;
    }

    void start_heating_process() {
        while (system_uptime < 5) {
            // Đợi...
        }
        Heater_TurnOn();
    }
    ```
*   **Vấn đề:** Khi biên dịch ở chế độ Debug (`-O0`), máy in chạy bình thường. Khi biên dịch để xuất xưởng chế độ Release (`-O2`), máy in bị đơ vĩnh viễn ở vòng lặp `while` và lò sưởi không bao giờ bật.
*   **Yêu cầu:**
    1.  Giải thích chi tiết tại sao Trình biên dịch (Compiler Optimizer) lại tạo ra vòng lặp vô hạn ở chế độ tối ưu hóa `-O2`.
    2.  Sửa mã nguồn trên chỉ bằng cách thêm **ĐÚNG MỘT TỪ KHÓA**. Giải thích ý nghĩa vật lý của từ khóa đó đối với hoạt động của thanh ghi CPU.

##### 📝 Bài Tập 2.2: Tiết Kiệm RAM Màn Hình Bằng Khai Báo `const`
*   **Ngữ cảnh:** Màn hình máy in 3D cần hiển thị các chuỗi Cảnh báo.
    ```c
    char msg_warning1[] = "Canh bao: Nhiet do cao!";
    char msg_warning2[] = "Canh bao: Ket nhua o dau in!";
    char msg_warning3[] = "Canh bao: Mat ket noi Wifi!";
    ```
*   **Yêu cầu:**
    1.  3 chuỗi trên được lưu trữ ở phân vùng bộ nhớ nào trên MCU sau khi khởi động? (Gợi ý: Section `.data`, `.bss`, `.text` hay `.rodata`?).
    2.  Việc sử dụng mảng ký tự không có từ khóa `const` gây lãng phí loại bộ nhớ nào? Vì sao?
    3.  Sửa lại khai báo trên để tiết kiệm tối đa dung lượng SRAM của chip. Giải thích luồng di chuyển dữ liệu lúc khởi động (Boot).

##### 📝 Bài Tập 2.3: Phân Tích Bản Đồ Định Vị Bộ Nhớ (Memory Map)
*   **Ngữ cảnh:** Cho đoạn code C của bộ điều khiển Robot dưới đây:
    ```c
    #include <stdint.h>

    const uint32_t PI_x1000 = 3141;         // (1)
    uint8_t robot_status = 1;               // (2)
    uint8_t error_log[1024];                // (3)

    void robot_move() {
        uint32_t target_position = 500;     // (4)
        static uint32_t move_count = 0;     // (5)
    }
    ```
*   **Yêu cầu:**
    1.  Hãy phân loại 5 thành phần dữ liệu trên vào các phân vùng bộ nhớ tương ứng trên Vi điều khiển: `.rodata`, `.data`, `.bss`, `Stack`.
    2.  Trình bày chi tiết quá trình Khởi động (C Startup/Boot code) làm thế nào để mảng `error_log` có toàn bộ giá trị là `0` trước khi hàm `main()` được gọi.

---

#### 🔴 NGÀY 3: CON TRỎ & SẮP XẾP BỘ NHỚ (TOPIC 3)

##### 📝 Bài Tập 3.1: Ép Kiểu Trực Tiếp Payload Mạng (Strict Aliasing & Alignment Fault)
*   **Ngữ cảnh:** Cổng IoT Gateway nhận được mảng byte dữ liệu từ cảm biến qua UART: `uint8_t payload[8]`. Kỹ sư biết rằng 4 byte đầu là số thực `float` nhiệt độ, 4 byte sau là số nguyên `uint32_t` áp suất. Kỹ sư mới viết đoạn phân tích (Parser) như sau:
    ```c
    void parse_payload(uint8_t* payload) {
        float temp = *(float*)&payload[0];
        uint32_t pres = *(uint32_t*)&payload[4];
    }
    ```
*   **Vấn đề:** Đoạn code vi phạm nghiêm trọng quy tắc **Strict Aliasing** và gây ra lỗi phần cứng **Hard Fault (Alignment Fault)** trên một số chip ARM Cortex-M0/M3 nếu mảng `payload` nằm ở địa chỉ lẻ trong RAM.
*   **Yêu cầu:**
    1.  Lỗi căn lề bộ nhớ (Memory Alignment Fault) là gì? Tại sao việc ép kiểu con trỏ trực tiếp này lại gây sập vi điều khiển?
    2.  Viết lại hàm `parse_payload` một cách an toàn bằng hàm copy bộ nhớ tiêu chuẩn (giúp Compiler tự động tối ưu hóa câu lệnh máy chuẩn hóa Alignment).

##### 📝 Bài Tập 3.2: Con Trỏ Lủng Lẳng (Dangling Pointer) - Use-After-Free Nội Bộ
*   **Ngữ cảnh:** Đoạn mã nguồn giao diện máy in:
    ```c
    char* get_printer_status(int code) {
        char status_str[50];
        if (code == 0) strcpy(status_str, "Ready");
        else strcpy(status_str, "Error");
        
        return status_str; 
    }

    void update_ui() {
        char* current_status = get_printer_status(0);
        lcd_print(current_status); // Màn hình in ra toàn ký tự rác!
    }
    ```
*   **Yêu cầu:**
    1.  Phân tích chi tiết vòng đời (Lifetime) của mảng `status_str` nằm trên Ngăn xếp (Stack). Vì sao lúc in ra LCD dữ liệu lại bị biến thành rác?
    2.  Viết lại hàm `get_printer_status` theo 2 cách thiết kế nhúng an toàn (Không dùng bộ nhớ Heap):
        *   *Cách 1:* Sử dụng từ khóa `static`.
        *   *Cách 2:* Truyền con trỏ đệm nhận dữ liệu từ Caller (Hàm gọi).

---

#### 🔴 NGÀY 4: HÀM, STACK FRAME & CALLBACKS (TOPIC 4)

##### 📝 Bài Tập 4.1: Rủi Ro Hàm Không Tái Nhập (Non-Reentrant) Trong RTOS
*   **Ngữ cảnh:** Cần một hàm để tạo chuỗi thời gian (Timestamp) đính kèm vào file Log:
    ```c
    char* format_timestamp(uint32_t seconds) {
        static char time_buf[20]; // Tránh dangling pointer
        sprintf(time_buf, "Time: %lu", seconds);
        return time_buf;
    }
    ```
    Trên hệ điều hành thời gian thực (FreeRTOS), Task A (Độ ưu tiên thấp) gọi hàm này. Khi hàm `sprintf` chạy được một nửa, Ngắt Timer (Độ ưu tiên cực cao) xen ngang và gọi lại chính hàm `format_timestamp()` để ghi log lỗi.
*   **Yêu cầu:**
    1.  Thế nào là một Hàm Tái Nhập (Reentrant Function)? 
    2.  Phân tích hiện tượng "Ghi đè rác" xảy ra bên trong biến tĩnh `time_buf` do sự xen ngang của Task ưu tiên cao (Race Condition).
    3.  Hãy viết lại hàm `format_timestamp` để nó trở thành Hàm Tái Nhập 100% an toàn cho Đa nhiệm (Thread-safe).

##### 📝 Bài Tập 4.2: Con Trỏ Hàm (Callback) Thiết Kế Scheduler Siêu Nhẹ
*   **Ngữ cảnh:** Bạn cần thiết kế một Trình Lập Lịch (Task Scheduler) rất cơ bản (Không dùng RTOS) trên C thuần. Cứ mỗi 10ms thì chạy Task A, 50ms thì chạy Task B.
    ```c
    typedef struct {
        uint32_t interval_ms;
        uint32_t last_run_ms;
        // THIẾU: Con trỏ hàm trỏ tới Task cần thực thi
    } Task_t;
    ```
*   **Yêu cầu:**
    1.  Khai báo bổ sung thêm 1 trường dữ liệu vào `Task_t` là một Con Trỏ Hàm (Function Pointer) không trả về giá trị, không có tham số đầu vào. Tên trường là `execute`.
    2.  Viết code khởi tạo mảng 2 `Task_t` chứa 2 task tương ứng với chu kỳ 10ms và 50ms.
    3.  Viết một hàm Main Loop (`while(1)`) quét liên tục qua mảng Task. Nếu thời gian hiện tại (`current_ms`) đã qua đủ chu kỳ `interval_ms` thì thực hiện cuộc gọi Callback thông qua Con trỏ hàm.

---

#### 🔴 NGÀY 5: BARE-METAL I/O & DEBUG (TOPIC 5)

##### 📝 Bài Tập 5.1: Kỹ Thuật Viết In Log UART Không Gây Nghẽn (Non-Blocking Logging)
*   **Ngữ cảnh:** Hệ thống điều khiển PID động cơ cần in log trạng thái tốc độ ra UART mỗi 1ms. 
    Kỹ sư cũ viết hàm in log trực tiếp bằng UART Polling (Chờ cờ truyền trống hoàn toàn):
    ```c
    void UART_SendString_Blocking(const char* str) {
        while (*str) {
            while (!(UART->STATUS & UART_TX_EMPTY_MASK)); // Chờ nghẽn thanh ghi truyền
            UART->DATA = *str++;
        }
    }
    ```
*   **Vấn đề:** Việc in một chuỗi 30 ký tự ở tốc độ baudrate `9600` tốn mất khoảng **30ms**. Điều này làm ngưng trệ hoàn toàn vòng điều khiển động cơ vốn cần độ trễ dưới 1ms!
*   **Yêu cầu:**
    1.  Giải thích tại sao việc in log bằng cơ chế Polling (Blocking) hủy hoại tính chất thời gian thực (Real-time discipline) của vi điều khiển.
    2.  Đề xuất một giải pháp thiết kế sử dụng **Interrupt (Ngắt)** hoặc **DMA** kết hợp với **Ring Buffer** để chuyển đổi hàm in log trên thành hàm Non-blocking (Truyền tức thời vào đệm rồi thoát ngay). Vẽ sơ đồ luồng dữ liệu.

##### 📝 Bài Tập 5.2: Thiết Kế Macro `ASSERT` An Toàn Failsafe Cho Sản Phầm
*   **Ngữ cảnh:** Chuẩn an toàn công nghiệp yêu cầu khi phát hiện trạng thái lỗi nghiêm trọng không phục hồi được (như nhiệt độ đầu in vượt ngưỡng cho phép 300°C), thiết bị phải lập tức về trạng thái an toàn trước khi dừng hẳn.
*   **Yêu cầu:**
    Hãy viết một Macro `EMBEDDED_ASSERT(expression)` trong C/C++ đáp ứng:
    1.  Nếu biểu thức `expression` đánh giá là `false`, macro sẽ:
        *   Tắt ngay lập tức ngắt toàn cục (Disable global interrupts) để đóng băng hệ thống.
        *   Bật trạng thái ngắt điện lò sưởi khẩn cấp (`HEATER_CUT_POWER()`).
        *   In mã lỗi và dòng code lỗi (Sử dụng các macro tiền xử lý `__FILE__` và `__LINE__`).
        *   Đi vào vòng lặp vô hạn kích hoạt Watchdog Reset hoặc tự treo chip để bảo vệ phần cứng.

---

#### 🔴 NGÀY 6: WORKSHOP TÍCH HỢP GIAI ĐOẠN 1
> 📋 **Nhiệm vụ:** Hoàn thành toàn bộ dự án thực hành đóng gói dữ liệu Telemetry và phân tích bản đồ định vị RAM/Flash ROM chi tiết tại tệp học liệu [workshop1.md](file:///d:/Workspaces/C_CPP/C_CPP_BASIC_EMB/workshop1.md).

---

### GIAI ĐOẠN 2: CHUYỂN DỊCH C++ VÀ HƯỚNG ĐỐI TƯỢNG TỐI ƯU

#### 🟢 NGÀY 7: C++ REFERENCE, NAMESPACE & CONSTEXPR (TOPIC 8)

##### 📝 Bài Tập 7.1: Chuyển Đổi Con Trỏ C Sang Tham Chiếu C++ An Toàn
*   **Ngữ cảnh:** Đoạn mã cấu hình Wifi bằng C thuần:
    ```c
    typedef struct {
        char ssid[32];
        uint8_t channel;
    } WifiConfig;

    void load_wifi_defaults(WifiConfig* p_cfg) {
        if (p_cfg == NULL) return; // Bắt buộc phải check NULL
        strcpy(p_cfg->ssid, "DefaultNet");
        p_cfg->channel = 6;
    }
    ```
*   **Yêu cầu:**
    1.  Chuyển đổi hàm trên sang C++ sử dụng **Tham chiếu (Reference `&`)**.
    2.  Nêu ít nhất 2 ưu điểm vượt trội về độ an toàn phần mềm và cú pháp của Tham chiếu C++ so với Con trỏ truyền thống trong bối cảnh lập trình nhúng.

##### 📝 Bài Tập 7.2: Kiểm Tra Cấu Hóa Lúc Build Code Bằng `constexpr` & `static_assert`
*   **Ngữ cảnh:** Trong hệ thống truyền tin IoT, kích thước gói tin gửi đi không được phép vượt quá giới hạn bộ nhớ đệm của phần cứng truyền thông (ví dụ: `256` bytes).
*   **Yêu cầu:**
    1.  Sử dụng `constexpr` để khai báo cấu trúc kích thước gói tin gồm Header (8 bytes), Payload (biến động), CRC (2 bytes).
    2.  Viết câu lệnh `static_assert` kiểm tra tính hợp lệ của tổng kích thước cấu hình này. Giải thích tại sao `static_assert` lại ưu việt hơn kiểm tra lỗi bằng lệnh `if` thông thường lúc Runtime.

---

#### 🟢 NGÀY 8: OOP BASICS & RAII TRÊN MCU (TOPIC 6)

##### 📝 Bài Tập 8.1: Hai Giai Đoạn Khởi Tạo & Driver Không Cho Phép Sao Chép
*   **Ngữ cảnh:** Bạn cần thiết kế một lớp driver C++ điều khiển quạt tản nhiệt của đầu đùn máy in (`HeaterFan`).
*   **Yêu cầu:**
    Viết lớp `HeaterFan` bằng C++ tuân thủ nghiêm ngặt các nguyên tắc sau:
    1.  **Cấm sao chép driver (Non-copyable):** Ngăn chặn lập trình viên sao chép đối tượng driver phần cứng vật lý bằng cách sử dụng `= delete` cho Constructor sao chép và Toán tử gán.
    2.  **Two-Stage Initialization:** Không cấu hình thanh ghi GPIO vật lý bên trong Constructor (Hàm dựng) vì lúc đó xung nhịp hệ thống chưa khởi động xong. Hãy tách cấu hình GPIO ra một hàm thành viên `bool init(uint8_t pin_num)`.

##### 📝 Bài Tập 8.2: Ứng Dụng RAII Viết Lớp Khóa Scope Lock Cho ISR
*   **Ngữ cảnh:** Để đọc/ghi một biến toàn cục dùng chung giữa luồng chính và ngắt một cách an toàn (tránh tranh chấp dữ liệu), ta phải khóa ngắt toàn cục trước khi truy cập và mở lại ngay sau khi hoàn tất.
*   **Yêu cầu:**
    Hãy triển khai một lớp C++ `InterruptGuard` theo nguyên lý **RAII**:
    1.  Hàm dựng (Constructor) của lớp sẽ tự động tắt ngắt toàn cục (`__disable_irq()`).
    2.  Hàm hủy (Destructor) của lớp sẽ tự động bật lại ngắt toàn cục (`__enable_irq()`).
    3.  Viết ví dụ sử dụng lớp này bảo vệ một Critical Section trong mã nguồn để chứng minh tính tự động giải phóng khóa khi thoát khỏi tầm vực (Scope).

---

#### 🟢 NGÀY 9: ĐA HÌNH & TỐI ƯU HÓA TRONG MCU (TOPIC 7)

##### 📝 Bài Tập 9.1: Phân Tích Hao Tổn Bộ Nhớ Hàm Ảo (VTable & VPtr)
*   **Ngữ cảnh:** Bạn định nghĩa lớp cơ sở trừu tượng cho cảm biến nhiệt độ:
    ```cpp
    class TempSensor {
    public:
        virtual float readTemp() = 0;
    };

    class LM35 : public TempSensor {
        uint8_t adc_pin;
    public:
        float readTemp() override { return 25.0f; }
    };
    ```
*   **Yêu cầu:**
    1.  Giải thích chi tiết cấu trúc bộ nhớ của lớp `LM35` khi được khởi tạo. Bảng hàm ảo (**VTable**) được lưu ở đâu? Con trỏ hàm ảo (**VPtr**) chiếm bao nhiêu byte RAM trên chip 32-bit?
    2.  Tại sao trong hệ nhúng an toàn bắt buộc phải biên dịch với cờ `-fno-rtti` (Tắt thông tin kiểu Runtime) và cấm tiệt cơ chế `dynamic_cast`?

##### 📝 Bài Tập 9.2: Đa Hình Tĩnh Zero-Overhead Bằng Mẫu CRTP
*   **Ngữ cảnh:** Bạn muốn duy trì tính kế thừa kiến trúc (HAL) nhưng cần loại bỏ hoàn toàn chi phí lưu trữ VTable và thời gian nhảy địa chỉ gián tiếp của VPtr để tối ưu tốc độ CPU.
*   **Yêu cầu:**
    1.  Hãy viết lại thiết kế đa hình Sensor ở Bài tập 9.1 sử dụng mẫu thiết kế **CRTP (Curiously Recurring Template Pattern)** để thực hiện đa hình tĩnh tại thời kỳ biên dịch (Compile-time polymorphism).
    2.  Chứng minh bằng code cách gọi các hàm của lớp dẫn xuất thông qua lớp cơ sở CRTP mà không phát sinh thêm bất kỳ chi phí bộ nhớ RAM hay VTable nào.

---

#### 🟢 NGÀY 10: CẤU TRÚC DỮ LIỆU AN TOÀN CẤM HEAP (TOPIC 2 & 9)

##### 📝 Bài Tập 10.1: Phân Mảnh RAM (Memory Fragmentation) và Giải Pháp Static Pool
*   **Ngữ cảnh:** Trạm Gateway IoT liên tục nhận gói tin cấu hình qua Wifi. Vì kích thước gói tin biến động, kỹ sư cũ viết code cấp phát động như sau:
    ```c
    void parse_wifi_packet(uint16_t packet_size) {
        char* json_payload = (char*)malloc(packet_size);
        if (json_payload != NULL) {
            wifi_read(json_payload, packet_size);
            process_json(json_payload);
            free(json_payload);
        }
    }
    ```
*   **Vấn đề:** Hệ thống chạy trơn tru trong 1 ngày. Đến ngày thứ 2, `malloc` luôn trả về `NULL` khiến thiết bị bị cô lập, mặc dù tổng dung lượng RAM báo trống trên chip vẫn còn hơn 10KB.
*   **Yêu cầu:**
    1.  Hãy giải thích hiện tượng phân mảnh RAM (Memory Fragmentation). Vẽ hình minh họa (bằng Text) giải thích vì sao RAM trống 10KB mà `malloc(200)` vẫn có thể thất bại.
    2.  Viết lại logic quản lý bộ nhớ trên sử dụng một **Memory Pool** cấp phát tĩnh (Static Array Allocation) với kích thước block cố định để đảm bảo thời gian thực thi cố định O(1) và loại bỏ phân mảnh RAM vĩnh viễn.

##### 📝 Bài Tập 10.2: Triển Khai Giải Thuật Lọc Số Khử Nhiễu EMA (Fixed-Point)
*   **Ngữ cảnh:** Cần đọc ADC thô 12-bit (`0 - 4095`) từ cảm biến nhiệt độ. Tín hiệu này bị nhiễu răng cưa cao tần.
*   **Yêu cầu:**
    1.  Viết hàm lọc số **Exponential Moving Average (EMA)** trong C bằng số nguyên (Fixed-Point) để chạy nhanh nhất trên MCU không có bộ xử lý số thực (FPU).
        Công thức lọc: $Y[n] = \alpha \cdot X[n] + (1 - \alpha) \cdot Y[n-1]$
        Với hệ số làm mịn $\alpha = 0.25$ (Gợi ý: biến đổi hệ số $\alpha$ về phép chia cho lũy thừa của 2 để dùng phép dịch bit `>>` thay thế cho phép nhân/chia số thực).

---

## 📚 KHỐI BÀI TẬP THAM KHẢO NÂNG CAO (TỰ HỌC THÊM)
*Phần này dành cho học viên muốn thử thách bản thân với các kỹ thuật nâng cao thuộc Chương 10 - 13.*

### 🛠️ Bài Tập A: Thiết Kế Điểm Cắt (Seams) Và Mock Driver I2C EEPROM (Topic 10 - Unit Testing)
*   **Ngữ cảnh:** Bạn viết logic lưu trữ số đếm máy in (Odometer) vào bộ nhớ I2C EEPROM vật lý. Bạn muốn Unit Test logic tăng odometer và ghi xuống EEPROM chạy hoàn toàn độc lập trên máy tính PC (Host) mà không cần nạp code vào board mạch thật.
*   **Yêu cầu:**
    1.  Thiết kế một Interface trừu tượng `IEEPROM` định nghĩa 2 API: `write(uint16_t addr, uint8_t data)` và `read(uint16_t addr)`.
    2.  Viết lớp logic `OdometerController` sử dụng Interface `IEEPROM` thông qua cơ chế tiêm sự phụ thuộc (Dependency Injection).
    3.  Viết một lớp giả lập `MockEEPROM` kế thừa từ `IEEPROM` để chạy giả lập lưu dữ liệu vào một mảng trong RAM trên PC. Viết kịch bản kiểm thử đơn vị (Unit Test) xác nhận logic odometer ghi đúng địa chỉ EEPROM quy định.

### 🛠️ Bài Tập B: Vận Chuyển OtaChunk 4KB Bằng Move Semantics (Zero-Copy) (Topic 11 - Advanced C++)
*   **Ngữ cảnh:** Cổng IoT Gateway nhận gói tin cập nhật Firmware (OTA Chunk) kích thước lớn 4KB. Gói tin này cần đi qua 3 lớp phần mềm (Lớp nhận Driver -> Lớp giải mã Decryption -> Lớp ghi Flash). Việc copy mảng 4KB này qua các hàm làm nghẽn CPU và nguy cơ gây tràn Stack.
*   **Yêu cầu:**
    1.  Hãy giải thích nguyên lý hoạt động của Move Semantics trong C++. Tại sao Move Semantics có thể chuyển quyền sở hữu bộ đệm dữ liệu mà không cần copy bộ nhớ vật lý?
    2.  Thiết kế lớp `OtaChunk` chứa con trỏ trỏ tới vùng đệm dữ liệu. Triển khai **Move Constructor** và **Move Assignment Operator** cho lớp này. Viết code minh họa cách chuyển đổi gói tin qua 3 lớp xử lý bằng `std::move`.

### 🛠️ Bài Tập C: Tối Ưu Giải Thuật PID Bằng Định Dạng Q16.16 (Topic 12 - Optimization)
*   **Ngữ cảnh:** Giải thuật điều khiển nhiệt độ đầu đùn máy in sử dụng bộ điều khiển PID:
    $$Output = K_p \cdot e(t) + K_i \cdot \int e(t) dt + K_d \cdot \frac{de(t)}{dt}$$
*   **Yêu cầu:**
    1.  Hầu hết các chip vi điều khiển giá rẻ không có FPU (Floating Point Unit), phép tính số thực `float` sẽ chạy chậm hơn từ 10 đến 50 lần so với phép tính số nguyên. Hãy chuyển đổi code tính toán PID số thực sang sử dụng định dạng số nguyên **Fixed-Point Q16.16** (16 bit nguyên, 16 bit phân số).
    2.  Triển khai các phép nhân và phép chia cơ bản Q16.16 bằng toán tử dịch bit và ép kiểu `int64_t` tránh tràn số để hoàn tất thuật toán PID tối ưu tốc độ.

### 🛠️ Bài Tập D: Code Auditing - Rà Soát Lỗi Cú Phép Theo Chuẩn MISRA (Topic 13 / General Safety)
*   **Ngữ cảnh:** Đoạn mã nguồn điều khiển động cơ bước dưới đây được viết bởi một cộng tác viên. Hãy chỉ ra các lỗi cú pháp vi phạm chuẩn an toàn **MISRA C/C++** và sửa đổi lại cho đạt chuẩn xuất xưởng (ISO 26262):
    ```cpp
    int step_motor_control(unsigned char* p_steps) {
        int i;
        if (p_steps == 0) return -1;
        
        for (i = 0; i < 10; i++) {
            if ((*p_steps++) > 100) {
                // Do something...
            }
        }
        return 1;
    }
    ```
*   **Yêu cầu:**
    1.  Chỉ ra **3 điểm vi phạm nghiêm trọng** chuẩn an toàn MISRA trong đoạn mã trên.
    2.  Viết lại mã nguồn hoàn toàn sạch lỗi MISRA.
