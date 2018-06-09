#include <stdio.h>
#include "analyzer.h"

void printSource(char *filename) {
    printFile(filename, "Source program:\n");
}

void printLexemeTable(char *filename) {
}

void printLexemeList(char *filename) {
    printFile(filename, "Lexeme list:\n");
}

void printFile(char *filename, char *header) {
    FILE *f;
    char buffer;

    // If file cannot be opened, do nothing.
    if ((f = fopen(filename, "r")) == NULL) {
        return;
    }

    // Print every character in the file.
    printf("%s", header);
    while(fscanf(f, "%c", &buffer) != EOF) {
        printf("%c", buffer);
    }
    printf("\n");

    // Don't leave your files open like a monkey.
    fclose(f);
}
