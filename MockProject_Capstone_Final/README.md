# MockProject: Multi-Sensor Edge Controller for Print/IoT/Robot

## Mục tiêu dự án
Xây dựng firmware hoàn chỉnh cho **Edge Controller** tích hợp đầy đủ kiến thức từ 13 chương:
- Thu thập data từ 4 sensors (temperature, humidity, pressure, limit switch)
- Điều khiển 2 motors (X, Y axes)
- Giao tiếp UART với host PC
- Lưu trữ cấu hình vào NVM (EEPROM simulation)
- Watchdog supervision
- State machine với fault handling

## Cấu trúc thư mục

```
MockProject_Capstone_Final/
├── README.md                    # Mô tả project
├── Makefile                     # Build system
├── linker_script.ld             # Linker script cho ARM Cortex-M4
│
├── include/                     # Public APIs
│   ├── types.h                  # stdint.h, common types
│   ├── config.h                 # System configuration
│   ├── hal.h                    # Hardware Abstraction Layer
│   ├── sensor.h                 # Sensor interface
│   ├── motor.h                  # Motor control interface
│   ├── command.h                # Command queue interface
│   ├── statemachine.h           # State machine interface
│   └── nvm.h                    # Non-volatile memory interface
│
├── src/                         # Source files
│   ├── main.c                   # Entry point
│   ├── startup.c                # Startup code (C runtime)
│   ├── system.c                 # System init (clock, peripherals)
│   │
│   ├── hal/                     # HAL implementation
│   │   ├── hal_uart.c           # UART driver
│   │   ├── hal_gpio.c           # GPIO driver
│   │   ├── hal_adc.c            # ADC driver
│   │   └── hal_timer.c          # Timer/PWM driver
│   │
│   ├── drivers/                 # Device drivers
│   │   ├── sensor_tmp102.c      # I2C temperature sensor
│   │   ├── sensor_dht22.c       # 1-wire humidity sensor
│   │   ├── sensor_adc123.c      # ADC-based pressure sensor
│   │   ├── motor_stepper.c      # Stepper motor driver
│   │   └── limit_switch.c       # Limit switch driver
│   │
│   ├── core/                    # Core modules
│   │   ├── command.c            # Command queue (ring buffer)
│   │   ├── statemachine.c       # State machine
│   │   ├── nvm.c                # NVM emulated in Flash
│   │   ├── watchdog.c           # Watchdog supervisor
│   │   └── fault.c              # Fault handling & logging
│   │
│   └── utils/                   # Utilities
│       ├── ringbuffer.c         # Generic ring buffer
│       ├── bitwise.c            # Bit manipulation utils
│       └── checksum.c           # CRC8 calculation
│
├── tests/                       # Unit tests
│   ├── test_ringbuffer.c
│   ├── test_statemachine.c
│   ├── test_command.c
│   ├── test_sensor.c
│   └── test_nvm.c
│
└── docs/                        # Documentation
    ├── architecture.md          # Kiến trúc hệ thống
    ├── memory_map.md            # Bản đồ bộ nhớ
    ├── misra_compliance.md      # MISRA compliance checklist
    └── topic_mapping.md         # Map code với 13 chủ đề
```

## Yêu cầu kỹ thuật

### Phần cứng (giả lập)
- MCU: ARM Cortex-M4 @ 72MHz
- RAM: 32KB (SRAM)
- Flash: 128KB
- UART: 115200 baud
- I2C: 400kHz (cho TMP102)
- GPIO: 8 pins cho motors & switches

### Tính năng chính
1. **Sensor Fusion**: Đọc 4 sensors định kỳ 100ms
2. **Motor Control**: Điều khiển 2 stepper motors với micro-stepping
3. **Command Processing**: Nhận lệnh từ UART, xử lý qua queue
4. **State Machine**: 6 states (INIT, IDLE, RUNNING, PAUSED, FAULT, RESET)
5. **NVM Storage**: Lưu calibration data và fault logs
6. **Watchdog**: Pet watchdog every 100ms, reset if not pet for 1s
7. **Fault Handling**: Detect overtemp, overcurrent, limit switch triggers

### Constraints tuân thủ
- **NO heap allocation** (`malloc`/`new` forbidden - MISRA)
- **Stack size < 2KB**
- **RAM usage < 16KB**
- **Flash usage < 64KB**
- **100% MC/DC coverage** cho safety-critical paths