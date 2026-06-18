#include <stdint.h>

//ex1

//when the index reach 64, that means it reach 1 byte past the buffer size, since its only from 0->63
//so that means that wtvr will be there will be overwritten by that byte => cause crash

volatile uint8_t rx_buffer[64U];
volatile uint16_t rx_index = 0U;
volatile uint8_t rx_flag = 0U;

void UART_Receive_ISR(void) {
    uint8_t incoming_byte = UART_Read_Data_Register();

    if (rx_index >= 64U) {
        rx_index = 0U;
        rx_flag = 1U;
        return;
    } //if theres no \n then set a flag error so that we can return/exit the function

    rx_buffer[rx_index] = incoming_byte;
    rx_index++; //if doesnt pass 64byte, store inc byte into buffer, i++

    if (incoming_byte == '\n') {
        process_gcode();
        rx_index = 0U;
        rx_flag = 0U;
    } //if theres \n then reset
}


//ex 2


//the hardware can only read an addy that is divisible by 4 from a 32 bit value => natural alignment
//unaligned 32-bit would result in hardfault 

//strict aliasing rule means that in c float* and int* cant point to the same mem location


//fix: use memcpy, since it copies byte for byte, => never miss aligned a 32-bit val

#include <string.h>

void parse_payload(const uint8_t* payload) {

    float temp = 0.0f; //float
    uint32_t pres = 0U; //unsigned int
    memcpy(&temp, &payload[0], sizeof(float));
    memcpy(&pres, &payload[4], sizeof(uint32_t));

}

//ex 3

//problem: they both points to one shared buffer, data might be overwritten when its being processed -> wrong audio
//=> double buffer, so that adc/dma write to one, buffer read from another, swap when done

#include <stdint.h>

static uint8_t bufferA[512U];
static uint8_t bufferB[512U];

static volatile uint8_t *p_write = bufferA; //dma writes to here
static volatile uint8_t *p_read = bufferB; //main loop read from this buffer
static volatile uint8_t finish = 0U; //flag to indicate when done with swapping

void swapping(void) {
    volatile uint8_t *temp;
    temp = p_read;
    p_read = p_write;
    p_write = temp;
    finish = 1U;
    
    DMA_transfer((uint16_t*)p_write, 512U);
}

void main(void) {
    while (1) {
        if (finish == 1U) {
            finish = 0U; //reset
            process_aud((uint16_t*)p_read, 512U);
        }
    }
}


//ex 4

/* #include <cstring>

struct WifiConfig {
    char ssid[32];
    char pass[32];
    uint8_t channel;
};

void load_wifi_defaults(WifiConfig& cfg) {

    std::strncpy(cfg.ssid, "DefaultNet", sizeof(cfg.ssid) - 1U);
    cfg.ssid[sizeof(cfg.ssid) - 1U] = '\0'; //nul terminate for no overflow
    cfg.channel = 6U;

} */ //no need to check for null since reference guarantess that val will be valid

//reference bc no null is allowed
//dot for a clearer syntax

//if the value is either not null or null, then we have to use pointer, since ref . cant be null


//ex 5

//because it is called within a function, so when the function is caleld, it will allocate mem in the stack
//when the func return, it'll dellocate the memory for status_str

//=> result in dangling ptr

//because when lcd func is called, it will overwrite on the stack with their own local var

//fix

//fix 1, call the function but have the stu str as a static variable, so itll be in .bss

char* get_printer_status1(int code) {
    static char status_str[50];
    if (code == 0) strcpy(status_str, "Ready");
    else strcpy(status_str, "Error");
    return status_str;
} 

//fix 2, pass in a buffer from the caller func

#include <string.h>

void get_printer_status2(int code, char* out_buf, uint16_t buf_size) {
    if (out_buf == NULL || buf_size == 0U) 
        return;
    if (code == 0) strncpy(out_buf, "Ready", buf_size - 1U);
    else strncpy(out_buf, "Error", buf_size - 1U);
    out_buf[buf_size - 1U] = '\0';
}

void printer(void) {
    char status_buf[50];
    get_printer_status2(0, status_buf, sizeof(status_buf));
    lcd_print(status_buf);
}