// Part of Plum by Tiger Sachse.

#include <stdio.h>
#include <string.h>
#include "plum.h"
#include "scanner/scanner.h"
#include "machine/machine.h"
#include "generator/generator.h"

// Get the mode of the machine.
int getMode(char *mode) {
    if (mode == NULL) {
        printError(ERROR_NO_MODE);
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
            printError(ERROR_BAD_MODE, mode);
        }
    }
    
    return SIGNAL_FAILURE;
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

// Get the output file for intermediate code, if specified.
int getOutFile(int argCount, char **argsVector) {
    int argIndex;

    for (argIndex = 3; argIndex < argCount; argIndex++) {
        if (strcmp(argsVector[argIndex], "--output-file") == 0 ||
            strcmp(argsVector[argIndex], "-o") == 0) {

            // If the output file is found and it is followed by another
            // argument, then return the index of that argument.
            if (argIndex + 1 < argCount) {
                return argIndex + 1;
            }
            else {
                printError(ERROR_MISSING_ARGUMENT, argsVector[argIndex]);
                
                return SIGNAL_FAILURE;
            }
        }
    }

    // Signal to the caller that there is no specified output file, so
    // it must be set to a default.
    return SIGNAL_RECOVERY;
}

// Main entry point of program.
int main(int argCount, char **argsVector) {
    int mode;
    int options;
    char *inFile;
    char *outFile;
    int outFileIndex;

    // If there aren't enough arguments passed, scream about it.
    if (argCount < 2) {
        printError(ERROR_NO_MODE);

        return 0;
    }

    // If an invalid mode was passed as the first argument, terminate the program.
    if ((mode = getMode(argsVector[1])) == SIGNAL_FAILURE) {
        return 0;
    }

    // If there aren't enough arguments passed to contain an input file,
    // scream about it.
    if (argCount < 3) {
        printError(ERROR_NO_SOURCE);

        return 0;
    }

    // If the input file doesn't exist, scream about it.
    inFile = argsVector[2];
    if (fileExists(inFile) == SIGNAL_FALSE) {
        printError(ERROR_FILE_NOT_FOUND, inFile);

        return 0;
    }

    // Parse options from the argsVector.
    options = getOptions(argCount, argsVector);

    // If there's something wrong with how the output file is passed to the
    // program, terminate.
    if ((outFileIndex = getOutFile(argCount, argsVector)) == SIGNAL_FAILURE) {
        return 0;
    }
    // If there was no output flag, set outFile to a default.
    else if (outFileIndex == SIGNAL_RECOVERY) {
        outFile = DEFAULT_OUTPUT_FILE;
    }
    // Set outFile to the array in the vector at the provided index.
    else {
        outFile = argsVector[outFileIndex];
    }

    switch (mode) {
        case MODE_RUN:
            break;

        case MODE_SCAN:
            scanSource(inFile, outFile, options);
            break;

        case MODE_COMPILE:
            compileLexemes(inFile, outFile, options);
            break;

        case MODE_EXECUTE:
            startMachine(inFile, options);
            break;
    }

    return 0;
}
