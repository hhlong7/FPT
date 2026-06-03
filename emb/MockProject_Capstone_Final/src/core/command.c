/**
 * @file command.c
 * @brief Command Queue implementation - Topic 9 (Data Structures)
 * @note Ring buffer with priority support (no heap allocation)
 */
#include "types.h"
#include "config.h"
#include "command.h"
#include "ringbuffer.h"

/* ==================== Command Queue (Ring Buffer) ==================== */
#define CMD_QUEUE_SIZE  COMMAND_QUEUE_SIZE  /* Must be power of 2 */

typedef struct {
    Command_t commands[CMD_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    volatile uint8_t count;
} CommandQueue_t;

/* Static allocation - Topic 2 (no heap) */
static CommandQueue_t s_cmd_queue;

/* Static assert - Queue size must be power of 2 */
_Static_assert((CMD_QUEUE_SIZE & (CMD_QUEUE_SIZE - 1U)) == 0U,
                "CMD_QUEUE_SIZE must be power of 2");

/* ==================== Internal Functions ==================== */

static inline uint8_t NextIndex(uint8_t idx) {
    return (idx + 1U) & (CMD_QUEUE_SIZE - 1U);
}

/* ==================== Command Queue API ==================== */

void Command_Init(void) {
    s_cmd_queue.head = 0U;
    s_cmd_queue.tail = 0U;
    s_cmd_queue.count = 0U;
}

Result_t Command_Enqueue(const Command_t* cmd) {
    if (cmd == NULL_PTR) {
        return ERROR_INVALID_PARAM;
    }

    if (s_cmd_queue.count >= CMD_QUEUE_SIZE) {
        return ERROR_OVERFLOW;  /* Queue full */
    }

    /* Copy command to queue */
    s_cmd_queue.commands[s_cmd_queue.head] = *cmd;
    s_cmd_queue.head = NextIndex(s_cmd_queue.head);
    s_cmd_queue.count++;

    return OK;
}

Result_t Command_Dequeue(Command_t* cmd) {
    if (cmd == NULL_PTR) {
        return ERROR_INVALID_PARAM;
    }

    if (s_cmd_queue.count == 0U) {
        return ERROR;  /* Queue empty */
    }

    /* Copy command from queue */
    *cmd = s_cmd_queue.commands[s_cmd_queue.tail];
    s_cmd_queue.tail = NextIndex(s_cmd_queue.tail);
    s_cmd_queue.count--;

    return OK;
}

bool Command_IsEmpty(void) {
    return (s_cmd_queue.count == 0U);
}

bool Command_IsFull(void) {
    return (s_cmd_queue.count >= CMD_QUEUE_SIZE);
}

uint8_t Command_GetCount(void) {
    return s_cmd_queue.count;
}

/* ==================== Command Parser (State Machine) - Topic 13 ==================== */

void CommandParser_Init(CommandParser_t* parser) {
    if (parser == NULL_PTR) {
        return;
    }

    parser->state = CMD_PARSER_WAIT_START;
    parser->index = 0U;
    parser->length = 0U;
    parser->checksum = 0U;
}

static uint8_t CalculateChecksum(const Command_t* cmd) {
    uint8_t sum = cmd->type + cmd->priority + cmd->length;
    uint8_t i;
    for (i = 0U; i < cmd->length; i++) {
        sum += cmd->data[i];
    }
    return sum;
}

Result_t CommandParser_ProcessByte(CommandParser_t* parser, uint8_t byte) {
    if (parser == NULL_PTR) {
        return ERROR_INVALID_PARAM;
    }

    switch (parser->state) {
        case CMD_PARSER_WAIT_START:
            if (byte == 0xAAU) {  /* Start byte */
                parser->state = CMD_PARSER_READ_TYPE;
                parser->checksum = 0U;
            }
            break;

        case CMD_PARSER_READ_TYPE:
            parser->current_cmd.type = (CommandType_t)byte;
            parser->state = CMD_PARSER_READ_LENGTH;
            break;

        case CMD_PARSER_READ_LENGTH:
            parser->current_cmd.length = byte;
            parser->index = 0U;
            parser->state = CMD_PARSER_READ_DATA;
            break;

        case CMD_PARSER_READ_DATA:
            parser->current_cmd.data[parser->index++] = byte;
            if (parser->index >= parser->current_cmd.length) {
                parser->state = CMD_PARSER_READ_CHECKSUM;
            }
            break;

        case CMD_PARSER_READ_CHECKSUM:
            parser->current_cmd.checksum = byte;
            parser->state = CMD_PARSER_COMPLETE;
            break;

        case CMD_PARSER_COMPLETE:
        default:
            parser->state = CMD_PARSER_WAIT_START;
            break;
    }

    return OK;
}

Result_t CommandParser_GetCommand(CommandParser_t* parser, Command_t* cmd) {
    if (parser == NULL_PTR || cmd == NULL_PTR) {
        return ERROR_INVALID_PARAM;
    }

    if (parser->state != CMD_PARSER_COMPLETE) {
        return ERROR;  /* No complete command */
    }

    /* Validate checksum */
    uint8_t expected_checksum = CalculateChecksum(&parser->current_cmd);
    if (expected_checksum != parser->current_cmd.checksum) {
        parser->state = CMD_PARSER_WAIT_START;
        return ERROR;  /* Checksum mismatch */
    }

    *cmd = parser->current_cmd;
    parser->state = CMD_PARSER_WAIT_START;

    return OK;
}

/* ==================== Command Handlers ==================== */

Result_t CommandHandler_Execute(const Command_t* cmd) {
    if (cmd == NULL_PTR) {
        return ERROR_INVALID_PARAM;
    }

    switch (cmd->type) {
        case CMD_MOVE_X:
        case CMD_MOVE_Y:
        case CMD_MOVE_XY:
            return CommandHandler_Move(cmd);

        case CMD_SET_SPEED:
            return CommandHandler_SetSpeed(cmd);

        case CMD_GET_STATUS:
            return CommandHandler_GetStatus(cmd);

        case CMD_RESET:
            return CommandHandler_Reset(cmd);

        default:
            return ERROR_INVALID_PARAM;
    }
}

Result_t CommandHandler_Move(const Command_t* cmd) {
    (void)cmd;
    /* Implementation would call Motor_MoveAbsolute or Motor_MoveRelative */
    return OK;
}

Result_t CommandHandler_SetSpeed(const Command_t* cmd) {
    (void)cmd;
    /* Implementation would update motor speed */
    return OK;
}

Result_t CommandHandler_GetStatus(const Command_t* cmd) {
    (void)cmd;
    /* Implementation would send status via UART */
    return OK;
}

Result_t CommandHandler_Reset(const Command_t* cmd) {
    (void)cmd;
    /* Trigger system reset */
    HAL_SystemReset();
    return OK;  /* Never reached */
}