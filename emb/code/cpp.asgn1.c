#include <stdint.h>

/* ex1
since the struct data is sent raw,  the signal was sent from a 8-bit avr to a 32 bit arm
where int on 8bit avr is 16bit while its 32 bit for arm
=> different in size */

#pragma pack(push, 1)
typedef struct{
    int16_t temp; //to make sure that the bytes are correct + -32768 -> 32767
    uint32_t timestamp; //best for time, from 0 to 4 bil, good for milliseconds
} SensorData;
#pragma pack(pop)

void pack_signal(int16_t temp, uint32_t output[2]){
    output[0] = (uint16_t)((temp >> 8) && 0xFF); //high bytes, 25C = 0x19 -> 0x00 send to the big endian
    output[1] = (uint16_t)(temp && 0xFF); //the og bytes, 25c = 0x19 -> 0x19 send to the big endian
}

int16_t get_signal(const uint8_t input[2]){
    return (int16_t)((input[0] << 8) | input[1]); //combine high and low bytes => og byte to lil endian
}

/* ex2 */

//the reason why this happen is because
//in the while loop block, the value system_update is not being modified
//since the value is passed down by a copy version of it from timer_isr
//so it is forever stuck in the state

// => fix
volatile uint32_t system_uptime = 0;
//this means that the variable can be changed outside of a program, read from ram on every access
//the cpu loads the value of system_update straight from the addy of it in RAM


//ex3

//the data is store in .rodata(rom), at boot, mcu copies it into .data(sram)
//=> wasted sram, since the data doesnt change, also the cpu only needs to read them
//not write to them, => store in .rodata but const char

const char msg_warning1[] = "Canh bao: Nhiet do cao!";
const char msg_warning2[] = "Canh bao: Ket nhua o dau in!";
const char msg_warning3[] = "Canh bao: Mat ket noi Wifi!";

//cpu read directly from .rodata (rom,) no need for copy to sram, so save waste

//ex4

//bit config: | 7 6 5 4 | (error msg), |3|, |2| bed, |1| heater, |0| fan

#define hardware_stu_reg 0x40021000
#define mask_error_msg (0x0FU << 4) //=> 0000 1111 -> 1111 0000
volatile uint8_t* const hardware_stu = (volatile uint8_t*)hardware_stu_reg;

void hardware(void){
    *hardware_stu |= (1U << 1); //heater on only
    *hardware_stu &= ~(1U << 0); //fan off only
    *hardware_stu &= ~mask_error_msg; //
    *hardware_stu |= ((uint8_t)5U << 4); //error msg 5 => 0101 0000
    uint8_t extract = (*hardware_stu >> 4) & 0x0FU; 
}

//ex5 

//wrong because bit overflow, since 16 bit + 16 bit but result is in 32 bit
void ex5(void){
    uint16_t pulse_a = 5000;
    uint16_t pulse_b = 5000;
    uint32_t total_pulses = (uint32_t)pulse_a + (uint32_t)pulse_b;
}