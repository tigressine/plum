#include <stdio.h>
#include <string.h>
#include "plum.h"

// Wrapper to print an error that requires no string arguments.
void printError(int errorCode) {
    printErrorWithArguments(errorCode, NULL, NULL);
}

// Wrapper to print an error that needs only one string argument.
void printErrorWithArgument(int errorCode, char *string1) {
    printErrorWithArguments(errorCode, string1, NULL);
}

// Print error associated with provided errorCode.
void printErrorWithArguments(int errorCode, char *string1, char *string2) {
    char error[MAX_ERROR_LENGTH];

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
        "unknown arguments passed to program"
    };

    switch (errorCode) {
        // Errors that call for one string argument.
        case ERROR_FILE_NOT_FOUND:
        case ERROR_SYMBOL_EXPECTED:
        case ERROR_NUMBER_TOO_LARGE:
        case ERROR_MISSING_ARGUMENT:
        case ERROR_UNKNOWN_CHARACTER:
        case ERROR_UNDECLARED_IDENTIFIER:
            snprintf(error, MAX_ERROR_LENGTH, errors[errorCode], string1);
            break;

        // Errors that call for two string arguments.
        case ERROR_ILLEGAL_TOKEN_START:
        case ERROR_ILLEGAL_FOLLOW_TOKEN:
            snprintf(error, MAX_ERROR_LENGTH, errors[errorCode], string1, string2);
            break;

        // Errors with no string arguments.
        default:
            strcpy(error, errors[errorCode]);
    }

    // Print the error buffer after formatting.
    printf("ERROR %s\n", error);
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
