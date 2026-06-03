# 01. Project Context

## Bai toan san pham
Can thiet ke mot bo dieu khien nhung co the dung cho 3 kieu san pham:
- printer subsystem
- IoT edge node
- robot subsystem controller

He thong can:
- doc sensor dinh ky
- xu ly du lieu va phat hien fault
- dieu khien co cau/actuator
- giao tiep voi ben ngoai
- luu config va fault evidence
- ve safe state khi co loi

## Yeu cau chuc nang
1. Doc 2-4 kenh sensor.
2. Scale va loc du lieu.
3. Xu ly state machine startup/ready/active/fault/sleep.
4. Co command interface de nhan lenh.
5. Co telemetry/diagnostic output.
6. Co luu config trong NVM.
7. Co watchdog supervision.

## Yeu cau phi chuc nang
- Deterministic timing o path quan trong.
- Khong dung heap vo toi.
- Boundary input phai validate.
- Fault path phai audit duoc.
- De unit test tren host truoc khi len board.

## Dinh huong domain
- Printer uu tien paper path, nhiet, print status.
- IoT uu tien telemetry, low-power, retry/network.
- Robot uu tien motion loop, encoder, fault latency.

## Dinh nghia thanh cong
Du an thanh cong khi hoc vien co the:
- mo ta architecture va fault model
- viet duoc module theo tung topic
- test duoc tren host
- giai thich duoc trade-off safety/performance/memory
