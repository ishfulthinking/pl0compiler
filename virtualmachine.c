// This is my HW1 but edited to work with HW3 & the header file that keeps things organized.

// Ishmael Perez and Trevor Tosi
// Professor Montagne
// COP3402 Summer 2018 HW1 (PM/0 Virtual machine)
// 2 June 2018
/*  Compile this program by entering into the command line: "gcc -o pm0vm pm0vm.c"
	Run this program after compilation by entering: "./pm0vm input.txt" where input.txt
		is a text file containing instructions for the virtual machine. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"

#define MAX_STACK_HEIGHT 2000
#define MAX_LEXI_LEVELS	3
#define NUM_REGISTERS 16

int stack[MAX_STACK_HEIGHT] = { 0 };
int reg[NUM_REGISTERS] = { 0 };
int code[MAX_CODE_LENGTH][4];
const char *instructWord[24] = { "inc","lit","rtn","lod","sto","cal","inc","jmp","jpc","sio","neg","add","sub",
                                "mul","div","odd","mod","eql","neq","lss","leq","gtr","geq" };
instruction currentInstruction = {0, 0, 0, 0};
int bar[MAX_STACK_HEIGHT / 2];
int barIndex = 0;
int SP = 0, BP = 1, PC = 0, HALT = 0;

void fetchInstruction();
void execute();
void printStack();

FILE *input;
FILE *stackTrace;
FILE *output;

void doVirtualMachine(char *fileIn)
{
	stack[2] = 1;
	input = fopen(fileIn, "r");
	if (input == NULL)
		throwError(29);

	stackTrace = fopen("vm_trace.txt", "w");
	output = fopen("out.txt", "w");

	char buffer[100];
    int i = 0;

	// Parse input by reading line, then separating each integer based on spaces.
	// We make the "code" readable by putting it into a 2D array where each column is OP, R, L, and M.
    while (fgets(buffer, 300, input) != NULL)
	{	
		char *ptr = strdup(buffer);
		code[i][0] = atoi(strsep(&ptr, " "));
		code[i][1] = atoi(strsep(&ptr, " "));
		code[i][2] = atoi(strsep(&ptr, " "));
		code[i][3] = atoi(strsep(&ptr, " "));
        i++;
    }

	fprintf(stackTrace, "\n%-16s%-3s %-3s %-3s  Stack\n",
			"OP     R  L  M", "PC", "BP", "SP");

	// The program runs until a HALT condition is met.
    while(!HALT) {
    	fetchInstruction();
		PC++;
		execute();
        printStack();
    	fprintf(stackTrace, "\n");
	}
  
	fclose(input);
	fclose(stackTrace);
	fclose(output);
}

// Instructions are fed into the instruction struct based on the contents of the current "line" of code in the array.
void fetchInstruction() {
	currentInstruction.op = code[PC][0];
	currentInstruction.r = code[PC][1];
	currentInstruction.l = code[PC][2];
	currentInstruction.m = code[PC][3];
}

int base(int l, int base)
{
    int b1 = base;

    while (l > 0)
    {
        b1 = stack[b1 + 1];
        l--;
    }

    return b1;
}

void execute() {
	// This is here just to make typing faster.
	int r = currentInstruction.r;   // R[i] == R[r]
	int l = currentInstruction.l;   // R[j] == R[l]
	int m = currentInstruction.m;   // R[k] == R[m]


	switch(currentInstruction.op) {
		case 1:
			reg[r] = m;
			break;
		case 2:
			SP = BP - 1;
			BP = stack[SP + 3];
            PC = stack[SP + 4];
			barIndex--;
			bar[barIndex] = -1;
			break;
		case 3:
			reg[r] = stack[base(l, BP) + m];
			break;
		case 4:
            stack[base(l, BP) + m] = reg[r];
			break;
		case 5:
			stack[SP+1] = 0;
			stack[SP+2] = base(l, BP);
			stack[SP+3] = BP;
			stack[SP+4] = PC;
			BP = SP +1;
			PC = m ;
            bar[barIndex] = SP;
            barIndex++;
			break;
		case 6:
            SP = SP + m;
			break;	
		case 7:
			PC = m;
			break;
		case 8:
            if(reg[r] == 0) PC = m;
			break;
		case 9:
			if (m == 1)
				fprintf(output, "%i\n", reg[r]);
			else if (m == 2)
				fscanf(input, "%i", &reg[r]);
			else {
				PC = 0;
				SP = 0;
				BP = 1;
				HALT = 1;
			}
			break;	
		case 10:
            reg[r] = -reg[l];
			break;
		case 11:
			reg[r] = reg[l] + reg[m];
			break;
		case 12:
            reg[r] = (reg[l] - reg[m]);
			break;
		case 13:
			reg[r] = reg[l] * reg[m];
			break;
		case 14:
            reg[r] = (reg[l] / reg[m]);
			break;
		case 15:
			reg[r] = reg[r] % 2;
			break;
		case 16:
            reg[r] = (reg[l] % reg[m]);
			break;
		case 17:
			reg[r] = (reg[l] == reg[m]);
			break;
		case 18:
            reg[r] = (reg[l] != reg[m]);
			break;
		case 19:
			reg[r] = (reg[l] < reg[m]);
			break;
		case 20:
            reg[r] = (reg[l] <= reg[m]);
			break;
		case 21:
			reg[r] = (reg[l] > reg[m]);
			break;
		case 22:
            reg[r] = (reg[l] >= reg[m]);
			break;
	}

}

void printStack()
{
    int j = 0;

	fprintf(stackTrace, "%-2i %-3s %-2i %-2i %-2i %-3i %-3i %-3i ",
			currentInstruction.op, instructWord[currentInstruction.op], currentInstruction.r, currentInstruction.l, currentInstruction.m, PC, BP, SP);

    if (SP == 0) printf(" 0");
	// We store the locations of lexicographical level divisions through bar[] and print when we reach that location in the stack.
    for (int i = 1; i <= SP; i++)
    {
        fprintf(stackTrace, " %i", stack[i]);
        if (bar[j] == i && barIndex > 0)
        {
            fprintf(stackTrace, " |");
            j++;
        }
    }
}
