// Part of Plum by Tiger Sachse.
#ifndef PLUM_H
#define PLUM_H

#include <limits.h>

#define INT_OFFSET 4
#define IDENTIFIER_LEN 11
#define REGISTER_COUNT 16
#define MAX_ERROR_LENGTH 50
#define INTERMEDIATE_FILE "plum.tmp"
#define DEFAULT_OUTPUT_FILE "plum.out"

// Operation codes for each assembly instruction.
enum Opcodes {
    LIT = 1,
    RTN, LOD, STO,
    CAL, INC, JMP,
    JPC, SIO, NEG,
    ADD, SUB, MUL,
    DIV, ODD, MOD,
    EQL, NEQ, LSS,
    LEQ, GTR, GEQ
};

// Enumeration of lexeme values.
enum LexemeValues {
    LEX_UNKNOWN,
    LEX_NULL,
    LEX_IDENTIFIER,
    LEX_NUMBER,
    LEX_PLUS,
    LEX_MINUS,
    LEX_MULTIPLY,
    LEX_SLASH,
    LEX_ODD,
    LEX_EQUAL,
    LEX_NOT_EQUAL,
    LEX_LESS,
    LEX_LESS_EQUAL,
    LEX_GREATER,
    LEX_GREATER_EQUAL,
    LEX_LEFT_PARENTHESIS,
    LEX_RIGHT_PARENTHESIS,
    LEX_COMMA,
    LEX_SEMICOLON,
    LEX_PERIOD,
    LEX_BECOME,
    LEX_BEGIN,
    LEX_END,
    LEX_IF,
    LEX_THEN,
    LEX_WHILE,
    LEX_DO,
    LEX_CALL,
    LEX_CONST,
    LEX_VAR,
    LEX_PROCEDURE,
    LEX_WRITE,
    LEX_READ,
    LEX_ELSE,
    LEX_COMMENT
} LexemeValues;

// Function return signals.
enum Signals {
    SIGNAL_FAILURE = INT_MIN,
    SIGNAL_SUCCESS,
    SIGNAL_RECOVERY,
    SIGNAL_KILL,
    SIGNAL_EOF,
    SIGNAL_FALSE = 0,
    SIGNAL_TRUE = 1
};

// The full collection of errors that this program can throw.
enum Errors {

    // Formatting errors.
    ERROR_SYMBOL_EXPECTED_STRING,
    ERROR_SYMBOL_EXPECTED_CHAR,
    ERROR_UNKNOWN_CHARACTER,
    ERROR_ILLEGAL_FACTOR_SYNTAX,
    ERROR_CHARACTERS_AFTER_PERIOD,
    ERROR_NO_VALUE_FOR_TOKEN,
    ERROR_ILLEGAL_COMPARATOR,
    ERROR_ILLEGAL_LEXEME_FORMAT,

    // Number and identifier errors.
    ERROR_NUMBER_EXPECTED,
    ERROR_IDENTIFIER_EXPECTED,
    ERROR_NUMBER_TOO_LARGE,
    ERROR_ILLEGAL_IDENTIFIER,
    ERROR_UNDECLARED_IDENTIFIER,
    ERROR_IDENTIFIER_TOO_LARGE,
    ERROR_IDENTIFIER_ALREADY_DECLARED,
    ERROR_ASSIGNMENT_TO_CONSTANT,

    // User IO errors.
    ERROR_NO_MODE,
    ERROR_BAD_MODE,
    ERROR_ARGUMENT_MISSING,
    ERROR_INPUT_FILE_MISSING,

    // Memory and bounds errors.
    ERROR_OUT_OF_MEMORY,
    ERROR_NO_STATIC_PARENT,
    ERROR_NO_DYNAMIC_PARENT,
    ERROR_NULL_POINTER,
    ERROR_REGISTER_OUT_OF_BOUNDS,
    ERROR_LOCAL_INDEX_OUT_OF_BOUNDS,
    ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS,

    // File IO errors.
    ERROR_FILE_TOO_LONG,
    ERROR_FILE_NOT_FOUND,
    ERROR_WRITING_FILE_FAILED,
    ERROR_UNEXPECTED_END_OF_FILE,

    // Assembly operation errors.
    ERROR_ILLEGAL_SYSTEM_CALL,
    ERROR_ILLEGAL_OP_CODE,
    ERROR_DIVIDE_BY_ZERO
};

// Available system calls.
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
    OPTION_PRINT_LEXEME_LIST,
    OPTION_PRINT_SYMBOL_TABLE,
    OPTION_PRINT_ASSEMBLY
};

// Different modes for the machine.
enum Modes {
    MODE_RUN,
    MODE_SCAN,
    MODE_PARSE,
    MODE_COMPILE,
    MODE_EXECUTE
};

// Instruction struct for each line of PL/0 code.
typedef struct Instruction {
    int opCode;
    int RField;
    int LField;
    int MField;
} Instruction;

// Utilities functional prototypes.
void setOption(int*, int);
int checkOption(int*, int);
int fileExists(char*);
int isAlphanumeric(char);
int isAlphabetic(char);
int isDigit(char);
int isWhitespace(char);
void setInstruction(Instruction*, int, int, int, int);

// Printer functional prototypes.
void printError(int, ...);
void printAssembly(char*);
void printFile(char*, char*);

#endif
