#include <stdio.h>
#include <string.h>
#include "scanner/scanner.h"

void setOption(int *options, int option) {
    printf("%d\n", *options);
    printf("shift %d\n", 1 << option);
    *options |= (1 << option);
    printf("new %d\n", *options);
}

int checkOption(int *options, int option) {
    return (*options & (1 << option));
}

int getMode(char *mode) {
    if (mode == NULL) {
        //printError(ERROR_NO_MODE);
    }
    else {
        if (strcmp(mode, "run") == 0) {
            return MODE_RUN;
        }
        else if (strcmp(mode, "scan") == 0) {
            return MODE_SCAN;
        }
        else if (strcmp(mode, "compile") == 0) {
            return MODE_COMPILE;
        }
        else if (strcmp(mode, "execute") == 0) {
            return MODE_EXECUTE;
        }
        else {
            //printError(ERROR_BAD_MODE);
        }
    }
    
    return OPERATION_FAILURE;
}

// Process all arguments and flags for the program.
int getOptions(int argCount, char **argsVector) {
    int argIndex;
    int options;

    options = 0;

    // For each argument in the vector, if that argument is supported, fiddle
    // the correct bits in the options int.
    for (argIndex = 2; argIndex < argCount; argIndex++) {
        if (strcmp(argsVector[argIndex], "--skip-errors") == 0) {
            setOption(&options, OPTION_SKIP_ERRORS);
        }
        else if (strcmp(argsVector[argIndex], "--print-all") == 0) {
            setOption(&options, OPTION_PRINT_SOURCE);
            setOption(&options, OPTION_PRINT_LEXEME_TABLE);
            setOption(&options, OPTION_PRINT_LEXEME_LIST);
        }
        else if (strcmp(argsVector[argIndex], "--print-source") == 0) {
            setOption(&options, OPTION_PRINT_SOURCE);
        }
        else if (strcmp(argsVector[argIndex], "--print-lexeme-table") == 0) {
            setOption(&options, OPTION_PRINT_LEXEME_TABLE);
        }
        else if (strcmp(argsVector[argIndex], "--print-lexeme-list") == 0) {
            setOption(&options, OPTION_PRINT_LEXEME_LIST);
        }
        else if (strcmp(argsVector[argIndex], "--trace-all") == 0) {
            setOption(&options, OPTION_TRACE_CPU);
            setOption(&options, OPTION_TRACE_RECORDS);
            setOption(&options, OPTION_TRACE_REGISTERS);
        }
        else if (strcmp(argsVector[argIndex], "--trace-cpu") == 0) {
            setOption(&options, OPTION_TRACE_CPU);
        }
        else if (strcmp(argsVector[argIndex], "--trace-records") == 0) {
            setOption(&options, OPTION_TRACE_RECORDS);
        }
        else if (strcmp(argsVector[argIndex], "--trace-registers") == 0) {
            setOption(&options, OPTION_TRACE_REGISTERS);
        }
    }

    return options;
}

// Main entry point of program.
int main(int argCount, char **argsVector) {
    int mode;
    int options;
    char *outFile;
   
    if (argCount < 2) {
        //printError(ERROR_NO_MODE);

        return 0;
    }

    if ((mode = getMode(argsVector[1])) == OPERATION_FAILURE) {
        return 0;
    }

    options = getOptions(argCount, argsVector);

    switch (mode) {
        case MODE_RUN:
            break;

        case MODE_SCAN:
            break;

        case MODE_COMPILE:
            break;

        case MODE_EXECUTE:
            break;
    }

    return 0;
}
