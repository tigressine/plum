#include <limits.h>
#include <stdio.h>

// Constants.
#define DEFAULT_OUTFILE "output.pll"
#define OP_SUCCESS INT_MIN + 1
#define OP_FAILURE INT_MIN
#define IDENTIFIER_LEN 11
#define NUMBER_LEN 5
#define KEYWORDS 13

// Options defined as bit flags.
#define OPTION_FAIL_ON_ERROR 1

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
    char follow;
    int soloValue;
    int pairValue;
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
int skipComment(FILE*);
void errorUnknownCharacter(char);
void errorTokenTooLong(char*, int);
void errorBadIdentifier(char*);

// Handler functional prototypes.
int isAlphanumeric(char);
int isAlphabetic(char);
int isDigit(char);
void eatCharacters(FILE*, int);
int checkKeywords(FILE*, char*);
int handleDirectMappedSymbol(FILE*, int);
int handlePair(FILE*, FILE*, SymbolSymbolPair);
int handleLongToken(FILE*, FILE*, char, int, int);

// Printer functional prototypes.
void printSource(char*);
void printLexemeList(char*);
void printFile(char*, char*);
void printLexemeTableLine(const char*, int);
void printLexemeTable(char*);
