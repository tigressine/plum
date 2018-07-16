// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "plum.h"

// Print error associated with provided errorCode.
void printError(int errorCode, ...) {
    va_list arguments;
    char error[MAX_ERROR_LENGTH];

    // All errors in this array map to the appropriate code in
    // an enumeration defined in the plum.h header.
    char errors[][MAX_ERROR_LENGTH] = {

        // Formatting errors.
        "symbol expected: %s",
        "symbol expected: %c",
        "unknown character: %c",
        "illegal factor syntax",
        "characters after period",
        "missing value for token: %d",
        "illegal comparator token: %d",
        "illegal format of lexemes: %d",
       
        // Number and identifier errors.
        "number expected",
        "identifier expected",
        "number too large: %s...",
        "illegal identifier: %s...",
        "identifier undeclared: %s",
        "identifier is too long: %s...",
        "identifier already declared: %s",
        "illegal assignment to constant: %s",
        
        // User IO errors.
        "no mode specified",
        "bad mode specified: %s",
        "argument missing for flag: %s",
        "input file required as second argument",
       
        // Memory and bounds errors.
        "program ran out of memory",
        "static parent does not exist",
        "dynamic parent does not exist",
        "attempted to dereference null",
        "register is out of bounds: %d",
        "local index is out of bounds: %d",
        "program counter out of bounds: %d",
      
        // File IO errors.
        "file too long: %s",
        "file not found: %s",
        "error writing to file",
        "unexpected end of file",
       
        // Assembly operation errors.
        "illegal system call: %d",
        "illegal operation code: %d",
        "attempted to divide by zero",
    };

    // Initialize the variadic argument list, starting after errorCode.
    va_start(arguments, errorCode);

    switch (errorCode) {
        
        // Errors that must be formatted first. Calls a safe version of sprintf
        // that also handles variadic lists.
        case ERROR_SYMBOL_EXPECTED_STRING:
        case ERROR_SYMBOL_EXPECTED_CHAR:
        case ERROR_UNKNOWN_CHARACTER:
        case ERROR_NO_VALUE_FOR_TOKEN:
        case ERROR_ILLEGAL_COMPARATOR:
        case ERROR_ILLEGAL_LEXEME_FORMAT:
        case ERROR_NUMBER_TOO_LARGE:
        case ERROR_ILLEGAL_IDENTIFIER:
        case ERROR_UNDECLARED_IDENTIFIER:
        case ERROR_IDENTIFIER_TOO_LARGE:
        case ERROR_IDENTIFIER_ALREADY_DECLARED:
        case ERROR_ASSIGNMENT_TO_CONSTANT:
        case ERROR_BAD_MODE:
        case ERROR_ARGUMENT_MISSING:
        case ERROR_REGISTER_OUT_OF_BOUNDS:
        case ERROR_LOCAL_INDEX_OUT_OF_BOUNDS:
        case ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS:
        case ERROR_FILE_TOO_LONG:
        case ERROR_FILE_NOT_FOUND:
        case ERROR_ILLEGAL_SYSTEM_CALL:
        case ERROR_ILLEGAL_OP_CODE:
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

// Print the assembly file.
void printAssembly(char *assemblyFile) {
    printFile(assemblyFile, "Assembly:\n---------\n");
    printf("\n");
}

// Print the contents of the file, as well as a header label.
void printFile(char *filename, char *header) {
    FILE *f;
    char buffer;

    if ((f = fopen(filename, "r")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, filename);
        
        return;
    }

    // Print every character in the file.
    printf("%s", header);
    while (fscanf(f, "%c", &buffer) != EOF) {
        printf("%c", buffer);
    }

    // Don't leave your files open like a monkey.
    fclose(f);
}
