// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include "scanner.h"

// Call printFile for a source file.
void printSource(char *filename) {
    printFile(filename, "Source Program:\n---------------\n");
    printf("\n");
}

// Call printFile for a lexeme file.
void printLexemeList(char *filename) {
    printFile(filename, "Lexeme List:\n------------\n");
    printf("\n\n");
}

// Standardizing function to print line in lexeme table.
void printLexemeTableLine(char *lexeme, int lexemeValue) {
    printf("%12s | %d\n", lexeme, lexemeValue); // magic number
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
        printError(ERROR_FILE_NOT_FOUND, filename);
        
        return;
    }

    // Print some custom header stuff for the table.
    printf("     :Lexeme | Value:\n");
    printf("     ----------------\n");

    // For each integer in the file, print the relevant symbol and lexeme value.
    while (fscanf(f, "%d", &buffer) != EOF) {

        // If the buffer is for an identifier or number, read the next
        // string in the file and then print.
        if (buffer == LEX_IDENTIFIER || buffer == LEX_NUMBER) {
            fscanf(f, "%12s", word); // magic number
            printLexemeTableLine(word, buffer);
        }

        // Else print the lexeme from the lexemes array, with an offset of
        // four to account for the first four values in the enumeration not
        // being included.
        else {
            printLexemeTableLine(lexemes[buffer - 4], buffer); // magic number
        }
    }
    printf("\n");

    fclose(f);
}
