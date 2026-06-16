#include <stdint.h>
//ex1

//1 = .rodata, in the flash rom, since using const
//2 = data, non zero init data, at boot is copied to ram from flash
//3 = .bss, global array, no init data, zero fills b4 main()
//4 = stack, local var, since declare in a func
//5 = .bss, global within the func, use static, live in ram like global

//copy .data from flash to ram, and zero fill .bss in ram (reset_handler)

//ex2

//it crashed because the stack is allocated for 8kb, and it grows downwards
//crashes down to .bss section, which grows upwards, and they collide

//1

static uint8_t image_buffer[8192];
void process_display_image() {
    draw_to_oled(image_buffer);
}

//uu diem: size is fixed, no dynamic allocation, stored in .bss, linker would error out if memory doesnt fit
//con: permanently occupies in .bss, even when it is not used, memory shortage and ram would fill up quickly
//if need for multiple other memory and funcitons

//2

void process_display_image() {
    static uint8_t image_buffer[8192]; 
    draw_to_oled(image_buffer);
}

//uu: within the function, only allocated when the function is called, and deallocated when the function returns, so it does not permanently occupy memory in .bss, and can be used for other functions when not in use
//con: the 8kb must be reserved in the .bss, 

//ex 3

//memory fragmentation (bo nho bi phan manh)
//|used|free|used|free|used|free|, there are free spaces but it is not big enough to allocate 
//a new block of memory, so it fails to allocate memory even though there is free space available

//misra c: 4.12, 21.3 / 21.4, 
//MISRA C++: Quy tắc 18-4-1
//they ban using malloc/free

#include <string.h>

static uint8_t memory_pool[4U][512U]; //4 blocks of 512 bytes each
static uint8_t pool_used[4U]; //0 means free, 1 means used

static uint8_t* allocate(void) {
    for (uint8_t i = 0U; i < 4U; i++) {
        if (pool_used[i] == 0U) {
            pool_used[i] = 1U;
            return memory_pool[i];
        }
    }
    return NULL;
}

static uint8_t deallocate(uint8_t* ptr) {
    for (uint8_t i = 0U; i < 4U; i++) {
        if (memory_pool[i] == ptr) {
            pool_used[i] = 0U;
            return;
        }
    }
}

void parse_wifi_packet(uint16_t packet_size) {
    if (packet_size > 512U) { return; } //to avoid large/oversized packets 

    uint8_t* json_payload = allocate();
    if (json_payload != NULL) {
        wifi_read(json_payload, packet_size);
        process_json(json_payload);
        deallocate(json_payload);
    }
}

//ex 4

/* Compiler flags (add to CFLAGS):
-ffunction-sections   Each function gets its own .text section
-fdata-sections       Each global variable gets its own .data/.bss section

# Linker flag (add to LDFLAGS):
--gc-sections         "Garbage Collect sections" — remove all unused ones */

/* arm-none-eabi-gcc -O2 -ffunction-sections -fdata-sections \
    -Wl,--gc-sections \
    main.c uart.c math_lib.c -o firmware.elf */

//with ffunction-sections n fdata-scetions, each function and glob var has its own mini section,
//ex: .text.cos, .text.sin, .text.matmul, etc

//with gc-sections, the linker would remove all the sections that are not referenced in the code, 
//so if we only use cos and sin, the sections for matmul and other unused functions would be removed


//ex 5

//9600 = 0x2580, where 0x25 is high, 0x80 is low, so in lil endian, its 0x80 => 128

//extern means this is a 1-byte variable at the symbol address baud_rate

//so it reads exactly 1 byte from the addy of baud_rate, which is 0x80, and returns 128, and 0x25 is ignored


//in c, it wouldnt catch this, since it has no concept of type safety
//but in c++, it would catch this, result in link error
//cpp use name mangling, so the symbol for baud_rate would be something like _Z9baud_rate, 
//and the linker would not find it when trying to link with the assembly code that defines 
//baud_rate as a 2-byte variable, resulting in a link error.

