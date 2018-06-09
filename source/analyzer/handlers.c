#include <stdio.h>
#include <string.h>
#include "analyzer.h"

// Check if character is alpabetic or numeric.
int isAlphanumeric(char character) {
    return (isAlphabetic(character) || isDigit(character));
}

int isAlphabetic(char character) {
    return ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'));
}

int isDigit(char character) {
    return (character >= '0' && character <= '9');
}

// Check if word is a keyword.
int isKeyword(FILE *fout, char *word, KeywordValuePair keyword) {
    if (fout == NULL || word == NULL) {
        return 0;
    }

    // If keyword and word match, print keyword's lexeme value to output file.
    if (strcmp(word, keyword.string) == 0) {
        fprintf(fout, "%d ", keyword.value);

        return 1;
    }
    else {
        return 0;
    }
}

// Check word against all keywords for matches.
int checkKeywords(FILE *fout, char *word) {
    int i;
   
    // An array of keywords and their associated LexemeValues.
    const KeywordValuePair keywords[] = {
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

    // For each keyword, check if word matches. If so, return success.
    for (i = 0; i < KEYWORDS; i++) {
        if (isKeyword(fout, word, keywords[i])) {
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

// Handle words (and possible keywords) in the input file.
int handleWord(FILE *fin, FILE *fout, char first) {
    int index;
    char buffer;
    char word[IDENTIFIER_LEN + 1];
    
    if (fin == NULL || fout == NULL) {
        return OP_FAILURE;
    }

    index = 0;
    word[index++] = first;

    // Eat up more characters!
    while(index < IDENTIFIER_LEN && fscanf(fin, "%c", &buffer) != EOF) { // add for too many
        // If it is a valid character for an identifier,
        // add it to the word.
        if (isAlphanumeric(buffer)) {
            word[index++] = buffer;
        }
        // Else rewind the file because the end of the
        // identifier/keyword has been reached.
        else {
            fseek(fin, -1, SEEK_CUR);
            
            break;
        }
    }

    // Make the word a bonafide string.
    word[index] = '\0';

    // If the word doesn't match any keywords, treat it
    // like an identifier.
    if (checkKeywords(fout, word) == OP_FAILURE) {
        fprintf(fout, "%d %s ", IDENTIFIER, word);
    }

    return OP_SUCCESS;
}

int handleNumber(FILE *fin, FILE *fout, char first) {
    int index;
    char buffer;
    char number[NUMBER_LEN + 1];
    
    if (fin == NULL || fout == NULL) {
        return OP_FAILURE;
    }

    index = 0;
    number[index++] = first;

    // Eat up more digits!
    while(index < NUMBER_LEN && fscanf(fin, "%c", &buffer) != EOF) { // add for too many
        // If it is a valid digit for a number,
        // add it to the number.
        if (isDigit(buffer)) {
            number[index++] = buffer;
        }
        // Else rewind the file because the end of the
        // number has been reached.
        else {
            fseek(fin, -1, SEEK_CUR);
            
            break;
        }
    }

    number[index] = '\0';

    // Print the number and it's lexeme value to the output file.
    fprintf(fout, "%d %s ", NUMBER, number);

    return OP_SUCCESS;
}
