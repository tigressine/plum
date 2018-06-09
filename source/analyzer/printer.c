#include <stdio.h>
#include "analyzer.h"

void printSource(char *filename) {
    printFile(filename, "Source program:\n");
}

void printLexemeTableLine(char *lexeme, int lexemeValue) {
    printf("%s %d\n", lexeme, lexemeValue);
}

void printLexemeTable(char *filename) {
    FILE *f;
    int buffer;
    char word[IDENTIFIER_LEN + 1];

    if ((f = fopen(filename, "r")) == NULL) {
        return;
    }

    printf("Lexeme table:\n");
    printf("lexeme:      token type:\n");
    printf("------------------------\n");
    while(fscanf(f, " %d", &buffer) != EOF) {
        switch(buffer) {
            case IDENTIFIER:
                fscanf(f, "%s", word);
                printLexemeTableLine(word, IDENTIFIER);
                break;

            case NUMBER:
                fscanf(f, "%s", word);
                printLexemeTableLine(word, NUMBER);
                break;

            case PLUS: printLexemeTableLine("+", PLUS); break;
            case MINUS: printLexemeTableLine("-", MINUS); break;
            case MULTIPLY: printLexemeTableLine("*", MULTIPLY); break;
            case SLASH: printLexemeTableLine("/", SLASH); break;
            case ODD: printLexemeTableLine("odd", ODD); break;
            case EQUAL: printLexemeTableLine("==", EQUAL); break;
            case NOT_EQUAL: printLexemeTableLine("!=", NOT_EQUAL); break;
            case LESS: printLexemeTableLine("<", LESS); break;
            case LESS_EQUAL: printLexemeTableLine("<=", LESS_EQUAL); break;
            case GREATER: printLexemeTableLine(">", GREATER); break;
            case GREATER_EQUAL: printLexemeTableLine(">=", GREATER_EQUAL); break;
            case LEFT_PARENTHESIS: printLexemeTableLine("(", LEFT_PARENTHESIS); break;
            case RIGHT_PARENTHESIS: printLexemeTableLine(")", RIGHT_PARENTHESIS); break;
            case COMMA: printLexemeTableLine(",", COMMA); break;
            case SEMICOLON: printLexemeTableLine(";", SEMICOLON); break;
            case PERIOD: printLexemeTableLine(".", PERIOD); break;
            case BECOME: printLexemeTableLine(":=", BECOME); break;
            case BEGIN: printLexemeTableLine("begin", BEGIN); break;
            case END: printLexemeTableLine("end", END); break;
            case IF: printLexemeTableLine("if", IF); break;
            case THEN: printLexemeTableLine("then", THEN); break;
            case WHILE: printLexemeTableLine("while", WHILE); break;
            case DO: printLexemeTableLine("do", DO); break;
            case CALL: printLexemeTableLine("call", CALL); break;
            case CONSTANT: printLexemeTableLine("const", CONSTANT); break;
            case VARIABLE: printLexemeTableLine("var", VARIABLE); break;
            case PROCEDURE: printLexemeTableLine("procedure", PROCEDURE); break;
            case WRITE: printLexemeTableLine("write", WRITE); break;
            case READ: printLexemeTableLine("read", READ); break;
            case ELSE: printLexemeTableLine("else", ELSE); break;
        } 
    }

    fclose(f);
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
