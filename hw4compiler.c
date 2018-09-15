// Ishmael Perez
// Prof. Montagne
// COP3402 Summer 18 HW3 (Tiny PL0)
// 14 July 2018
/* This "tiny PL/0" compiler takes in a text file of code and moves/translates it
 * through the different steps of compilation and execution: lexical analysis,
 * parsing, and execution on a virtual machine.
 * It's organized into 4 different .c programs and linked by the header.h file. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// This program uses a lot of global macro definitions, structs, and functions -- they're in this header file.
#include "header.h"

// Here each error message is defined -- it looks chaotic but helps with clean error handling!
const char *errors[31] = { "Invalid number of arguments.\nMake sure you compile with 'gcc -o hw3tc hw3tc.c lexer.c parser.c virtualmachine.c header.h'\nand execute the program with './hw3tc <inputfile> <optionalargs>'",
                        "Used = instead of :=", "= must be followed by a number", "Identifier must be followed by =",
                        "const, var, procedure must be followed by identifier", "Semicolon or comma missing", "Incorrect symbol after procedure declaration",
                        "Statement expected", "Incorrect symbol after statement part in block", "Period expected",
                        "Semicolon between statements missing", "Undeclared identifier", "Assignment to constant or procedure is not allowed",
                        "Assignment operator expected", "call must be followed by an identifier", "Call of a constant or variable is meaningless",
                        "'then' expected", "Semicolon or } expected", "'do' expected",
                        "Incorrect symbol following statement", "Relational operator expected", "Expression must not contain a procedure identifier",
                        "Right parenthesis missing", "The preceding factor cannot begin with this symbol", "An expression cannot begin with this symbol",
                        "This number is too large", "Identifier too long", "Invalid symbol",
                        "File not found", "Invalid operation code", "Code too long" };

// printFlags[0] = print lexemes?, [1] = print assembly?, [2] = print stacktrace?.
int printFlags[3] = { 0, 0, 0 };

void printFile(char *fileIn, char *filePurpose);
void throwError(int errorNum);

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 5)
        throwError(0);  // error: invalid args!
    
    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0)
            printFlags[0] = 1;
        else if (strcmp(argv[i], "-a") == 0)
            printFlags[1] = 1;
        else if (strcmp(argv[i], "-v") == 0)
            printFlags[2] = 1;
        else
            throwError(0);  // error: invalid args!
    }
    
    printFile(argv[1], "Input File");
    // Execute the scanner etc. by passing in the filenames, and print if specified.
    doLexer(argv[1]);
    if (printFlags[0])
        printFile("lex_out.txt", "Lexemes/Tokens");

    doParser("lex_out.txt");
    if (printFlags[1])
        printFile("parser_out.txt", "Generated Assembly Code");

    doVirtualMachine("parser_out.txt");
    if (printFlags[2])
        printFile("vm_trace.txt", "Virtual Machine Execution Trace");

    printFile("out.txt", "Output File");

    return 0;
}

void printFile(char *fileIn, char *filePurpose)
{
    char c;

	FILE* ifp = fopen(fileIn, "r");
    if (ifp == NULL)
        throwError(28);

    fprintf(stdout, "\n%s:\n", filePurpose);

    rewind(ifp);
    while((c = fgetc(ifp)) !=EOF)
        printf("%c", c);

    fprintf(stdout, "\n");
}

void throwError(int errorNum)
{
    fprintf(stderr, "Error [%i]: %s.\n", errorNum, errors[errorNum]);
    exit(errorNum);
}