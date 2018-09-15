// Header file for TinyPL0, since there are so many structs, definitions, etc...

#ifndef HEADER_H
#define HEADER_H

// Global macro definitions
#define MAX_IDENT_LENGTH 11
#define MAX_NUM_LENGTH 5
#define MAX_CODE_LENGTH 500

// Structs/enumerations from the past 2 HW assignments, and this one (3)
typedef struct instruction {
	int op; // opcode
	int r;	// register
	int l;	// lexicographical level
	int m;  // offset
} instruction;

typedef enum token_type {
	nulsym = 1, identsym, numbersym,    plussym,   minussym,
	multsym,    slashsym, oddsym,       eqsym,     neqsym,
	lessym,     leqsym,   gtrsym,       geqsym,    lparentsym,
	rparentsym, commasym, semicolonsym, periodsym, becomessym,
	beginsym,   endsym,   ifsym,        thensym,   whilesym,
	dosym,      callsym,  constsym,     varsym,    procsym,
	writesym,   readsym,  elsesym
} token_type;

typedef struct tokenStruct {
	int type;
	int value;
	char ident[MAX_IDENT_LENGTH];
} tokenStruct;

// The error messages are defined in hw3tc.c
const char *errors[31];

void doLexer(char *fileIn);
void doParser(char *fileIn);
void doVirtualMachine(char *fileIn);
void throwError(int errorNum);

#endif