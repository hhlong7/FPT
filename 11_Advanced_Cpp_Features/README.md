# Topic 11: Advanced C++ Features

## Muc tieu
- Hieu cac tinh nang C++ nang cao nhung van phu hop embedded.
- Dung move semantics, RAII, `constexpr`, `enum class`, `noexcept`, `atomic` dung cho cho.
- Tranh dung tinh nang hien dai mot cach mo ho gay tang chi phi va giam predictability.
- Tang safety, ownership ro rang, va communication an toan giua ISR/task.

## Pham vi
- Modern C++ cho firmware va module host-side support.
- Uu tien determinism, code reviewability, va chi phi ro rang.
- Gan voi print buffer handling, IoT session object, robot motion resource.

## Ban se hoc duoc gi
- Khi nao move semantics co y nghia trong embedded.
- Cach dung smart pointer co kiem soat o boundary phu hop.
- Cach dung `atomic` va memory ordering o muc thiet yeu.
- Cach viet type-safe API bang `enum class`, `constexpr`, `span`-like pattern.

## Case study lien quan
- Ownership cua DMA buffer.
- RAII cho interrupt masking va lock.
- Atomic flag giua ISR va task.
- Session object cho communication stack.

## Dieu kien truoc
- Topic 06-10.

## Lien ket hoc tiep
- Topic 12 de toi uu va phat hien defect.
- Topic 13 de dua abstraction nang cao vao pattern ung dung.

## Checklist hoc
- Liet ke tinh nang C++ hien dang dung va xem co dang phu hop embedded khong.
- Xac dinh object nao nen move-only, object nao nen non-copyable.
- Doc `material.md` va `example.md`.
