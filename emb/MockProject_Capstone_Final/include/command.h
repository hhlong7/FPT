/**
 * @file command.h
 * @brief Command queue interface - Topic 9 (Data Structures) - Ring Buffer
 */
#ifndef COMMAND_H
#define COMMAND_H

#include "types.h"
#include "config.h"

/* Command types */
typedef enum {
    CMD_NOP = 0U,
    CMD_MOVE_X = 1U,
    CMD_MOVE_Y = 2U,
    CMD_MOVE_XY = 3U,
    CMD_SET_SPEED = 4U,
    CMD_SET_TEMP = 5U,
    CMD_GET_STATUS = 6U,
    CMD_RESET = 7U,
    CMD_SAVE_CONFIG = 8U,
    CMD_LOAD_CONFIG = 9U,
    CMD_FAULT = 0xFF
} CommandType_t;

/* Command priority */
typedef enum {
    CMD_PRIORITY_LOW = 0U,
    CMD_PRIORITY_NORMAL = 1U,
    CMD_PRIORITY_HIGH = 2U,
    CMD_PRIORITY_CRITICAL = 3U
} CommandPriority_t;

/* Command structure - Topic 3 (Arrays, Pointers) */
typedef struct {
    CommandType_t type;
    CommandPriority_t priority;
    uint8_t length;
    uint8_t checksum;
    uint8_t data[12];
    uint32_t timestamp_ms;
} Command_t;

/* Command parser states */
typedef enum {
    CMD_PARSER_WAIT_START = 0U,
    CMD_PARSER_READ_TYPE = 1U,
    CMD_PARSER_READ_LENGTH = 2U,
    CMD_PARSER_READ_DATA = 3U,
    CMD_PARSER_READ_CHECKSUM = 4U,
    CMD_PARSER_COMPLETE = 5U
} CommandParserState_t;

/* Parser context */
typedef struct {
    CommandParserState_t state;
    uint8_t index;
    uint8_t length;
    uint8_t checksum;
    Command_t current_cmd;
    uint8_t rx_buffer[32];
} CommandParser_t;

/* ==================== Command Queue API ==================== */
/* Ring buffer implementation - Topic 9 */
void Command_Init(void);
Result_t Command_Enqueue(const Command_t* cmd);
Result_t Command_Dequeue(Command_t* cmd);
bool Command_IsEmpty(void);
bool Command_IsFull(void);
uint8_t Command_GetCount(void);

/* ==================== Command Parser API ==================== */
/* UART command parser - Topic 3 (Pointer arithmetic) */
void CommandParser_Init(CommandParser_t* parser);
Result_t CommandParser_ProcessByte(CommandParser_t* parser, uint8_t byte);
Result_t CommandParser_GetCommand(CommandParser_t* parser, Command_t* cmd);

/* ==================== Command Handlers ==================== */
Result_t CommandHandler_Execute(const Command_t* cmd);
Result_t CommandHandler_Move(const Command_t* cmd);
Result_t CommandHandler_SetSpeed(const Command_t* cmd);
Result_t CommandHandler_GetStatus(const Command_t* cmd);
Result_t CommandHandler_Reset(const Command_t* cmd);

#endif /* COMMAND_H */