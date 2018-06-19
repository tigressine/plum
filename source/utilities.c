#include <stdio.h>

// Set an option flag to true in an options int.
void setOption(int *options, int option) {//unsafe
    *options |= (1 << option);
}

// See if an option is enabled in an options int.
int checkOption(int *options, int option) {//unsafe
    return (*options & (1 << option));
}

// Check if the provided file exists.
int fileExists(char *filename) {
    FILE *f;

    if ((f = fopen(filename, "r")) == NULL) {
        return 0;
    }
    else {
        fclose(f);
    
        return 1;
    }
}
