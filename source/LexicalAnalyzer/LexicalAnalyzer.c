#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LexicalAnalyzer.h"

// Main entry point of program.
int main(int argsCount, char **argsVector) {
    int argIndex;
    char *outFile;
    
    // If no source file is provided, scream about it.
    if (argsCount < 2) {
        printf("ERROR: Please pass a PL/0 source file as your first argument.\n");

        return 0;
    }

    // Will trigger use of DEFAULT_OUTFILE later in code if no outFile is
    // specified in flags.
    outFile = NULL;

    // Parse arguments passed after first argument for known flags.
    for (argIndex = 2; argIndex < argsCount; argIndex++) {
        // Optional flag '-o' for outFile specification.
        if (strcmp(argsVector[argIndex], "-o") == 0) {
            // If There's no argument after the flag, scream about it.
            if (argIndex + 1 >= argsCount) {
                printf("ERROR: No argument provided after '-o' flag.\n");

                return 0;
            }
            // Otherwise point outFile at desired output file vector from user.
            else {
                outFile = argsVector[argIndex + 1];
                // Skip parsing the argument following the '-o' flag.
                argIndex++;
            }
        }
        // If an argument can't be understood, scream about it.
        else {
            printf("ERROR: Unknown arguments after initial argument.\n");

            return 0;
        }
    }
    
    // Analyze the provided source and print to an output file.
    analyzeSource(argsVector[1], (outFile == NULL) ? DEFAULT_OUTFILE : outFile);

    /*
    FILE *f;
    char buff;
    printf("here\n");
    //fscanf(NULL, "%c", &buff);
    f = fopen("hello.txt", "r");
    fscanf(f, "%c", &buff);
    fscanf(f, "%c", &buff);
    fscanf(f, "%c", &buff);
    fscanf(f, "%c", &buff);
    fclose(f);
    */

    return 0;
}

// Skip past a comment in the source file.
void skipComment(FILE *f) {
    char buffer;

    if (f == NULL) {// necessary i believe
        return;
    }

    // test all this
    while(fscanf(f, "%c", &buffer) != EOF) {
        if (buffer == '*') {
            fscanf(f, "%c", &buffer);
            if (buffer == '/') {
                return;
            }
        }
    }
}

int analyzeSource(char *sourceFile, char *outFile) {
    FILE *fin, *fout;
    int skip;
    char buffer;

    fin = fopen(sourceFile, "r");//null checks kiddo
    fout = fopen(outFile, "w");

    while (fscanf(fin, "%c", &buffer) != EOF) {
        printf("main buffer %c\n", buffer);
        switch (buffer) {
            case '+':
                fprintf(fout, "%d ", PLUS);
                break;
                
            case '-':
                fprintf(fout, "%d ", MINUS);
                break;
                
            case '*':
                fprintf(fout, "%d ", MULTIPLY);
                break;
                
            case '/'://not done
                if (fscanf(fin, "%c", &buffer) != EOF) {
                    if (buffer == '*') {
                        skipComment(fin);
                        //printf("skipping %d\n", skip);
                        //if (skip >= 0) {
                            //fscanf(fin, "%c", &buffer);
                            //printf("last item %c\n", buffer);
                            //fseek(fin, skip-4, SEEK_CUR);
                        //}
                        //else {
                            //explode cuz skip went wrong
                        //}
                    }
                    else {
                        fseek(fin, -1, SEEK_CUR);
                        fprintf(fout, "%d ", SLASH);
                    }
                }
                else {
                    // end of file calamety
                }
                
                break;
                
            case '(':
                fprintf(fout, "%d ", LEFT_PARENTHESIS);
                break;
                
            case ')':
                fprintf(fout, "%d ", RIGHT_PARENTHESIS);
                break;
                
            case ',':
                fprintf(fout, "%d ", COMMA);
                break;
                
            case '.':
                fprintf(fout, "%d ", PERIOD);
                break;
                
            case ';':
                fprintf(fout, "%d ", SEMICOLON);
                break;
                
            case '<':// scan another
            case '>':// ^
            case ':':
                if (fscanf(fin, "%c", &buffer) != EOF) {
                    if (buffer == '=') {
                        fprintf(fout, "%d ", EQUAL);
                    }
                    else {
                        fseek(fin, -1, SEEK_CUR);
                        // : isn't valid alone RIP
                        printf("what do : is alone\n");
                    }
                }
                else {
                    printf("ran outta file\n");
                    // this probably cant happen cuz files end with \n so itll
                    // always have an extra character to read
                    //explode cuz end of file
                    //might need to seek back one to not kaboom the loop
                }

                break;

            default:
                printf("unknown charavter %c\n", buffer);
                //prints this too many times probably something weird with rewinding
                break;
                
        }
    }

    fclose(fin);
    fclose(fout);
    
    return 0;
}
