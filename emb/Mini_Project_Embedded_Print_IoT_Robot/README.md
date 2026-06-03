# Mini Project: Embedded Edge Controller for Print / IoT / Robot

## Muc tieu
Mini project nay dong vai tro capstone de noi 13 topic thanh mot he thong hoan chinh. Day la mot **Edge Controller** co kha nang:
- thu thap sensor
- dieu khien co cau chuyen dong
- giao tiep voi may chu/host
- luu NVM an toan
- xu ly fault va watchdog
- ho tro mo rong thanh 3 nhanh: printer, IoT node, robot controller

## Ban se hoc duoc gi
- Chuyen kien thuc tu kieu du lieu den architecture.
- Lap rap module theo thu tu de test duoc.
- Dung state machine, queue, NVM pattern, diagnostics, timing discipline.
- Review code theo MISRA/ISO 26262 mindset.

## Cau truc tai lieu
- `01_Project_Context.md`
- `02_Architecture.md`
- `03_Topic_Mapping.md`
- `04_Implementation_Roadmap.md`
- `05_Test_Strategy.md`
- `06_Review_Checklists.md`
- `07_Extensions.md`
- `code_skeleton/` - source code hoc tap sat firmware that

## Code skeleton moi
Trong `code_skeleton/`, ban se thay mot he thong mau co:
- `include/` cho public API va data types
- `src/` cho service, state machine, NVM, fault, diagnostics, watchdog
- `tests/` cho skeleton unit test

Ngoai skeleton chinh, da co them cac mini lab cho nen Topic 01-05:
- `sensor_parser.c`
- `command_parser.c`
- `log_formatter.c`
- `fault_evidence_codec.c`
- `test_bit_ops.cpp`
- cac test mini lab trong `tests/`

De hoc theo bai ban, doc them `..\mini_lab_guide.md`, `..\foundation_practice.md` va `..\review_checklist_foundation.md` song song voi code. Chi xem `code_skeleton/sample_solutions/` sau khi da tu lam va tu review.

Neu muon hoc sat thuc te hon, hay doc markdown va mo code song song. Day la cach nhanh nhat de noi ly thuyet voi implementation.

## San pham trung tam
Mot board controller quan ly:
- 2 cam bien analog
- 1 encoder
- 1 motor/actuator output
- 1 kenh CAN/UART
- 1 kenh network logic (co the la MQTT/telemetry)
- 1 bo NVM config + fault evidence

## Bien the domain
- **Printer:** edge controller tro thanh paper-feed + thermal/fuser + print job status controller.
- **IoT:** edge controller tro thanh sensor gateway low-power + telemetry + OTA-aware node.
- **Robot:** edge controller tro thanh 1 axis/1 subsystem motion controller co fault handling.

## Cach hoc de hieu qua
1. Doc `..\master_learning_map.md` de thay toan canh 01-13.
2. Doc `01_Project_Context.md` de hieu bai toan.
3. Doc `02_Architecture.md` de nam block va data flow.
4. Doi chieu `03_Topic_Mapping.md` de thay moi topic dong gop gi.
5. Lam theo `04_Implementation_Roadmap.md` tung giai doan.
6. Dung `05_Test_Strategy.md` va `06_Review_Checklists.md` de tu review.
7. Neu muon di sau hon, doc `07_Extensions.md` de chon nhanh printer/IoT/robot.
