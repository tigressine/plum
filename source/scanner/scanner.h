#include "../plum.h"
#include <limits.h>
#include <stdio.h>

// Constants.
#define DEFAULT_OUTFILE "output.pll"
// MAX_TOKEN_FORMAT prevents stack smashing if
// the lexeme file contains tokens that are too long. It's
// value relies on IDENTIFIER_LEN. The smashing happens
// because these long tokens overflow a buffer in printLexemeTable.
// I'd rather not make the buffer bigger, so I restrict the input
// instead. It is obviously not an ideal solution.
#define MAX_TOKEN_FORMAT "%11s"
#define OP_SUCCESS INT_MIN + 1
#define OP_FAILURE INT_MIN
#define IDENTIFIER_LEN 11
#define NUMBER_LEN 5
#define KEYWORDS 13

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
    MULTIPLY, SLASH, ODD,
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
int analyzeSource(char*, char*, int);

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
void errorMissingFile(char*);
void errorUnknownCharacter(char);
void errorTokenTooLong(char*, int);
void errorBadIdentifier(char*);
void errorNoSource(void);
void errorNoArgument(char*);
void errorUnknownArguments(void);
void printSource(char*);
void printLexemeList(char*);
void printFile(char*, char*);
void printLexemeTableLine(char*, int);
void printLexemeTable(char*);
