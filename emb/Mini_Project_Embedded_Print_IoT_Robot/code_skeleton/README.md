# Code Skeleton

Day la skeleton hoc tap cho capstone Embedded Edge Controller.

## Muc tieu
- Cung cap cau truc source code sat firmware that.
- Cho hoc vien thay ro boundary giua HAL, service, application.
- Minh hoa luong du lieu tu input -> processing -> state -> fault -> diagnostics.

## Cau truc
- `include/`: public types va API
- `src/`: implementation core
- `tests/`: test skeleton minh hoa

## Luong chinh
1. Sensor raw/sample hoac command frame duoc dua vao queue.
2. Service layer xu ly va cap nhat du lieu he thong.
3. State machine quyet dinh trang thai va fault reaction.
4. Diagnostics va watchdog supervision lay thong tin tu service/app layer.
5. NVM luu config va fault evidence theo block co CRC.

## Luu y
- Skeleton nay uu tien ro rang hon toi uu build.
- Uu tien static allocation, fixed-size buffer, status code minh bach.
- Khong dung heap trong path target.
