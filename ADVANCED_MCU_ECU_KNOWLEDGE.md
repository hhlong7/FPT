# ADVANCED MCU / ECU KNOWLEDGE

## 1. Muc tieu cua tai lieu nay

Tai lieu nay cung cap context nen de doc 13 chu de trong khoa hoc theo huong:
- Embedded C/C++ thuc chien
- Print firmware, IoT edge device, robot controller
- MISRA C / MISRA C++
- ISO 26262, AUTOSAR thinking, va real-time discipline

Day khong phai la tai lieu thay the cho tieu chuan goc. Muc tieu la giup hoc vien hieu vi sao code embedded khac code desktop va vi sao cac quy tac an toan la bat buoc.

---

## 2. Buc tranh tong the cua mot he thong embedded/ECU

Mot he thong nhung thuong co cac lop:
1. **Hardware**: MCU/SoC, GPIO, ADC, PWM, CAN, LIN, SPI, I2C, UART, Flash, RAM.
2. **Driver/HAL**: lop truy cap thanh ghi va driver ngoai vi.
3. **Middleware/Services**: scheduler, watchdog, NVM manager, communication stack, diagnostics.
4. **Application logic**: state machine, control algorithm, print pipeline, sensor fusion, telemetry.
5. **Host tooling/test**: unit test, trace parser, calibration tools, manufacturing tools.

Trong **printer**, application co the gom rasterizer, paper path control, thermal/ink driver, maintenance logic.
Trong **IoT**, application co the gom sensor acquisition, protocol client, OTA, telemetry, local fail-safe.
Trong **robot**, application co the gom motor control, kinematics, safety zone, fault recovery.

---

## 3. MISRA mindset

### 3.1 Muc dich
MISRA khong chi la "cam" ngon ngu. MISRA giup:
- Giam hanh vi mo ho va implementation-defined
- Lam code doc duoc, review duoc, static-analyze duoc
- Giam loi pointer, ep kieu, control flow, dead code, side effect
- Lam code de chuyen doi giua compiler/MCU

### 3.2 Thoi quen can co
- Dung fixed-width integer types (`uint8_t`, `uint16_t`, `int32_t`)
- Han che implicit conversion
- Tach ro side effect khoi bieu thuc dieu kien
- Khong dung memory dong neu khong that su can
- Interface ro precondition/postcondition
- Co owner ro rang cho resource va bo nho
- So huu danh sach deviation co giai trinh neu phai vi pham rule

### 3.3 Vi du tinh huong MISRA thuc te
- ADC raw 12-bit duoc doc vao `uint16_t`, scale len `int32_t` truoc khi tinh
- Bien set boi ISR phai `volatile`, nhung `volatile` khong thay the lock/atomic
- Khong ep `void*` tu do giua cac loai khac nhau trong driver stack
- Khong viet `if ((a++ < b) && f())` trong code safety path

---

## 4. ISO 26262 mindset

### 4.1 ISO 26262 quan tam dieu gi
ISO 26262 tap trung vao functional safety cua he thong dien/dien tu tren xe, nhung tu duy cua no rat huu ich cho printer cong nghiep, IoT quan trong, robot va may tu dong:
- Hazard analysis
- Safety goal
- ASIL / muc nghiem trong
- Fault detection
- Fault reaction
- Freedom from interference
- Verification va traceability

### 4.2 Cach ap dung vao hoc C/C++
Khi viet mot module, luon hoi:
- Loi module nay co the gay ra hau qua gi?
- Co can phat hien timeout, CRC, range error, stuck signal khong?
- Neu logic sai, he thong co vao safe state duoc khong?
- Co test duoc state transition va error path khong?

### 4.3 Vi du
- Robot axis mat encoder -> dua he thong ve torque off / stop controlled
- Printer qua nhiet dau in -> ngung in, bao loi, cho nguoi dung reset
- IoT gateway ghi Flash loi CRC -> rollback config cu, khong boot voi config hong

---

## 5. AUTOSAR duoi goc nhin de hoc

Khong can hoc AUTOSAR day du moi viet duoc firmware tot, nhung can hieu tu duy phan lop:
- **Application/SWC**: logic nghiep vu
- **RTE-like boundary**: interface giua logic va service
- **BSW-like services**: comm, NVM, diagnostics, watchdog, OS
- **MCAL/HAL**: driver gan MCU

Ngay ca khi khong dung AUTOSAR, ban van nen:
- Tach application khoi thanh ghi phan cung
- Dung interface de unit test logic tren host PC
- Giu communication stack, NVM, diagnostic la service rieng

---

## 6. Real-time constraints

### 6.1 Khai niem co ban
- **Period**: chu ky task
- **Deadline**: han phai xong
- **Jitter**: dao dong thoi diem chay
- **WCET**: worst-case execution time
- **Latency**: do tre phan hoi

### 6.2 Nguyen tac thuc chien
- Ham trong ISR phai ngan, khong block, khong cap phat dong
- Tinh toan nang dua ra task nen
- Logging muc cao khong duoc pha vo deadline control loop
- Moi buffer phai tinh duoc do sau toi da
- Timeout phai dua tren tick/time base ro rang

### 6.3 Vi du
- ISR CAN chi copy frame vao ring buffer va set flag
- Task 1 ms xu ly control loop
- Task 10 ms scale sensor, diagnostics local
- Task 100 ms dong goi telemetry, maintenance counter

---

## 7. Memory layout can nho

### 7.1 Thanh phan bo nho
- `.text`: code trong Flash
- `.rodata`: hang so, bang tra, calibration readonly
- `.data`: bien global static co khoi tao, nam Flash luc build va copy sang RAM khi boot
- `.bss`: bien global static chua khoi tao, zero-init khi boot
- `stack`: local variable, return address, ISR frame
- `heap`: neu co dung thi phai kiem soat rat chat

### 7.2 Nguyen tac
- Uu tien static allocation
- Biet module nao an nhieu stack
- Cac bang lon co the dat vao Flash
- Tranh de recursion trong firmware safety-critical
- Kiem soat alignment va packed struct khi map protocol/register

---

## 8. NVM / Flash patterns

### 8.1 Van de
Flash co gioi han wear, ghi cham, erase theo sector, co the mat nguon giua chu ky ghi.

### 8.2 Mau thiet ke thuong gap
- Double-buffer config block
- Header + version + length + CRC
- Valid marker chi set o cuoi sau khi ghi xong
- Wear leveling don gian bang vong block
- Deferred write queue tranh ghi trong ISR hay duong nong

### 8.3 Vi du
- Printer luu page counter va maintenance count theo lo
- IoT node luu network credential theo block co CRC
- Robot luu calibration home offset va rollback neu CRC sai

---

## 9. Communication basics cho ECU / IoT

### 9.1 CAN/LIN/UART/SPI/I2C
- CAN phu hop distributed control, deterministic hon Ethernet thong thuong
- LIN cho node don gian, toc do thap, gia re
- UART tot cho trace/debug va module ngoai
- SPI nhanh, full duplex, can quan ly chip-select va timing
- I2C don gian nhung de gap clock stretch, bus hang, address conflict

### 9.2 Nguyen tac protocol parsing
- Kiem tra DLC/do dai frame truoc khi unpack
- Khong trust payload tu bus/network
- Tach parser va action handler
- Dung endianness ro rang
- Kiem tra timeout/missing frame cho tin hieu quan trong

---

## 10. Diagnostics va serviceability

### 10.1 Muc tieu
- Biet he thong dang lam gi
- Biet vi sao fail
- Co du lieu de debug ma khong can phan cung dat tien ngay tu dau

### 10.2 Nen co
- Error code / DTC map ro rang
- Counter cho reset reason, watchdog reset, comm timeout
- Trace level phu hop build mode
- Session/service concept cho diagnostic command
- Log co timestamp hoac sequence number

### 10.3 Vi du
- Printer ghi lai fault: paper jam, overheat, head open
- IoT ghi TLS handshake fail, sensor timeout, brown-out reset
- Robot ghi overcurrent, limit switch fault, position mismatch

---

## 11. Watchdog va fault handling

### 11.1 Watchdog khong phai la reset ngau nhien
Watchdog la co che phat hien he thong khong con tien trien dung. Kick watchdog chi nen xay ra khi cac module quan trong van khoe.

### 11.2 Mau dung
- Supervision point theo task
- Window watchdog neu co
- Fault counter truoc khi reset
- Boot reason logging sau reset

### 11.3 Mau sai
- Kick watchdog trong timer ISR bat ke he thong co treo hay khong
- Reset fault flag qua som truoc khi log

---

## 12. Power modes

Nhieu he thong nhung can cac trang thai:
- Run
- Idle
- Sleep
- Deep sleep
- Wakeup handling

Can xac dinh ro:
- Module nao duoc phep tat clock
- Du lieu nao phai luu truoc khi sleep
- Nguon wakeup nao hop le
- Thoi gian khoi phuc co dap ung deadline khong

Vi du:
- IoT sensor node ngu 99% thoi gian, chi day theo RTC hoac GPIO
- Printer vao standby sau 5 phut nhan roi, nhung van giam sat nap in / cover open
- Robot controller can controlled stop truoc khi ve low-power

---

## 13. ISR safety va task communication

### 13.1 Chi nen lam gi trong ISR
- Clear interrupt source
- Snapshot du lieu nho
- Push vao queue/ring buffer
- Set flag/semaphore/event

### 13.2 Khong nen lam gi
- Ghi Flash
- Cap phat dong
- Parse protocol lon
- Goi API co the block
- In log dai dong

### 13.3 Mau giao tiep an toan
- Single producer / single consumer ring buffer
- Atomic flag
- Double buffering cho ADC/sample stream
- Message queue co gioi han do sau

---

## 14. State machine la ky nang cot loi

State machine nen duoc dung cho:
- Startup/shutdown
- Print job lifecycle
- OTA update
- Robot homing
- Diagnostic session
- Fault recovery

Mot state machine tot can:
- State ro rang
- Event ro rang
- Guard condition ro rang
- Action ro rang
- Default path an toan khi event khong hop le

---

## 15. Test strategy cho embedded

### 15.1 Kim tu thap den cao
1. Static analysis
2. Unit test tren host
3. Integration test voi mock driver
4. HIL test
5. Fault injection
6. Timing/load test

### 15.2 Nhung test de bi bo quen
- Boundary test
- Timeout test
- CRC fail test
- Brown-out / reset recovery
- Queue full / buffer overflow reject
- State transition invalid
- Lost frame / duplicate frame

---

## 16. Review checklist nhanh

- Kieu du lieu da fixed-width chua?
- Co implicit conversion nguy hiem khong?
- Pointer co ro ownership va lifetime khong?
- Buffer co bound check khong?
- ISR co ngan va non-blocking khong?
- Co timeout, CRC, range check o input boundary khong?
- Co safe state khi fault khong?
- Co test cho error path khong?
- Co phu hop MISRA va giai trinh deviation neu can khong?

---

## 17. Cach dung tai lieu nay khi hoc 13 topic

- Topic 1-5: doi chieu moi khái niem voi memory map, real-time, protocol va ISR safety.
- Topic 6-10: hoc cach dung C++ de code de test hon, nhung khong duoc lam tang unpredictability.
- Topic 11-13: hoc abstraction nang cao, toi uu va pattern, nhung van uu tien determinism, readability, va safety.

Neu phai chon giua code “hay” va code “du doan duoc, review duoc, test duoc”, trong embedded/ECU hay chon ve phia thu hai.
