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
        "use = instead of :=",
        "symbol expected: %s",
        "%s must be followed by %s",
        "statement expected",
        "missing bisecting semicolon between statements",
        "identifier undeclared: %s",
        "illegal assignment to constant or procedure",
        "call of constant or variable is meaningless",
        "expression must not contain a procedure identifier",
        "%s cannot begin with %s",
        "number too large: %s",
        "file not found: %s",
        "unknown character: %s",
        "argument missing for flag: %s",
        "unknown arguments passed to program",
        "bad mode specified: %s",
        "no mode specified",
        "input file required as second argument"
    };

    // Initialize the variadic argument list, starting after errorCode.
    va_start(arguments, errorCode);

    switch (errorCode) {
        // Errors that must be formatted first.
        case ERROR_BAD_MODE:
        case ERROR_FILE_NOT_FOUND:
        case ERROR_SYMBOL_EXPECTED:
        case ERROR_NUMBER_TOO_LARGE:
        case ERROR_MISSING_ARGUMENT:
        case ERROR_UNKNOWN_CHARACTER:
        case ERROR_ILLEGAL_TOKEN_START:
        case ERROR_ILLEGAL_FOLLOW_TOKEN:
        case ERROR_UNDECLARED_IDENTIFIER:
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

/*
// Call printFile for a source file.
void printSource(char *filename) {
    printFile(filename, "Source Program:\n---------------\n");
}

// Call printFile for a lexeme file.
void printLexemeList(char *filename) {
    printFile(filename, "Lexeme List:\n------------\n");
}

// Print the contents of the file, as well as a header label.
void printFile(char *filename, char *header) {
    FILE *f;
    char buffer;

    if ((f = fopen(filename, "r")) == NULL) {
        return;
    }

    // Print every character in the file.
    printf("%s", header);
    while(fscanf(f, "%c", &buffer) != EOF) {
        printf("%c", buffer);
    }

    // Don't leave your files open like a monkey.
    fclose(f);
}

// Standardizing function to print line in lexeme table.
void printLexemeTableLine(char *lexeme, int lexemeValue) {
    printf("%12s | %d\n", lexeme, lexemeValue);
}

// Print entire lexeme table from lexeme file.
void printLexemeTable(char *filename) {
    FILE *f;
    int buffer;
    char word[IDENTIFIER_LEN + 1];

    // These lexemes directly map to the LexemeValues enumeration
    // defined in the analyzer header (with an offset of four).
    char *lexemes[] = {
        "+", "-", "*", "/", "odd", "=", "<>", "<",
        "<=", ">", ">=", "(", ")", ",", ";", ".", ":=",
        "begin", "end", "if", "then", "while", "do",
        "call", "const", "var", "procedure",
        "write", "read", "else",
    };

    if ((f = fopen(filename, "r")) == NULL) {
        return;
    }

    // Print some custom header stuff for the table.
    printf("     :Lexeme | Value:\n");
    printf("     ----------------\n");

    // For each integer in the file, print the relevant symbol and lexeme value.
    while(fscanf(f, " %d", &buffer) != EOF) {
        // If the buffer is for an identifier or number, read the next
        // string in the file and then print.
        if (buffer == IDENTIFIER || buffer == NUMBER) {
            fscanf(f, MAX_TOKEN_FORMAT, word);
            printLexemeTableLine(word, buffer);
        }
        // Else print the lexeme from the lexemes array, with an offset of
        // four to account for the first four values in the enumeration not
        // being included.
        else {
            printLexemeTableLine(lexemes[buffer - 4], buffer);
        }
    }

    fclose(f);
}
*/
