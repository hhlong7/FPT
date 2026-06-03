# Topic 03: Arrays, Pointers & Reference

## Muc tieu
- Su dung array va pointer dung ky luat trong embedded.
- Hieu ownership, aliasing, lifetime, buffer boundary.
- Mapping frame CAN/LIN/UART an toan.
- Chuyen tiep tu C pointer sang C++ reference mot cach co kiem soat.

## Pham vi
- C-first, co lien he voi C++ reference.
- Uu tien memory safety, protocol parsing, register access.
- Ap dung cho print buffer, IoT payload, robot sample window.

## Ban se hoc duoc gi
- Khi nao dung array tĩnh, khi nao truyen pointer + length.
- Cach tranh out-of-bounds, use-after-free, null dereference.
- Cach unpack frame ma khong phu thuoc alignment nguy hiem.
- Cach giai thich reference cho nguoi da hoc pointer.

## Case study lien quan
- CAN signal unpacking.
- Ring buffer UART.
- Bang calibration static.
- Trao doi mau ADC giua ISR va task.

## Dieu kien truoc
- Topic 01 va Topic 02.

## Lien ket hoc tiep
- Topic 04 de hoc interface function va passing convention.
- Topic 12 de hoc defect prevention va buffer safety nang cao.

## Checklist hoc
- Tim 5 loi pointer thuong gap va viet cach phong tranh.
- Luyen viet API theo dang `buffer + length`.
- Doc 10 case study trong `example.md`.
