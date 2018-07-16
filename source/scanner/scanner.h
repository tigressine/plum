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

// Core functional prototypes.
int scanSource(char*, char*, int);

// Handler functional prototypes.
int skipComment(FILE*);
void eatCharacters(FILE*, int);
int checkKeywords(FILE*, char*);
int handleDirectMappedSymbol(FILE*, int);
int handlePair(FILE*, FILE*, SymbolSymbolPair);
int handleLongToken(FILE*, FILE*, char, int, int);

// Printer functional prototypes.
void printSource(char*);
void printLexemeList(char*);
void printLexemeTableLine(char*, int);
void printLexemeTable(char*);
