#ifndef PLUM_H
#define PLUM_H

#include <limits.h>
#include "machine/machine.h"
//#include "generator/generator.h"

#define DEFAULT_OUTPUT_FILE "plum.out"
#define MAX_ERROR_LENGTH 80
#define IDENTIFIER_LEN 11

enum Status {
    STATUS_ACTIVE,
    STATUS_INACTIVE
};

enum Signals {
    SIGNAL_FAILURE = INT_MIN,
    SIGNAL_SUCCESS,
    SIGNAL_RECOVERY,
    SIGNAL_KILL,
    SIGNAL_FALSE = 0,
    SIGNAL_TRUE = 1
};

// The full collection of errors that this program can throw.
enum Errors {
    ERROR_BECOME_INSTEAD_OF_EQUAL,
    ERROR_SYMBOL_EXPECTED,
    ERROR_ILLEGAL_FOLLOW_TOKEN,
    ERROR_STATEMENT_EXPECTED,
    ERROR_BISECTING_SEMICOLON_MISSING,
    ERROR_UNDECLARED_IDENTIFIER,
    ERROR_ILLEGAL_ASSIGNMENT,
    ERROR_MEANINGLESS_CALL,
    ERROR_PROCEDURE_IDENTIFIER_IN_EXPRESSION,
    ERROR_ILLEGAL_TOKEN_START,
    ERROR_NUMBER_TOO_LARGE,
    ERROR_FILE_NOT_FOUND,
    ERROR_UNKNOWN_CHARACTER,
    ERROR_MISSING_ARGUMENT,
    ERROR_UNKNOWN_ARGUMENTS,
    ERROR_BAD_MODE,
    ERROR_NO_MODE,
    ERROR_NO_SOURCE,
    ERROR_NULL_CHECK,
    ERROR_OUT_OF_MEMORY,
    ERROR_INVALID_REGISTER,
    ERROR_INVALID_STATIC_PARENT,
    ERROR_LOCAL_OUT_OF_BOUNDS,
    ERROR_ILLEGAL_SYSTEM_CALL,
    ERROR_DIVIDE_BY_ZERO,
    ERROR_FILE_TOO_LONG,
    ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS,
    ERROR_ILLEGAL_OPCODE
};

enum SystemCalls {
    CALL_PRINT = 1,
    CALL_SCAN,
    CALL_KILL
};

// All possible options for processing files.
enum Options {
    OPTION_TRACE_CPU,
    OPTION_TRACE_RECORDS,
    OPTION_TRACE_REGISTERS,
    OPTION_SKIP_ERRORS,
    OPTION_PRINT_SOURCE,
    OPTION_PRINT_LEXEME_TABLE,
    OPTION_PRINT_LEXEME_LIST
};

// Different modes for the machine.
enum Modes {
    MODE_RUN,
    MODE_SCAN,
    MODE_COMPILE,
    MODE_EXECUTE
};

// Printer prototypes.
void printError(int, ...);
void printStackTraceHeader(int);
void printStackTraceLine(CPU*, recordStack*, int);
void printRegisters(CPU*);
void printCPU(CPU*);
void printRecords(recordStackItem*);

// Utilities prototypes.
void setOption(int*, int);
int checkOption(int*, int);
int fileExists(char*);

#endif
