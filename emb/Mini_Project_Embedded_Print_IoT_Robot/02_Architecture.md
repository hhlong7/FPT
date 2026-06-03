# 02. Architecture

## Khoi chuc nang
1. **HAL/Driver Layer**
   - ADC
   - GPIO/PWM
   - UART/CAN
   - Flash/NVM
   - Timer/Watchdog

2. **Service Layer**
   - SensorService
   - Filter/ScalingService
   - MotionOrActuatorService
   - CommService
   - NvStorageService
   - DiagnosticService
   - WatchdogSupervisor

3. **Application Layer**
   - SystemStateMachine
   - CommandDispatcher
   - FaultManager
   - TelemetryManager

## Data flow
- ISR/DMA thu du lieu nho gon -> push queue/flag.
- Service task lay du lieu -> scale/filter -> cap nhat state.
- Application nhan event -> quyet dinh action -> phat command.
- Fault/evidence -> Diagnostic + NvStorage.

## Task model de xuat
- ISR: capture sample/frame/edge event
- Task 1 ms: control loop / actuator update
- Task 10 ms: sensor scaling, fault check, state update
- Task 100 ms: telemetry, maintenance, deferred NVM

## State machine tong quat
- Boot
- SelfTest
- Ready
- Active
- Fault
- Sleep

## Fault model co ban
- Sensor out-of-range
- Comm timeout
- NVM CRC fail
- Watchdog miss
- Over-temperature / over-current / jam / stall tuy domain

## Nguyen tac kien truc
- Application khong truy cap thanh ghi truc tiep.
- Queue/event o boundary ISR-task.
- Module public API nho va testable.
- Moi du lieu persistent co version + CRC neu can.
