// Ishmael Perez
// Prof. Montagne
// This is the parser created for HW3. It's called by hw3tc.c, which handles
// the entire compilation, assembly, and execution process.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define MAX_SYM_TABLE_SIZE 100
#define MAX_LEXI_LIST_SIZE 100

typedef struct symbol {
    int kind; 		// const = 1, var = 2, proc = 3
	char name[10];	// name up to 11 chars
	int val; 		// number (ASCII value) 
	int level; 		// L level
	int addr; 		// M address
	int mark;		// to indicate that code has been generated already for a block.
} symbol;

symbol symTable[MAX_SYM_TABLE_SIZE] = {0};
instruction code[MAX_CODE_LENGTH];

int sp = 1, level = 1, currLexLevel = 1, cx = 0, codeLength = 0;
int symbolIndex = 0, currRegister = 0;
tokenStruct currToken;

FILE *input;
FILE *parser_out;

void program();
void block();
void declareConst();
void declareVar(int *space);
void declareProc(int *jumpAddress, int *procedureIndex);
void statement();
void condition();
int checkTokenOp();
void expression();
void term();
void factor();
void token();
void emit(int op, int r, int l, int m);
void putInTable(int kind, char *name, int val, int addr);

void doParser(char *fileIn)
{
    input = fopen(fileIn, "r");
    if (input == NULL)
        throwError(28); // file not found

    parser_out = fopen("parser_out.txt", "w");

    program();

    // If we got this far, there were no issues with code generation!

    fprintf(stdout, "Code generated successfully; no errors found!\n");

    for (int i = 0; i < codeLength; i++)
        fprintf(parser_out, "%i %i %i %i\n", code[i].op, code[i].r, code[i].l, code[i].m);

    fclose(input);
    fclose(parser_out);
}

void program()
{
    // Token is a global int, so we can update it from anywhere in the program.
    token();

    // Begin parsing the input file from the first character.
    block();

    if (currToken.type != periodsym)
        throwError(9);
}

// This and the other parse___ functions follow the assignment pdf's instructions.
void block()
{
    sp = 3;
    int space = 4, procedureIndex;
    int jumpAddress = cx;

    // Emit a jump. We'll replace it later if we don't actually need it.
    emit(7, 0, 0, 0);

    if (currToken.type == constsym)
        declareConst();
    if (currToken.type == varsym)
        declareVar(&space);
    if (currToken.type == procsym)
        declareProc(&jumpAddress, &procedureIndex);
    
    code[jumpAddress].m = cx;
    emit(6, 0, 0, space);
    statement();

    currLexLevel--;
}

// make sure the constant follows rules for constants -- no variable attached, etc.
void declareConst()
{
    char name[12];
    int val;

    do
    {
        token();
        // check for identifier
        if (currToken.type != identsym)
            throwError(4); // err: const int proc must be followed by identifier
        
        strcpy(name, currToken.ident);

        token();
        // check for ':='
        if (currToken.type != becomessym)
            throwError(3); // err: ident must be followed by =

        token();
        // check for number
        if (currToken.type != numbersym)
            throwError(2); // err: '=' must be followed by a number
        
        val = currToken.value;
        putInTable(constsym, name, val, 0);
        
        token();

    } while (currToken.type == commasym);

    if (currToken.type != semicolonsym)
        throwError(5); // err: expected semicolon

    token();
}

void declareVar(int *space)
{   
    char name[12];
    int val;

    do
    {
        token();

        if (currToken.type != identsym)
            throwError(4);  // err: missing identifier
        strcpy(name, currToken.ident);

        putInTable(varsym, name, 0, sp);
        symTable[symbolIndex - 1].level = level;
        *space++;
        sp++;
        token();
    } while (currToken.type == commasym);

    if (currToken.type != semicolonsym)
        throwError(5); // err: expected semicolon
    
    token();
}

void declareProc(int *jumpAddress, int *procedureIndex)
{
    char name[12];

    while (currToken.type == procsym)
    {
        token();

        if (currToken.type != identsym)
            throwError(4); // err: identifier missing
        
        strcpy(name, currToken.ident);

        putInTable(procsym, name, 0, 0);
        *procedureIndex = symbolIndex - 1;
        symTable[*procedureIndex].level = level;
        symTable[*procedureIndex].addr = *jumpAddress + 1;

        token();

        if (currToken.type != semicolonsym)
            throwError(17); // err: semicolon expected

        token();

        level++;

        block();
        if (currToken.type != semicolonsym)
            throwError(17);
        
        token();
    }
}

void statement()
{
    int currIndex, declared = 0, identIndex;

    if (currToken.type == identsym)
    {
        for (int i = symbolIndex - 1; i >= 0; i--)
        {
            if (currToken.ident == symTable[i].name)
            {
                if (symTable[i].kind == constsym || symTable[i].kind == procsym)
                    throwError(12); // err: assignment to const/procedure not allowed
            }
            else if (symTable[i].kind == varsym)
            {
                declared = 1;
                identIndex = i;
                break;
            }
        }

        if (declared == 0)
            throwError(11); // err: undeclared ident

        token();

        if (currToken.type != becomessym)
            throwError(3); // err: ident must be followed by =
        
        token();

        expression();

        emit(4, currRegister - 1, currLexLevel - symTable[identIndex].level, symTable[identIndex].addr - 1);

        currRegister--;
    }
    else if (currToken.type == callsym)
    {
        declared = 0;

        token();

        if (currToken.type != identsym)
            throwError(14); // err: call must be followed by ident
        
        for (int i = symbolIndex - 1; i>= 0; i--)
        {
            if (currToken.ident == symTable[i].name)
            {
                identIndex = i;
                declared = 1;
                break;
            }
        }
        if (declared == 0)
            throwError(11); // err: ident undeclared

        if (symTable[identIndex].kind == procsym)
        {
            emit(5, 0, level, symTable[identIndex].addr);
            currLexLevel++;
        }
        else
            throwError(14); // err: call must be followed by ident

        token();
    }
    else if (currToken.type == beginsym)
    {
        token();
        statement();
        while (currToken.type == semicolonsym)
        {
            token();
            statement();
        }
        if (currToken.type != endsym)
            throwError(17); // err: end expected
        
        token();
    }
    else if (currToken.type == ifsym)
    {
        token();
        condition();

        if (currToken.type != thensym)
            throwError(16); // then expected
        
        token();
    
        int tempCX = cx;

        emit(8, currRegister - 1, 0, 0);

        statement();

        token();

        if (currToken.type == elsesym)
        {
            int tempCX2 = cx;

            emit(7, 0, 0, 0);
            code[tempCX].m = cx;

            token();
            statement();

            code[tempCX2].m = cx;
            currRegister--;
        }
        else
        {
            code[tempCX].m = cx;
            currRegister--;
        }
    }
    else if (currToken.type == whilesym)
    {
        int tempCX = cx, tempCX2 = cx;

        token();
        condition();

        emit(8, currRegister - 1, 0, 0);

        if (currToken.type != dosym)
            throwError(18); // do expected

        token();
        statement();

        emit(7, 0, 0, tempCX);
        code[tempCX2].m = cx;

        currRegister--;
    }
    else if (currToken.type == readsym)
    {
        token();
        if (currToken.type != identsym)
            throwError(27); // err: invalid symbol after read

        for (int i = symbolIndex - 1; i >= 0; i--)
        {
            if (currToken.ident == symTable[i].name)
            {
                identIndex = i;
                declared = 1;
                break;
            }
        }
        if (declared == 0)
            throwError(11);

        emit(10, currRegister, 0, 2);

        if (symTable[identIndex].kind == varsym)
            emit(4, currRegister, currLexLevel - symTable[identIndex].level, symTable[identIndex].addr - 1);
        else if (symTable[identIndex].kind == 1)
            throwError(12); // err: illegal assignment to const/proc
        
        token();
    }
    else if (currToken.type == writesym)
    {
        token();
        if (currToken.type != identsym)
            throwError(27); // err: invalid symbol after read

        for (int i = symbolIndex - 1; i >= 0; i--)
        {
            if (currToken.ident == symTable[i].name)
            {
                identIndex = i;
                declared = 1;
                break;
            }
        }
        if (declared == 0)
            throwError(11);

        if (symTable[identIndex].kind == varsym)
        {
            emit(3, currRegister, currLexLevel - symTable[identIndex].level, symTable[identIndex].addr - 1);
            emit(9, currRegister, 0, 1);
        }
        else if (symTable[identIndex].kind == constsym)
        {
            emit(1, currRegister, 0, symTable[identIndex].val);
            emit(9, currRegister, 0, 1);
        }

        token();
    }
}

void condition()
{
    if (currToken.type == oddsym)
    {
        token();
        expression();
        emit(17, currRegister - 1, currRegister - 1, 0);
    }
    else
    {
        expression();
        int relOp = checkTokenOp();
        if (relOp == 0)
            throwError(20); // err: expected relationship op
        
        token();
        expression();

        emit(relOp, currRegister - 2, currRegister - 2, currRegister - 1);
        currRegister--;
    }
}

int checkTokenOp()
{
    switch(currToken.type)
    {
        case eqsym:
            return 19;
            break;
        case neqsym:
            return 20;
            break;
        case lessym:
            return 21;
            break;
        case leqsym:
            return 22;
            break;
        case gtrsym:
            return 23;
            break;
        case geqsym:
            return 24;
            break;
        default:
            return 0;
    }
}

void expression()
{
    int addOp;

    if (currToken.type == plussym || currToken.type == minussym)
    {
        addOp = currToken.type;
        token();
        term();

        if (addOp == minussym)
            emit(12, currRegister - 1, currRegister - 1, 0);
    }
    else
        term();
    
    while (currToken.type == plussym || currToken.type == minussym)
    {
        addOp = currToken.type;
        token();
        term();

        if (addOp == plussym)
        {
            emit(13, currRegister - 2, currRegister - 2, currRegister - 1);
            currRegister--;
        }
        // else addOp is minussym
        else
        {
            emit(14, currRegister - 2, currRegister - 2, currRegister - 1);
            currRegister--;
        }

    }
}
// parse multiplication and division symbols
void term()
{
    int multOp;

    factor();

    while (currToken.type == multsym || currToken.type == slashsym)
    {
        multOp = currToken.type;
        token();
        factor();

        if (multOp == multsym)
        {
            emit(15, currRegister - 2, currRegister - 2, currRegister - 1 );
            currRegister--;
        }
        // else multOp is slashsym
        else
        {
            emit(16, currRegister - 2, currRegister - 2, currRegister - 1 );
            currRegister--;
        }
    }
}

void factor()
{
    int declared = 0, identIndex = 0;

    if (currToken.type == varsym)
    {
        for (int i = symbolIndex - 1; i>= 0; i--)
        {
            if (currToken.ident == symTable[i].name)
            {
                identIndex = i;
                declared = 1;
                break;
            }
        }
        if (declared == 0)
            throwError(11); // err: ident undeclared

        if (symTable[identIndex].kind == varsym)
            emit(3, currRegister, currLexLevel - symTable[identIndex].level, symTable[identIndex].addr - 1);
        else if (symTable[identIndex].kind == constsym)
            emit(1, currRegister, 0, symTable[identIndex].val);

        currRegister++;
        token();
    }
    else if (currToken.type == procsym)
    {
        emit(1, currRegister, 0, currToken.value);
        currRegister++;
        token();
    }
    else if (currToken.type == lparentsym)
    {
        token();
        expression();
        if (currToken.type != rparentsym)
            throwError(22); // err: right parenthesis missing!

        token();
    }
    else
        throwError(23); // err: preceding factor can't begin with this symbol
}

// Retrieves token from lexeme list (input file).
void token()
{
    char *tempToken = malloc(sizeof(char) * (MAX_IDENT_LENGTH + 1));
    char *tempID = malloc(sizeof(char) * (MAX_IDENT_LENGTH + 1));
    char *tempVal = malloc(sizeof(char) * (MAX_IDENT_LENGTH + 1));

    fscanf(input, "%s", tempToken);

    currToken.type = atoi(tempToken);
    if (currToken.type == 2)
    {
        fscanf(input, "%s", tempID);
        strcpy(currToken.ident, tempID);
    }
    if (currToken.type == 3)
    {
        fscanf(input, "%s", tempVal);
        currToken.value = atoi(tempVal);
    }


    free(tempToken);
    free(tempID);
    free(tempVal);
}

// Write to code array, which will be entered into the output file after hitting "end".
void emit(int op, int r, int l, int m)
{
    code[cx].op = op;
    code[cx].r = r;
    code[cx].l = l;
    code[cx].m = m;

    cx++;
}

void putInTable(int kind, char *name, int val, int addr)
{
    symTable[symbolIndex].kind = kind;
    strcpy(symTable[symbolIndex].name, name);
    symTable[symbolIndex].val = val;
    symTable[symbolIndex].addr = addr;

    symbolIndex++;
}