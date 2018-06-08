#include <limits.h>

#define DEFAULT_OUTFILE "ff.out"
#define OP_FAILURE INT_MIN
#define OP_SUCCESS INT_MIN + 1

enum {
    NUL = 1, IDENTIFIER,
    NUMBER, PLUS, MINUS,
    MULTIPLY, SLASH, ODD,
    EQUAL, NOT_EQUAL, LESS,
    LESS_EQUAL, GREATER,
    GREATER_EQUAL, LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS, COMMA,
    SEMICOLON, PERIOD, BECOME,
    BEGIN, END, IF, THEN, WHILE,
    DO, CALL, CONSTANT, VARIABLE,
    PROCEDURE, WRITE, READ, ELSE
} TokenTypes;

int skipComment(FILE*);
int analyzeSource(char*, char*);
void skipUnknownCharacter(char);
