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

// Eat all remaining characters in a bad token.
void eatCharacters(FILE *fin, int lexemeValue) {
    char buffer;

    if (fin == NULL) {
        return;
    }

    while(fscanf(fin, "%c", &buffer) != EOF) {
        // Once a non-token character is detected, break.
        if ((lexemeValue == IDENTIFIER && !isAlphanumeric(buffer)) ||
            (lexemeValue == NUMBER && !isDigit(buffer))) {
            
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
    
    if (fin == NULL || fout == NULL) {
        return OP_FAILURE;
    }
    
    if (fscanf(fin, "%c", &buffer) != EOF) {
        // If the character is the expected follow character, they are a pair!
        if (buffer == pair.follow) {
            if (pair.pairValue == COMMENT) {
                skipComment(fin);
            }
            else {
                fprintf(fout, "%d ", pair.pairValue);
            }

            return OP_SUCCESS;
        }
        // Else the next character was something else. Rewind the file pointer.
        else {
            fseek(fin, -1, SEEK_CUR);
        }
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
    while(fscanf(fin, "%c", &buffer) != EOF) {
        // If we are building an identifier and the buffer is alphanumeric
        // or if we are building a number and the buffer is a digit,
        // add to the token array.
        if ((lexemeValue == IDENTIFIER && isAlphanumeric(buffer)) ||
            (lexemeValue == NUMBER && isDigit(buffer))) {
            
            token[index] = buffer;
        }
        // If an alphabetic character is detected in what was expected to be
        // a number, then throw an error and return failure.
        else if (lexemeValue == NUMBER && isAlphabetic(buffer)) {
            eatCharacters(fin, lexemeValue);

            token[index] = '\0';
            errorBadIdentifier(token);

            return OP_FAILURE;
        }
        // Else rewind the file because the end of the
        // token has been reached.
        else {
            fseek(fin, -1, SEEK_CUR);
            break;
        }

        // If the above conditional did not break and the index has reached
        // the end of the array, there are too many characters for this token.
        if (index >= len) {
            eatCharacters(fin, lexemeValue);    

            // Stringify for the error message and print the message.
            token[index] = '\0';
            errorTokenTooLong(token, len);

            return OP_FAILURE;
        }
      
        index++;
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
