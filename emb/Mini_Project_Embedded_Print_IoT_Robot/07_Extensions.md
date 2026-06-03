# 07. Extensions

## Nhanh 1: Printer Extension
Them vao capstone:
- Paper path state
- Cover/door switch
- Thermal control / fuser temperature
- Print job queue
- Jam detection va maintenance counter

### Bai tap goi y
- Them state `WarmingUp`, `Printing`, `Cooling`.
- Them fault `PaperJam`, `DoorOpen`, `OverTemp`.
- Them deferred NVM write cho page counter.

## Nhanh 2: IoT Extension
Them vao capstone:
- MQTT/CoAP session logic
- reconnect policy
- low-power sleep/wakeup
- telemetry batching
- OTA metadata state

### Bai tap goi y
- Them state `NetworkJoin`, `Publish`, `Sleep`.
- Them timeout test cho connect/publish.
- Them retention data sau sleep.

## Nhanh 3: Robot Extension
Them vao capstone:
- Encoder feedback
- motion loop 1 ms
- homing sequence
- limit switch / overcurrent fault
- safe stop va recovery

### Bai tap goi y
- Them state `Homing`, `Ready`, `Moving`, `FaultStop`.
- Them queue waypoint bounded.
- Them stuck sensor / stalled motor detection.

## Cach chon nhanh
- Chon **Printer** neu muon hoc queue + thermal + state machine co nhieu fault field.
- Chon **IoT** neu muon hoc telemetry + low-power + NVM/network retry.
- Chon **Robot** neu muon hoc timing + control loop + fault latency thap.
