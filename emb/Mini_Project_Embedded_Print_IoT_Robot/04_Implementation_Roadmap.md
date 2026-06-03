# 04. Implementation Roadmap

## Giai doan 1: Nen C-first
- Dinh nghia kieu du lieu, enum, error code.
- Tao sensor raw model va scaling helper.
- Viet parser buffer + length cho frame command.
- Tao state machine enum co ban.

## Giai doan 2: Memory va runtime safety
- Chon memory layout.
- Dinh queue/ring buffer cho event va frame.
- Dinh NVM block format version + CRC.
- Xac dinh watchdog evidence block.

## Giai doan 3: Service API
- SensorService
- CommService
- NvStorageService
- FaultManager
- TelemetryManager

## Giai doan 4: OOP / abstraction
- Tach HAL interface.
- Dung composition cho application manager.
- Them strategy cho filter neu can.

## Giai doan 5: Unit test va replay
- Fake time source
- Mock ADC/CAN/UART/NVM
- Test state machine
- Test CRC fail, timeout, queue full

## Giai doan 6: Toi uu va review
- Do stack/latency
- Kiem tra wrap-around, saturation, overflow
- Review heap policy, ISR policy, logging policy

## Giai doan 7: Mo rong theo domain
- Printer: paper path + thermal + job status
- IoT: telemetry + retry + sleep policy
- Robot: motion loop + encoder + fault stop
