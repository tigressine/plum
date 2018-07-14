// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include "plum.h"

// Set an option flag to true in an options int.
void setOption(int *options, int option) {
    if (options == NULL) {
        printError(ERROR_NULL_CHECK);

        return;
    }
    else {
        *options |= (1 << option);
    }
}

// See if an option is enabled in an options int.
int checkOption(int *options, int option) {
    if (options == NULL) {
        printError(ERROR_NULL_CHECK);
        
        return SIGNAL_FAILURE;
    }
    else {
        return (*options & (1 << option));
    }
}

// Check if the provided file exists.
int fileExists(char *filename) {
    FILE *f;

    if ((f = fopen(filename, "r")) == NULL) {
        return SIGNAL_FALSE;
    }
    else {
        fclose(f);
    
        return SIGNAL_TRUE;
    }
}

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

// Return if the character is whitespace.
int isWhitespace(char character) {
    return (character == '\0' || character == ' ' || character == '\t' || character == '\n'); 
}
