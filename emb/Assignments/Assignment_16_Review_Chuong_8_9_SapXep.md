C/C++
Training Assignment

| Document Code | 25e-BM/HR/HDCV/FSOFT |
| --- | --- |
| Version | 2.0 (Embedded Print/IoT Edition) |
| Effective Date | 18/08/2025 |

Danang, 08/2025

Contents

|  | CODE: <CPP.Assignment16> TYPE: <Review_Chuong_8_9> LOC: <Lines of Code> DURATION: <240 minutes> |

## Review Chương 8-9: Data Structures & Templates

### Exercise 1: Generic RingBuffer Với Template
> [!TIP]
> **Hướng dẫn tự học:** Bài tập này tích hợp kiến thức chuyên sâu **Chương 08 (Templates)** và **Chương 09 (Data Structures)**. Hãy tham khảo mẫu code thiết kế hàng đợi generic tại [08_Namespace_Templates/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/08_Namespace_Templates/example.md) (Case 6) và [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 2, 7).

**Ngữ cảnh**: Cần implement Ring Buffer cho nhiều kiểu dữ liệu khác nhau.
```cpp
template<typename T, uint16_t SIZE>
class RingBuffer {
    T data[SIZE];
    uint16_t head;
    uint16_t tail;
public:
    bool push(const T& item);
    bool pop(T& item);
};
```
**Yêu cầu**:
1. Viết implementation đầy đủ cho `push()` và `pop()`
2. Sử dụng `static_assert` để đảm bảo SIZE là lũy thừa của 2
3. Giải thích tại sao dùng `(idx + 1) & (SIZE - 1)` thay vì `% SIZE`
4. Trình bày trade-off khi dùng template: code bloat vs performance

### Exercise 2: Memory Pool Cho Packet Allocation
> [!TIP]
> **Hướng dẫn tự học:** Hãy xem kỹ phương pháp phân bổ bộ nhớ không phân mảnh và an toàn đa nhiệm tại **Chương 09 (Cấu trúc dữ liệu)** và **Chương 11 (std::atomic)**. Tham khảo [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 6) và [11_Advanced_Cpp_Features/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/11_Advanced_Cpp_Features/example.md) (Case 5).

**Ngữ cảnh**: Network subsystem cần cấp phát packet buffers nhanh mà không dùng heap.
**Yêu cầu**:
1. Thiết kế Memory Pool với 16 blocks, mỗi block 256 bytes
2. Viết `PoolAlloc()` và `PoolFree()` sử dụng bitmap array `bool block_used[16]`
3. Thêm thread-safety với `std::atomic<uint16_t>` cho bitmap
4. Tính toán maximum RAM consumption: 16 × 256 = 4KB + overhead

### Exercise 3: Binary Search Trong DTC Table
> [!TIP]
> **Hướng dẫn tự học:** Bài tập củng cố thuật toán nhị phân tối ưu cho MCU tại **Chương 09 (Cấu trúc dữ liệu và giải thuật)**. Hãy xem kỹ ví dụ tra cứu bảng hiệu chuẩn nhiệt độ tại [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 4, 8).

**Ngữ cảnh**: Bảng mã lỗi 1000 entries đã sorted theo ID.
**Yêu cầu**:
1. Viết binary search function: `const char* FindDTC(uint16_t id)`
2. Tính số lần so sánh tối đa cho 1000 elements (log₂(1000))
3. So sánh với linear search: 1000 so sánh → 10 so sánh (10x improvement)
4. Nếu cần tìm kiếm cả message description, thiết kế struct nào để binary search hiệu quả?

### Exercise 4: Sắp Xếp Các Bước Binary Search
> [!TIP]
> **Hướng dẫn tự học:** Hãy ôn tập thuật toán tìm kiếm nhị phân trên bảng ROM đã được hướng dẫn chi tiết tại [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 4).

**Yêu cầu**: Sắp xếp các bước của binary search algorithm đúng thứ tự:
1. mid = (low + high) / 2
2. If arr[mid] == target → return found
3. low = 0, high = size - 1
4. If arr[mid] < target → low = mid + 1
5. If arr[mid] > target → high = mid - 1
6. Repeat until low > high → return not found

### Exercise 5: Sắp Xếp Thứ Tự Template Instantiation
> [!TIP]
> **Hướng dẫn tự học:** Ôn tập quy chế biên dịch hằng và trương nở mã nguồn do lạm dụng template tại **Chương 08 (Templates)**. Tham khảo [08_Namespace_Templates/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/08_Namespace_Templates/example.md) (Case 5, 8).

**Yêu cảnh**: Khi compiler gặp template, các bước instantiation diễn ra theo thứ tự.
**Yêu cầu**: Sắp xếp đúng thứ tự:
1. Compiler sees `RingBuffer<uint8_t, 64> rb;`
2. Compiler checks if SIZE is power of 2 (via static_assert)
3. Compiler generates concrete class: `RingBuffer_uint8_64`
4. Compiler generates concrete methods: push(), pop()
5. Linker combines all template instantiations
6. Dead code elimination removes unused instantiations

---

## Bài Tập Sắp Xếp (Ordering Exercises)

### Exercise 6: Sắp Xếp Quy Trình Cấp Phát Từ Memory Pool
> [!TIP]
> **Hướng dẫn tự học:** Hãy xem lại thuật toán cấp phát tĩnh không phân mảnh Memory Pool tại [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 6).

**Yêu cầu**: Sắp xếp các bước khi gọi `PoolAlloc()` đúng thứ tự:
- A. Caller requests memory: `uint8_t* buf = PoolAlloc(256);`
- B. Scan block_used[] from index 0 to find first free block
- C. Set block_used[i] = true (mark as used)
- D. Return pointer to block i start address
- E. If no free block found → return NULL
- F. Caller uses buffer for data storage

### Exercise 7: Sắp Xếp Thứ Tự Iterator Pattern Cho RingBuffer
> [!TIP]
> **Hướng dẫn tự học:** Quy tắc duyệt vòng đệm Ring Buffer không dùng modulo được hướng dẫn chi tiết tại [09_DataStructures_Algorithms/example.md](file:///d:/Workspaces/C_CPP/C_CPP_EMB_PRINT_IOT/09_DataStructures_Algorithms/example.md) (Case 2, 7).

**Yêu cầu**: Sắp xếp thứ tự khi traverse qua RingBuffer:
1. Start from tail index
2. Check if tail == head (buffer empty)
3. Read data[tail] into variable
4. Increment tail using `tail = (tail + 1) & (SIZE - 1)`
5. Process data
6. Repeat until tail == head
7. Return when buffer empty