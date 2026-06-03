# Chủ Đề 2: Cấu Trúc Bộ Nhớ (Memory Layout) & Quá Trình Biên Dịch (Compile Process)

## Lời Mở Đầu: Đằng Sau Nút "Build/Compile"
Khi bạn học C cơ bản trên IDE như Dev-C++ hay Code::Blocks, bạn ấn "Run" và chương trình chạy. Bạn hiếm khi biết điều gì xảy ra bên dưới.
Trong lập trình nhúng, File thực thi cuối cùng (Binary file `.bin` hoặc `.hex`) sẽ được "nạp" (Flash/Burn) vĩnh viễn vào con chip. Bạn CẦN PHẢI BIẾT biến nào được lưu ở đâu, hàm nào chiếm bao nhiêu bộ nhớ, nếu không vi điều khiển sẽ hết dung lượng (Out of Memory) hoặc chạy sai lệch do tràn RAM.

---

## 1. 4 Giai Đoạn Của Quá Trình Biên Dịch (C Build Pipeline)
Một file `main.c` không biến ngay thành mã máy. Nó qua 4 bước:

### Bước 1: Preprocessor (Tiền xử lý)
- **Nhiệm vụ**: Quét toàn bộ các dòng bắt đầu bằng dấu `#` (VD: `#include`, `#define`, `#ifdef`).
- **Hoạt động**: Nó copy toàn bộ nội dung file `.h` dán thẳng vào file `.c`. Nó thay thế tất cả chữ `MAX_SPEED` bằng số `100` (dạng Text-replace). Nó xóa tất cả các chú thích (Comments `//`).
- **Kết quả**: Tạo ra file `.i` (Code C thuần túy khổng lồ).

### Bước 2: Compiler (Trình biên dịch)
- **Nhiệm vụ**: Dịch file `.i` (C language) sang Ngôn ngữ Bậc thấp (Assembly language - `MOV`, `ADD`, `LDR`, `STR`).
- **Hoạt động**: Ở bước này, Trình biên dịch thực hiện kiểm tra Cú pháp (Syntax Check) và Tối ưu hóa (Optimization) loại bỏ các đoạn code thừa, các vòng lặp chết.
- **Kết quả**: Tạo ra file `.s` (Mã Assembly).

### Bước 3: Assembler (Trình hợp dịch)
- **Nhiệm vụ**: Dịch mã Assembly sang Ngôn ngữ Máy (Machine Code - mã nhị phân 0101).
- **Kết quả**: Tạo ra các file Object (`.o` hoặc `.obj`). Tới bước này, mỗi file `.c` ban đầu trở thành một file `.o`. Các file `.o` này chứa các lệnh CPU, nhưng **chưa có địa chỉ vật lý thật**.

### Bước 4: Linker (Trình liên kết) - Quan trọng nhất trong MCU!
- **Nhiệm vụ**: Ghép tất cả các file `.o` lại với nhau.
- **Hoạt động**: Linker nhìn vào một file đặc biệt gọi là **Linker Script** (`.ld` đối với GCC, hoặc `.icf` đối với IAR). Linker Script cho Linker biết con chip này có Flash nằm ở địa chỉ `0x08000000` (size 64KB) và RAM nằm ở `0x20000000` (size 20KB). Linker sau đó sẽ lôi các biến, các hàm từ file `.o` nhét vào đúng các ô nhớ vật lý này! Cuối cùng, Linker "vá" lại các lời gọi hàm giữa các file khác nhau.
- **Kết quả**: File thực thi cuối cùng (`.elf`, `.bin`, `.hex`) để nạp vào MCU, kèm theo file `.map` (Bản đồ chi tiết cho biết hàm A tốn bao nhiêu byte, nằm ở địa chỉ nào).

---

## 2. Bản Đồ Bộ Nhớ (Memory Layout) Của C/C++ Trong MCU
Bộ nhớ MCU được chia thành các Vùng (Sections/Segments) chuyên biệt. Phải thuộc nằm lòng cấu trúc này để kiểm soát RAM/ROM.

### A. Vùng ROM / Flash (Bộ Nhớ Bất Biến - Không mất khi tắt điện)
1. **`.text` section**: 
   - Chứa Mã máy (Instructions) của các hàm. Toàn bộ logic `if-else`, vòng lặp nằm ở đây.
   - Thường chiếm dung lượng lớn nhất.
2. **`.rodata` section (Read-Only Data)**:
   - Chứa các Hằng số (`const uint8_t arr[]`), các chuỗi văn bản (`"Hello World"`), các bảng tra cứu (Lookup Tables).
   - *Mẹo Nhúng*: Đẩy càng nhiều dữ liệu vào `.rodata` càng tốt để cứu vãn RAM.
3. **`.data` init values**:
   - Chứa giá trị khởi tạo của các biến toàn cục (VD: `int score = 50;`). Số `50` này phải được cất ở Flash để khi tắt điện bật lại nó không bị mất.

### B. Vùng RAM / SRAM (Bộ Nhớ Khả Biến - Mất sạch khi tắt điện)
1. **`.data` section**:
   - Chứa các Biến Toàn Cục (Global variables) VÀ Biến Tĩnh (Static variables) CÓ GÁN GIÁ TRỊ BAN ĐẦU (Khác 0).
   - VD: `int score = 50;` hoặc `static float temp = 25.5;`.
   - *Cơ chế hoạt động*: Khi chip vừa cấp nguồn, một đoạn code mồi (Startup code - `Reset_Handler`) sẽ chép copy số `50` và `25.5` từ Flash (mục 3 ở trên) vào vùng RAM `.data` này.
2. **`.bss` section (Block Started by Symbol)**:
   - Chứa các Biến Toàn Cục và Tĩnh KHÔNG CÓ GIÁ TRỊ BAN ĐẦU hoặc gán BẰNG 0.
   - VD: `int buffer[1000];` hoặc `static int count = 0;`.
   - *Cơ chế hoạt động*: Startup code sẽ tự động dùng lệnh vòng lặp Xóa Sạch (Zero-out) vùng RAM này bằng số `0` trước khi gọi hàm `main()`.
3. **Heap (Đống)**:
   - Vùng nhớ dùng để cấp phát động (`malloc()`, `free()`, `new`, `delete`). Bắt đầu từ sau đuôi `.bss` mọc dâng lên trên.
   - **Chuẩn an toàn (MISRA / ISO 26262)**: CẤM SỬ DỤNG HEAP. Do nguy cơ phân mảnh (Memory Fragmentation) và hết bộ nhớ ảo làm hệ thống bị treo cứng (Crash).
4. **Stack (Ngăn Xếp)**:
   - Nằm ở cuối RAM, mọc hướng cắm xuống dưới.
   - Chứa các Biến Cục Bộ (Local variables khai báo trong hàm), Các tham số của hàm (Parameters), và Địa chỉ trả về (Return Addresses) khi gọi hàm lồng nhau hoặc khi có Ngắt (Interrupt).

---

## 3. Khái Niệm Stack Overflow (Tràn Ngăn Xếp)
Do Stack mọc cắm xuống và Heap/BSS mọc dâng lên, nếu chúng đụng nhau trong RAM, thảm họa sẽ xảy ra!
Nếu hàm của bạn có quá nhiều biến cục bộ khổng lồ, hoặc bạn gọi Đệ quy (Recursion - hàm tự gọi chính nó) quá sâu, Stack sẽ dài ra nhanh chóng, xuyên thủng vào vùng `.data` hoặc `.bss`, ghi đè lên các biến toàn cục khác!
Kết quả: Các biến đột nhiên bị đổi giá trị rác. Bug ngẫu nhiên và chết máy.

**Phòng Chống**:
1. Tuyệt đối không dùng Đệ quy (Luật MISRA).
2. Tránh khai báo mảng lớn (Vd: `uint8_t img[2048]`) cục bộ trong hàm. Hãy đưa nó ra ngoài thành biến Global hoặc thêm từ khóa `static` để đẩy nó vào vùng `.bss` (Kiểm soát được size tĩnh lúc biên dịch).

---

## 4. Phân Tích Kích Thước Code Bằng Tiện Ích (`size`)
Sau khi build (biên dịch) xong firmware, Compiler sẽ báo:
```text
text    data    bss     dec     hex filename
15428   204     1088    16720   4150 firmware.elf
```
- **Flash ROM tiêu thụ** = `text` + `data` = 15428 + 204 = 15632 bytes.
- **RAM tiêu thụ (Tĩnh)** = `data` + `bss` = 204 + 1088 = 1292 bytes.
*(Lưu ý: Số RAM này CHƯA bao gồm bộ nhớ Stack động khi chương trình chạy).*

## 5. Từ Khóa `static` Trong C/C++ Nhúng (Cực Kỳ Quan Trọng)

### A. Dùng Trong Một Hàm (Static Local Variable)
```c
void count_clicks() {
    static uint8_t count = 0; // Biến này nằm ở .bss, sống VĨNH VIỄN! Không nằm trên Stack.
    count++;
    printf("Clicks: %d\n", count); // Sẽ in ra 1, 2, 3...
}
```
Khác với biến cục bộ thông thường sẽ bị hủy khi hàm kết thúc, `static` giữ nguyên giá trị qua mỗi lần gọi hàm.

### B. Dùng Ngoài Phạm Vi Hàm (Static Global Variable / Static Function)
```c
// File sensor.c
static uint16_t adc_buffer[10]; // Biến này CHỈ có thể bị truy xuất bởi các hàm bên trong file sensor.c.

static void read_hardware() {  // Hàm này CHỈ gọi được từ trong file sensor.c.
    // ...
}
```
- **Tác dụng**: Giới hạn phạm vi (Scope/Visibility) của biến và hàm. Tương tự như Private trong OOP.
- **Lợi ích Nhúng**: Ngăn chặn tình trạng (Name Collision) khi bạn ghép hàng chục file C của nhiều người code lại với nhau mà có hàm bị trùng tên. Nó tuân thủ chặt chẽ nguyên tắc đóng gói của hệ nhúng an toàn.
