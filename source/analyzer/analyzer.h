#include <limits.h>
#include <stdio.h>

// Constants.
#define DEFAULT_OUTFILE "output.pll"
#define OP_SUCCESS INT_MIN + 1
#define OP_FAILURE INT_MIN
#define IDENTIFIER_LEN 11
#define NUMBER_LEN 5
#define KEYWORDS 13

// Container for keyword:lexeme value mappings.
typedef struct KeywordValuePair {
    char *string;
    int value;
} KeywordValuePair;

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
int analyzeSource(char*, char*);
int skipComment(FILE*);
void skipUnknownCharacter(char);

// Handler functional prototypes.
int isAlphanumeric(char);
int isAlphabetic(char);
int isDigit(char);
int checkKeywords(FILE*, char*);
int handleDirectMappedSymbol(FILE*, int);
int handlePair(FILE*, FILE*, char, char, int, int);
int handleLongToken(FILE*, FILE*, char, int, int);

// Printer functional prototypes.
void printSource(char*);
void printLexemeTableLine(const char*, int);
void printLexemeTable(char*);
void printLexemeList(char*);
void printFile(char*, char*);
