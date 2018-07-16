// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include <string.h>
#include "scanner.h"

// Skip past a comment in the source file.
int skipComment(FILE *f) {
    char buffer;

    if (f == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }

    // Assumes that the "/*" characters have already been consumed by the
    // file pointer. Scans through the file until "*/" is found or the end
    // of the file is reached.
    while (fscanf(f, "%c", &buffer) != EOF) {
        if (buffer == '*') {
            fscanf(f, "%c", &buffer);
            if (buffer == '/') {
                return SIGNAL_SUCCESS;
            }
        }
    }

    return SIGNAL_FAILURE;
}

// Eat all remaining characters in a bad token.
void eatCharacters(FILE *fin, int lexemeValue) {
    char buffer;

    if (fin == NULL) {
        printError(ERROR_NULL_POINTER);

        return;
    }

    while (fscanf(fin, "%c", &buffer) != EOF) {

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
        { "begin", LEX_BEGIN },
        { "call", LEX_CALL },
        { "const", LEX_CONST },
        { "do", LEX_DO },
        { "else", LEX_ELSE },
        { "end", LEX_END },
        { "if", LEX_IF },
        { "procedure", LEX_PROCEDURE },
        { "then", LEX_THEN },
        { "read", LEX_READ },
        { "var", LEX_VAR },
        { "while", LEX_WHILE },
        { "write", LEX_WRITE }
    };
   
    if (fout == NULL || word == NULL) {
        printError(ERROR_NULL_POINTER);

        return SIGNAL_FAILURE;
    }

    // If any of the keywords match the word, return success.
    for (i = 0; i < KEYWORDS; i++) {
        if (strcmp(word, keywords[i].keyword) == 0) {
            fprintf(fout, "%d ", keywords[i].value);

            return SIGNAL_SUCCESS;
        }
    }
   
    return SIGNAL_FAILURE;
}

// Send a directly-mapped symbol's lexeme value to the output file.
int handleDirectMappedSymbol(FILE *fout, int lexemeValue) {
    if (fout == NULL) {
        printError(ERROR_NULL_POINTER);
        
        return SIGNAL_FAILURE;
    }

    fprintf(fout, "%d ", lexemeValue);
    
    return SIGNAL_SUCCESS;
}

// Print appropriate lexeme value to output file, based on presence of pair
// in the input file.
int handlePair(FILE *fin, FILE *fout, SymbolSymbolPair pair) {
    int i;
    char buffer;
    
    if (fin == NULL || fout == NULL) {
        printError(ERROR_NULL_POINTER);
        
        return SIGNAL_FAILURE;
    }
    
    if (fscanf(fin, "%c", &buffer) != EOF) {

        // Check all expected follow characters for pairs.
        for (i = 0; i < pair.pairs; i++) {
            if (buffer == pair.follows[i]) {
                if (pair.pairValues[i] == LEX_COMMENT) {
                    skipComment(fin);
                }
                else {
                    fprintf(fout, "%d ", pair.pairValues[i]);
                }

                return SIGNAL_SUCCESS;
            }
        }
    
        // Else the next character was something else. Rewind the file pointer.
        fseek(fin, -1, SEEK_CUR);
    }

    // If the solo value indicates the symbol is unknown, throw an
    // error, else print it to the output file.
    if (pair.soloValue == LEX_UNKNOWN) {
        printError(ERROR_UNKNOWN_CHARACTER, pair.lead);

        return SIGNAL_FAILURE;
    }
    else {
        fprintf(fout, "%d ", pair.soloValue);
    
        return SIGNAL_SUCCESS;
    }
}

// Handle long tokens like words and numbers in the input file.
int handleLongToken(FILE *fin, FILE *fout, char first, int lexemeValue, int len) {
    int index;
    char buffer;
    char token[len + 1];
    
    if (fin == NULL || fout == NULL) {
        printError(ERROR_NULL_POINTER);
        
        return SIGNAL_FAILURE;
    }

    index = 0;
    token[index++] = first;

    // Eat up more characters!
    while (index < len && fscanf(fin, "%c", &buffer) != EOF) {
       
        // If we are building an identifier and the buffer is alphanumeric
        // or if we are building a number and the buffer is a digit,
        // add to the token array.
        if ((lexemeValue == LEX_IDENTIFIER && isAlphanumeric(buffer)) ||
            (lexemeValue == LEX_NUMBER && isDigit(buffer))) {
            
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
            if (lexemeValue == LEX_NUMBER && isAlphabetic(buffer)) {
                printError(ERROR_ILLEGAL_IDENTIFIER, token);
            }
       
            // Else we either have a number that's too long or an
            // identifier that's too long.
            else if (lexemeValue == LEX_NUMBER) {
                printError(ERROR_NUMBER_TOO_LARGE, token);
            }
            else {
                printError(ERROR_IDENTIFIER_TOO_LARGE, token);
            }

            // Explode.
            return SIGNAL_FAILURE;
        }
    }

    // Make the token a bonafide string.
    token[index] = '\0';

    if (lexemeValue == LEX_IDENTIFIER) {

        // If the word doesn't match any keywords,
        // print into the output file as an identifier.
        if (checkKeywords(fout, token) == SIGNAL_FAILURE) {
            fprintf(fout, "%d %s ", LEX_IDENTIFIER, token);
        }
    }
    else if (lexemeValue == LEX_NUMBER){
        fprintf(fout, "%d %s ", LEX_NUMBER, token);
    }

    return SIGNAL_SUCCESS;
}
