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

// Check word against all keywords for matches.
int checkKeywords(FILE *fout, char *word) {
    int i;
   
    // An array of keywords and their associated lexeme values.
    KeywordValuePair keywords[] = {
        { "begin", BEGIN },
        { "call", CALL },
        { "const", CONSTANT },
        { "do", DO },
        { "else", ELSE },
        { "end", END },
        { "if", IF },
        { "procedure", PROCEDURE },
        { "then", THEN },
        { "read", READ },
        { "var", VARIABLE },
        { "while", WHILE },
        { "write", WRITE }
    };
   
    if (fout == NULL || word == NULL) {
        return OP_FAILURE;
    }

    // If any of the keywords match the word, return success.
    for (i = 0; i < KEYWORDS; i++) {
        if (strcmp(word, keywords[i].string) == 0) {
            fprintf(fout, "%d ", keywords[i].value);

            return OP_SUCCESS;
        }
    }
   
    // No keywords matched word, return failure.
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
int handlePair(FILE *fin, FILE *fout, char first, char second, int pairValue, int soloValue) {
    char buffer;
    
    if (fin == NULL || fout == NULL) {
        return OP_FAILURE;
    }
    
    // Check the next character in the file.
    if (fscanf(fin, "%c", &buffer) != EOF) {
        // If the character is the expected second character, they are a pair!
        if (buffer == second) {
            // If the pair indicates the start of a comment block, skip the comment.
            if (pairValue == COMMENT) {
                skipComment(fin);
            }
            // Otherwise add the pair value to the output file.
            else {
                fprintf(fout, "%d ", pairValue);
            }
        }
        // Else the next character was something else. Rewind the
        // file pointer and print the soloValue to the output file.
        else {
            fseek(fin, -1, SEEK_CUR);

            // If the solo value indicates the symbol is unknown, skip the
            // character, else print it to the output file.
            if (soloValue == UNKNOWN) {
                skipUnknownCharacter(first);
            }
            else {
                fprintf(fout, "%d ", soloValue);
            }
        }
    }
    else {
        // If the solo value indicates the symbol is unknown, skip the
        // character, else print it to the output file.
        if (soloValue == UNKNOWN) {
            skipUnknownCharacter(first);
        }
        else {
            fprintf(fout, "%d ", soloValue);
        }
    }

    return OP_SUCCESS;
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
    while(index < len && fscanf(fin, "%c", &buffer) != EOF) { // add for too many
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
        // Print the token into the output file as a number.
        fprintf(fout, "%d %s ", NUMBER, token);
    }

    return OP_SUCCESS;
}
