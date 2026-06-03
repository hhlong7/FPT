# 05. Test Strategy

## Muc tieu
Dam bao hoc vien khong chi viet duoc module, ma con kiem tra duoc behavior quan trong truoc khi dua len board.

## Tang test
1. **Static analysis / warning clean**
2. **Unit test tren host**
3. **Integration test voi fake/mock services**
4. **Replay test tu file/log**
5. **HIL test co chon loc**
6. **Fault injection**

## Nhom test bat buoc
- Boundary value
- Timeout
- CRC fail
- Queue full/empty/wrap-around
- Invalid state transition
- Sensor out-of-range
- Comm timeout
- Reset recovery

## Test cho state machine
Can assert:
- state moi
- action side effect
- fault latch/clear policy
- timeout reaction

## Test cho NVM
- version mismatch
- CRC fail
- partial write/power-loss model
- rollback/default policy

## Test cho concurrency
- ISR-task handoff
- flag/queue ownership
- no blocking in ISR

## Do luong nen co
- line/branch coverage cho logic quan trong
- stack watermark
- control loop jitter
- queue high watermark
