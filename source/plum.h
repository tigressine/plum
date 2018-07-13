// Part of Plum by Tiger Sachse.
#ifndef PLUM_H
#define PLUM_H

#include <limits.h>

#define IDENTIFIER_LEN 11
#define MAX_ERROR_LENGTH 80
#define DEFAULT_OUTPUT_FILE "plum.out"

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
    ERROR_IDENTIFIER_EXPECTED,
    ERROR_NUMBER_EXPECTED,
    ERROR_ILLEGAL_IDENTIFIER,
    ERROR_TOKEN_TOO_LONG,
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
    ERROR_INVALID_DYNAMIC_PARENT,
    ERROR_LOCAL_OUT_OF_BOUNDS,
    ERROR_ILLEGAL_SYSTEM_CALL,
    ERROR_DIVIDE_BY_ZERO,
    ERROR_FILE_TOO_LONG,
    ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS,
    ERROR_ILLEGAL_OPCODE,
    ERROR_MISSING_TOKEN,
    ERROR_ILLEGAL_LEXEME_FORMAT,
    ERROR_TRAILING_CHARACTERS
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
    OPTION_PRINT_LEXEME_LIST
};

// Different modes for the machine.
enum Modes {
    MODE_RUN,
    MODE_SCAN,
    MODE_COMPILE,
    MODE_EXECUTE
};

// Utilities functional prototypes.
void setOption(int*, int);
int checkOption(int*, int);
int fileExists(char*);
int isAlphanumeric(char);
int isAlphabetic(char);
int isDigit(char);
int isWhitespace(char);

// Printer functional prototypes.
void printError(int, ...);

#endif
