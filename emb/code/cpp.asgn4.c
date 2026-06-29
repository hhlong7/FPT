#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

//ex 1
typedef struct {
    float x;
    float y;
    float z;
    float e;
    float feedrate;
    float accel;
} kinematics;

void calc_kinematics(const kinematics* n) {
    if (n == NULL) {return;}
    //do work
}
// them tu kboa const vi function chi can read only not write on to the values of the struct


//ex 2

//reentrant la gi: 1 ham khi no co the bi interrupt khi dang chay
//va 1 task khac goi lai ham do thi moi thu van tra ve ket qua dung dan

//2
char* format_timestamp1(uint32_t seconds) {
    static char time_buf[20]; // Dùng static để chuỗi không biến mất khi thoát hàm (Tránh Dangling Pointer)
    sprintf(time_buf, "Time: %lu", seconds);
    return time_buf;
}

//tai vi xai chung 1 buffer
//nen neu khi bi ngat trong khi dang chay format_timestamp()
//ghi buffer se bi ghi de len khi goi lai ham nay sau khi bi ngat
//vi du goi format_timestamp(1000) -> buffer = "Time: 1000"
//neu nhu bi ngat va sau do goi lai format_timestamp(2000) -> buffer = "Time: 2000"
//thi sprintf co the ghi de len buffer va khi ham format_timestamp(1000) ket thuc thi buffer se la "Time: 2000" khong phai "Time: 1000" nhu mong muon

void format_timestamp2(uint32_t seconds, char* buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {return;}
    snprintf(buffer, buffer_size, "Time: %lu", (unsigned long)seconds);
}

void task(void) {
    char task_buffer[20];
    format_timestamp2(1000U, task_buffer, sizeof(task_buffer));
    log_write(task_buffer);
}

void ISR_timer(void) {
    char isr_buffer[20];
    format_timestamp2(2000U, isr_buffer, sizeof(isr_buffer));
    log_write(isr_buffer);
}
//ko bi ghi de len buffer cua task

//ex3

//beacuse when call to draw_menu(), it pushed a new stack frame with the ra, ptr of current node, loc vars, and saved regs
//so when called 50 times, it will result in stack overflow

typedef struct MenuNode {
    struct MenuNode *parent;
    struct MenuNode *child[10];
    const char* name;
    uint8_t num_child;
} MenuNode_t;

void run_menu(MenuNode_t* root) {
    MenuNode_t* current = root; 

    while (current != NULL) {
        display_menu(current);

        uint8_t btn = wait_for_button();

        if (btn == 1U) {
            if (current->parent != NULL) {
                current = current->parent;
            }
        }
        else if (btn == 2U) {
            uint8_t sel = get_selected_index();
            if (sel < current->num_child) {
                current = current->child[sel];
            }
        }
        else if (btn == 3U) {
            break;
        }
    }
}

//ex4

typedef void (*TaskFunction_t)(void*);

typedef struct {
    uint32_t interval; //how often
    uint32_t last_run; //last exec
    TaskFunction_t execute; //ptr to task func
} Task_t;

void taskA(void) {
    //do work every 10ms
}

void taskB(void) {
    //do worke  eveyr 50ms
}

static Task_t task_scheduler[2] = {
    {.interval = 10U, .last_run = 0U, .execute = taskA},
    {.interval = 50U, .last_run = 0U, .execute = taskB}
};

void main(void) {
    while (1) {
        uint32_t current_time = get_system_time();
        for (uint8_t i = 0; i < 2U; i++) {
            if ((current_time - task_scheduler[i].last_run) >= task_scheduler[i].interval) {
                task_scheduler[i].last_run = current_time;
                if (task_scheduler[i].execute != NULL) {
                    task_scheduler[i].execute();
                }
            }
        }
    }
}

//ex5

inline void toggle_pin(volatile uint32_t* port, uint8_t pin) {
    *port ^= (1U << pin);
}

//khac biet so voi macro define la compiler se check type
//debug duoc, hop cho misra hon