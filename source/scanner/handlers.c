#include <stdio.h>
#include <string.h>
#include "analyzer.h"

// Return if the character is alpabetic or numeric.
int isAlphanumeric(char character) {
    return (isAlphabetic(character) || isDigit(character));
}

// Return if the character is alphabetic.
int isAlphabetic(char character) {
    return ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'));
}

// Return if the character is a digit.
int isDigit(char character) {
    return (character >= '0' && character <= '9');
}

// Skip past a comment in the source file.
int skipComment(FILE *f) {
    char buffer;

    if (f == NULL) {
        return OP_FAILURE;
    }

    // Assumes that the "/*" characters have already been consumed by the
    // file pointer. Scans through the file until "*/" is found or the end
    // of the file is reached.
    while(fscanf(f, "%c", &buffer) != EOF) {
        if (buffer == '*') {
            fscanf(f, "%c", &buffer);
            if (buffer == '/') {
                return OP_SUCCESS;
            }
        }
    }

    return OP_FAILURE;
}

// Eat all remaining characters in a bad token.
void eatCharacters(FILE *fin, int lexemeValue) {
    char buffer;

    if (fin == NULL) {
        return;
    }

    while(fscanf(fin, "%c", &buffer) != EOF) {
        // Once a non-token character is detected, rewind and break.
        if (!isAlphanumeric(buffer)) {
            fseek(fin, -1, SEEK_CUR);
            break;
        }
    }
}

// Check word against all keywords for matches.
int checkKeywords(FILE *fout, char *word) {
    int i;
   
    KeywordValuePair keywords[] = {
        { "begin", BEGIN },
        { "call", CALL },
        { "const", CONST },
        { "do", DO },
        { "else", ELSE },
        { "end", END },
        { "if", IF },
        { "procedure", PROCEDURE },
        { "then", THEN },
        { "read", READ },
        { "var", VAR },
        { "while", WHILE },
        { "write", WRITE }
    };
   
    if (fout == NULL || word == NULL) {
        return OP_FAILURE;
    }

    // If any of the keywords match the word, return success.
    for (i = 0; i < KEYWORDS; i++) {
        if (strcmp(word, keywords[i].keyword) == 0) {
            fprintf(fout, "%d ", keywords[i].value);

            return OP_SUCCESS;
        }
    }
   
    return OP_FAILURE;
}

// Send a directly-mapped symbol's lexeme value to the output file.
int handleDirectMappedSymbol(FILE *fout, int lexemeValue) {
    if (fout == NULL) {
        return OP_FAILURE;
    }

    fprintf(fout, "%d ", lexemeValue);
    
    return OP_SUCCESS;
}

// Print appropriate lexeme value to output file, based on presence of pair
// in the input file.
int handlePair(FILE *fin, FILE *fout, SymbolSymbolPair pair) {
    char buffer;
    int i;
    
    if (fin == NULL || fout == NULL) {
        return OP_FAILURE;
    }
    
    if (fscanf(fin, "%c", &buffer) != EOF) {
        // Check all expected follow characters for pairs.
        for (i = 0; i < pair.pairs; i++) {
            if (buffer == pair.follows[i]) {
                if (pair.pairValues[i] == COMMENT) {
                    skipComment(fin);
                }
                else {
                    fprintf(fout, "%d ", pair.pairValues[i]);
                }

                return OP_SUCCESS;
            }
        }
        // Else the next character was something else. Rewind the file pointer.
        fseek(fin, -1, SEEK_CUR);
    }

    // If the solo value indicates the symbol is unknown, throw an
    // error, else print it to the output file.
    if (pair.soloValue == UNKNOWN) {
        errorUnknownCharacter(pair.lead);

        return OP_FAILURE;
    }
    else {
        fprintf(fout, "%d ", pair.soloValue);
    
        return OP_SUCCESS;
    }
}

// Handle long tokens like words and numbers in the input file.
int handleLongToken(FILE *fin, FILE *fout, char first, int lexemeValue, int len) {
    int index;
    char buffer;
    char token[len + 1];
    
    if (fin == NULL || fout == NULL) {
        return OP_FAILURE;
    }

    index = 0;
    token[index++] = first;

    // Eat up more characters!
    while(index < len && fscanf(fin, "%c", &buffer) != EOF) {
        // If we are building an identifier and the buffer is alphanumeric
        // or if we are building a number and the buffer is a digit,
        // add to the token array.
        if ((lexemeValue == IDENTIFIER && isAlphanumeric(buffer)) ||
            (lexemeValue == NUMBER && isDigit(buffer))) {
            
            token[index++] = buffer;
        }
        // Else rewind the file because the end of the
        // token has been reached.
        else {
            fseek(fin, -1, SEEK_CUR);
            break;
        }
    }

    // Check the next character.
    if (fscanf(fin, "%c", &buffer) != EOF) {
        fseek(fin, -1, SEEK_CUR);
        
        if (isAlphanumeric(buffer)) {
            eatCharacters(fin, lexemeValue);
            token[index] = '\0';

            // If we were building a number and it's followed by a letter,
            // this is actually an identifier with digits at the start.
            if (lexemeValue == NUMBER && isAlphabetic(buffer)) {
                errorBadIdentifier(token);
            }
            // Else we either have a number that's too long or an
            // identifier that's too long.
            else {
                errorTokenTooLong(token, len);
            }

            // Explode.
            return OP_FAILURE;
        }
    }

    // Make the token a bonafide string.
    token[index] = '\0';

    if (lexemeValue == IDENTIFIER) {
        // If the word doesn't match any keywords,
        // print into the output file as an identifier.
        if (checkKeywords(fout, token) == OP_FAILURE) {
            fprintf(fout, "%d %s ", IDENTIFIER, token);
        }
    }
    else if (lexemeValue == NUMBER){
        fprintf(fout, "%d %s ", NUMBER, token);
    }

    return OP_SUCCESS;
}
