// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include "../plum.h"

// Constants.
#define KEYWORDS 13
#define NUMBER_LEN 5

// Map keywords and their values.
typedef struct KeywordValuePair {
    char *keyword;
    int value;
} KeywordValuePair;

// Map symbols and their values.
typedef struct SymbolValuePair {
    char symbol;
    int value;
} SymbolValuePair;

// Map symbol pairs and their values.
typedef struct SymbolSymbolPair {
    char lead;
    char follows[2];
    int soloValue;
    int pairValues[2];
    int pairs;
} SymbolSymbolPair;

// Enumeration of lexeme values.
enum LexemeValues {
    UNKNOWN, NULL_VAL, IDENTIFIER,
    NUMBER, PLUS, MINUS,
    MULTIPLY, SLASH, ODD_KEY,
    EQUAL, NOT_EQUAL, LESS,
    LESS_EQUAL, GREATER,
    GREATER_EQUAL, LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS, COMMA,
    SEMICOLON, PERIOD, BECOME,
    BEGIN, END, IF, THEN, WHILE,
    DO, CALL, CONST, VAR,
    PROCEDURE, WRITE, READ, ELSE, COMMENT
} LexemeValues;

// Core functional prototypes.
int scanSource(char*, char*, int);

// Handler functional prototypes.
int isAlphanumeric(char);
int isAlphabetic(char);
int isDigit(char);
int skipComment(FILE*);
void eatCharacters(FILE*, int);
int checkKeywords(FILE*, char*);
int handleDirectMappedSymbol(FILE*, int);
int handlePair(FILE*, FILE*, SymbolSymbolPair);
int handleLongToken(FILE*, FILE*, char, int, int);

// Printer functional prototypes.
void printSource(char*);
void printLexemeList(char*);
void printFile(char*, char*);
void printLexemeTableLine(char*, int);
void printLexemeTable(char*);
