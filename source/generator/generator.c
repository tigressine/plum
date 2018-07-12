// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include <stdlib.h>
#include "generator.h"

typedef struct IOTunnel {
    int token;
    int value;
    FILE *fin;
    FILE *fout;
    char name[12];
} IOTunnel;

IOTunnel *createIOTunnel(char *lexemeFile, char *outFile) {
    IOTunnel *tunnel;

    if ((tunnel = malloc(sizeof(IOTunnel))) == NULL) {
        printError(ERROR_OUT_OF_MEMORY);

        return NULL;
    }

    if ((tunnel->fin = fopen(lexemeFile, "r")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, lexemeFile);
        free(tunnel);

        return NULL;
    }

    if ((tunnel->fout = fopen(outFile, "w")) == NULL) {
        printError(ERROR_FILE_NOT_FOUND, outFile);
        fclose(tunnel->fin);
        free(tunnel);

        return NULL;
    }

    return tunnel;
}

int loadToken(IOTunnel *tunnel) {
    if (tunnel == NULL) {
        printError(ERROR_NULL_CHECK);

        return SIGNAL_FAILURE;
    }

    if (fscanf(tunnel->fin, " %d", &(tunnel->token)) == EOF) {
        return SIGNAL_RECOVERY;
    }

    //if (tunnel->token == )

    return SIGNAL_SUCCESS;
}

void destroyIOTunnel(IOTunnel *tunnel) {
    fclose(tunnel->fin);
    fclose(tunnel->fout);
    free(tunnel);
}

int compileLexemes(char *lexemeFile, char *outFile, int options) {
    IOTunnel *tunnel;
    SymbolTable *table;

    if ((table = createSymbolTable()) == NULL) {
        return SIGNAL_FAILURE;
    }
    
    if ((tunnel = createIOTunnel(lexemeFile, outFile)) == NULL) {
        return SIGNAL_FAILURE;
    }

    destroyIOTunnel(tunnel);
    destroySymbolTable(table);

    return SIGNAL_SUCCESS;
}
