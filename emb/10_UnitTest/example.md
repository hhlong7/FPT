# Unit Test - 10 Case Studies (Thực tế Embedded / IoT / Robotics)

## Case 1: Tách Biệt Logic Ra Khỏi Phần Cứng Để Test
**Ngữ cảnh**: Firmware đo mức pin (Battery Level).
**Bad Practice**: 
```c
uint8_t get_battery_level() {
    uint16_t adc = read_adc_register(0x40012400); // Lệnh này gọi trực tiếp thanh ghi, sẽ báo lỗi "Segmentation Fault" nếu chạy trên máy tính (Unit Test).
    return (adc * 100) / 4095;
}
```
**Good Practice (Có thể test trên PC)**:
```c
// Hàm này 100% logic phần mềm, có thể Unit Test cực dễ dàng
uint8_t calculate_battery_level(uint16_t adc_raw) {
    if (adc_raw > 4095) return 100;
    return (adc_raw * 100) / 4095;
}
```

## Case 2: Test Biên (Boundary Testing - BVA)
**Ngữ cảnh**: Hàm `calculate_battery_level` ở Case 1. Nếu hệ thống ADC 12-bit bị nhiễu và trả về số 4096 hoặc số âm, điều gì sẽ xảy ra?
**Practice**: Viết Test Case cho các giá trị Biên.
```cpp
TEST(BatteryTest, AdcRawExceedsMaximum_Returns100) {
    EXPECT_EQ(calculate_battery_level(5000), 100); 
}
TEST(BatteryTest, AdcRawMinimum_ReturnsZero) {
    EXPECT_EQ(calculate_battery_level(0), 0);
}
```

## Case 3: Sử Dụng C++ Mock Interface Để Giả Lập SPI
**Ngữ cảnh**: Lớp `FlashMemory` yêu cầu giao tiếp SPI. Trong Unit Test, bạn không có chip Flash thật.
**Kịch bản Test**: Khi gọi hàm `FlashMemory::write()`, ta mong đợi (Expect) hàm `SPI::send()` được gọi đúng 1 lần với dữ liệu chính xác.
```cpp
TEST(FlashTest, WriteSendsCorrectData) {
    MockSPI mock_spi; // Đối tượng giả
    FlashMemory flash(mock_spi); // Dependency Injection
    
    // Yêu cầu (Expectation): Hàm send() của SPI phải được gọi 1 lần.
    EXPECT_CALL(mock_spi, send(testing::NotNull(), 256)).Times(1);
    
    // Act
    flash.write(data_buffer, 256);
}
```

## Case 4: Test State Machine Của Robot
**Ngữ cảnh**: Quản lý trạng thái Robot (Idle -> Moving -> Error).
**Vấn đề**: Việc nạp code lên Robot vật lý, gạt công tắc để nhảy vào từng trạng thái cực kỳ tốn thời gian.
**Giải pháp Unit Test**: Viết Test tạo một luồng sự kiện (Events Sequence).
```cpp
TEST(RobotStateTest, ErrorEventWhileMoving_TransitionsToErrorState) {
    RobotFSM robot;
    robot.dispatch(Event::START);
    EXPECT_EQ(robot.getState(), State::MOVING);
    
    robot.dispatch(Event::OBSTACLE_DETECTED);
    EXPECT_EQ(robot.getState(), State::ERROR); // Test logic cực nhanh trong 1ms
}
```

## Case 5: Giả Lập Thời Gian (Time Mocking)
**Ngữ cảnh**: Một Task yêu cầu timeout 5 giây (5000ms) nếu mất kết nối mạng.
**Bad Practice trong Test**: Chạy `sleep(5000)` trong Unit Test để đợi. Làm Unit Test chạy chậm rì.
**Good Practice**: Không dùng `millis()` trực tiếp của hệ thống. Dùng Dependency Injection cho Đồng hồ (Clock).
```cpp
TEST(NetworkTest, TimeoutOccursAfter5Seconds) {
    MockClock fake_clock;
    NetworkTask net(fake_clock);
    
    fake_clock.setTime(0);
    net.update(); // Trạng thái: Đang đợi
    
    fake_clock.setTime(5001); // Quay cót đồng hồ đi qua tương lai ngay lập tức!
    net.update(); // Trạng thái: Timeout!
    
    EXPECT_TRUE(net.hasTimeoutOccurred());
}
```

## Case 6: Kiểm Tra Mã Lỗi Lặp Lại (Regression Testing)
**Ngữ cảnh**: Tuần trước, có một bug xảy ra do Parsing (Phân tích) nhầm gói tin UART có độ dài lẻ. Đã sửa xong.
**Practice**: Viết ngay một Test Case nạp đúng chuỗi gói tin lỗi đó vào hàm Parser để đảm bảo bug này không bao giờ tái xuất hiện trong tương lai. Lợi ích lớn nhất của Unit Test là "Bảo vệ các code đang chạy tốt không bị lập trình viên vô ý phá hỏng sau này".

## Case 7: Test Mạng / Giao Thức (Protocol Testing)
**Ngữ cảnh**: Xây dựng chuẩn giao tiếp MQTT cho máy in (IoT Print).
**Practice**: Bạn có thể viết hàng trăm Test Case đẩy các byte chuỗi nhị phân (Binary Hex array) rác, thiếu header, thiếu footer, checksum sai vào hàm Parser. Việc này (Fuzzing / Negative Testing) đảm bảo chương trình không bị Crash khi gặp kết nối mạng nhiễu.

## Case 8: Code Coverage Dành Cho Safety (ISO 26262)
**Ngữ cảnh**: Chuẩn ISO 26262 ASIL-D yêu cầu Modified Condition/Decision Coverage (MC/DC) đạt 100%.
**Ví dụ**:
`if (A && B)` -> Test Case phải bao phủ đủ 3 trường hợp:
1. A=True, B=True (Kết quả True)
2. A=False, B=True (Kết quả False, do A)
3. A=True, B=False (Kết quả False, do B)
Các công cụ Coverage (như gcov/lcov) sẽ highlight đoạn code đỏ nếu bạn viết Test chưa đủ.

## Case 9: Giả Lập Mảng Của DMA / ADC
**Ngữ cảnh**: Thuật toán phân tích giọng nói (Audio DSP) lấy dữ liệu từ DMA của vi điều khiển.
**Practice**: Trong Unit Test PC, tạo một mảng mảng tĩnh (Static array) chứa sóng âm hình Sin chuẩn (được tạo từ Python hoặc Excel). Truyền mảng này vào Hàm lọc DSP để kiểm tra xem thuật toán xuất ra kết quả phân tích có khớp với kỳ vọng toán học hay không. Không cần nối Micro vật lý để Test!

## Case 10: Tách Biệt Hardware Definitions (`#ifdef`)
**Ngữ cảnh**: Mã nguồn có các biến kiểu `#include <stm32f4xx.h>`. Nếu biên dịch trên PC sẽ bị báo lỗi "File not found".
**Practice**: 
```c
#ifdef RUNNING_ON_HARDWARE
  #include <stm32f4xx.h>
  #define HW_READ_PIN() (GPIOA->IDR & 0x01)
#else
  // Chạy trên môi trường PC Unit Test
  extern uint8_t mock_pin_state; 
  #define HW_READ_PIN() mock_pin_state
#endif
```
Đây là cách cơ bản nhất để chuyển đổi qua lại giữa Môi trường nhúng thật và Môi trường Test giả lập.
