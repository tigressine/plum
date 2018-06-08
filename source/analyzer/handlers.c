#include <stdio.h>
#include "analyzer.h"

// Send a directly-mapped symbol's lexeme value to the output file.
int handleDirectMappedSymbol(FILE *f, int symbol) {
    if (f == NULL) {
        return OP_FAILURE;
    }

    fprintf(f, "%d ", symbol);
    
    return OP_SUCCESS;
}

int handlePair(FILE *fin, FILE *fout, char first, char second, char pairValue, char soloValue) {
    char buffer;
    
    // Protect fprintf from NULL file pointers.
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
        // file pointer back one and print the soloValue to the
        // output file.
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
    else {// try these as ternaries/////////////////////////////////
        if (soloValue == UNKNOWN) {
            skipUnknownCharacter(first);
        }
        else {
            fprintf(fout, "%d ", soloValue);
        }
    }

    return OP_SUCCESS;
}

int handleWord(FILE *fin, FILE *fout, char first) {
    char buffer;
    char word[IDENTIFIER_LEN + 1];//
    
    //use loop, read characters until either length cap or non alphanumeric
    //once done compare? to all reserved words? oof seems like the monkaS way
    //if not a keyword then save buffer to fout with whatever value
    //
    if (fin == NULL || fout == NULL) {
        return OP_FAILURE;
    }

}
