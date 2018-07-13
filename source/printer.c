// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "plum.h"

// Print error associated with provided errorCode.
void printError(int errorCode, ...) {
    char error[MAX_ERROR_LENGTH];
    va_list arguments;

    // All errors in this array map to the appropriate code in
    // an enumeration defined in the plum.h header.
    char errors[][MAX_ERROR_LENGTH] = {
        "identifier expected",
        "number expected",
        "illegal identifier: %s...",
        "token is too long: %s...",
        "use = instead of :=",
        "symbol expected: %c",
        "%s must be followed by %s",
        "statement expected",
        "missing bisecting semicolon between statements",
        "identifier undeclared: %s",
        "illegal assignment to constant or procedure",
        "call of constant or variable is meaningless",
        "expression must not contain a procedure identifier",
        "%s cannot begin with %s",
        "number too large: %s",//
        "file not found: %s",//
        "unknown character: %c",//
        "argument missing for flag: %s",//
        "unknown arguments passed to program",
        "bad mode specified: %s",//
        "no mode specified",//
        "input file required as second argument",//
        "attempted to dereference null",//
        "program ran out of memory",
        "register is out of bounds: %d",//
        "static parent does not exist",//
        "dynamic parent does not exist",//
        "local index is out of bounds: %d",//
        "illegal system call: %d",//
        "attempted to divide by zero",//
        "file too long: %s",//
        "program counter out of bounds: %d",//
        "illegal operation code: %d",//
        "missing value for token: %d",
        "illegal format of lexemes: %d",
        "characters after period"
    };

    // Initialize the variadic argument list, starting after errorCode.
    va_start(arguments, errorCode);

    switch (errorCode) {
        // Errors that must be formatted first.
        case ERROR_BAD_MODE:
        case ERROR_FILE_TOO_LONG:
        case ERROR_MISSING_TOKEN:
        case ERROR_ILLEGAL_OPCODE:
        case ERROR_TOKEN_TOO_LONG:
        case ERROR_FILE_NOT_FOUND:
        case ERROR_SYMBOL_EXPECTED:
        case ERROR_NUMBER_TOO_LARGE:
        case ERROR_MISSING_ARGUMENT:
        case ERROR_INVALID_REGISTER:
        case ERROR_UNKNOWN_CHARACTER:
        case ERROR_ILLEGAL_IDENTIFIER:
        case ERROR_ILLEGAL_TOKEN_START:
        case ERROR_LOCAL_OUT_OF_BOUNDS:
        case ERROR_ILLEGAL_SYSTEM_CALL:
        case ERROR_ILLEGAL_FOLLOW_TOKEN:
        case ERROR_UNDECLARED_IDENTIFIER:
        case ERROR_ILLEGAL_LEXEME_FORMAT:
        case ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS:
            // Call a safe version of sprintf that also handles variadic lists.
            vsnprintf(error, MAX_ERROR_LENGTH, errors[errorCode], arguments);
            break;

        // Errors that require no formatting.
        default:
            strcpy(error, errors[errorCode]);
    }

    // Print the error buffer after formatting.
    printf("ERROR %s\n", error);

    // Close the variadic argument list.
    va_end(arguments);
}
