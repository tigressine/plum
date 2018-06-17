#include <limits.h>

#define OPERATION_FAILURE INT_MIN
#define OPERATION_SUCCESS INT_MIN + 1
#define MAX_ERROR_LENGTH 80

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
    ERROR_UNKNOWN_ARGUMENTS
};

enum Options {
    OPTION_TRACE_CPU,
    OPTION_TRACE_RECORDS,
    OPTION_TRACE_REGISTERS,
    OPTION_SKIP_ERRORS,
    OPTION_PRINT_SOURCE,
    OPTION_PRINT_LEXEME_TABLE,
    OPTION_PRINT_LEXEME_LIST
};

enum Modes {
    MODE_RUN,
    MODE_SCAN,
    MODE_COMPILE,
    MODE_EXECUTE
};

// Printer prototypes.
void printError(int, ...);
//void printErrorWithArgument(int, char*);
//void printErrorWithArguments(int, char*, char*);
