// This is my HW2 but edited to work with HW3 and the header file.

// Ishmael Perez
// Professor Montagne
// COP3402 Summer 2018 HW2 (Scanner)
// 15 June 2018
/* This scanner takes in a .txt file of code and makes sure no errors are made in its structure.
    Errors include variable names starting with ints, variable names that are too long, and invalid symbols. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

char lexemes1[100][MAX_IDENT_LENGTH];
int lexemes2[100];
int halt = 0, currCell = 0, currLine = 0;

void parseStr(char* str);
int checkResWords(char* str);
int checkSymbols(char c);
int checkSymbolExt(char c, char d, char e);
char* makeSubStr(char* str, int l, int r);
int checkInt(char* str);
int checkVars(char* str);

FILE *input;
FILE *lex_out;

void doLexer(char *fileIn)
{
	input = fopen(fileIn, "r");
    if (input == NULL)
        throwError(28);

    lex_out= fopen("lex_out.txt", "w");

    char buffer[100];
    currLine = 0;

	// Go through the opened file line by line til EOF. Check for strings that imply keywords or variables or whatever
    while (fgets(buffer, 100, input) != NULL && !halt)
    {
		parseStr(buffer);
        currLine++;
    }

    for (int i = 0; i < currCell; i++)
    {
        fprintf(lex_out, "%i ", lexemes2[i]);

        if(lexemes2[i] == 2 || lexemes2[i] == 3)
            fprintf(lex_out, "%s ", lexemes1[i]);
    }
    fprintf(lex_out, "\n");
    
    fclose(input);
    fclose(lex_out);
}

// Take the current line from the buffer and parse it, looking for tokens.
void parseStr(char* str)
{
    // i will be our left-most counter and j will be our right-most one. We also make a length variable so we don't have to call strlen multiple times.
    int i = 0, j = 0, temp;
    int length = strlen(str);

    while (i <= j && j <= length)
    {
        // The current char on the right bound of our search is not a symbol or operator. Increase bound, in case it's a word.
        if (checkSymbols(str[j]) < 0)
            throwError(27);
        if (checkSymbols(str[j]) == 0)
        {
            j++;
        }
        if (checkSymbols(str[j]) > 0 && i == j)
        {
            temp = checkSymbolExt(str[j], str[j+1], str[j+2]);
            if (checkSymbols(str[j]) == 19 && str[j-3] == 'e' && str[j-2] == 'n' && str[j-1] == 'd' && str[j+1] == '.')
                halt = 1;
            if (temp < 0)
            {
                // This gets thrown when there's just a colon alone, which would pass the first test.
                throwError(27);
            }
            else if (temp == 0 && str[j] != ' ' && str[j] != '\0' && str[j] != '\n' && str[j] != '\r' && str[j] != '\t')
            {
                lexemes1[currCell][0] = str[j];
                lexemes1[currCell][1] = '\0';
                lexemes2[currCell] = checkSymbols(str[j]);
                currCell++;
            }
            else if (temp == 8)
            {
                strcpy(lexemes1[currCell], "odd");
                lexemes2[currCell] = temp;
                currCell++;
            }
            else if (temp > 0)
            {
                lexemes1[currCell][0] = str[j];
                lexemes1[currCell][1] = str[j+1];
                lexemes1[currCell][2] = '\0';
                lexemes2[currCell] = temp;
                currCell++;
                j++;
            }

            j++;
            i = j;
        }
        else if (checkSymbols(str[j]) > 0 && i != j || (j == length && i != j))
        {
            // Here we check for ints, reserved words, and variable names.
            char* subStr = makeSubStr(str, i, (j - 1));

            temp = checkResWords(subStr);
            if (temp > 1)
            {
                strcpy(lexemes1[currCell], subStr);
                lexemes2[currCell] = temp;
                currCell++;
            }
            else if (checkInt(subStr) > 0)
            {
                strcpy(lexemes1[currCell], subStr);
                lexemes2[currCell] = 3;
                currCell++;
            }
            else if (checkVars(subStr) > 0)
            {
                strcpy(lexemes1[currCell], subStr);
                lexemes2[currCell] = 2;
                currCell++;
            }
            i = j;
        }
    }
}

// A bunch of "if" statements to check if the generated subString is equal to any of the reserved words in the .doc list.
int checkResWords(char* str)
{
    if(strcmp(str,"const") == 0)
        return 28;
    if(strcmp(str,"var") == 0)
        return 29;
    if(strcmp(str,"procedure") == 0)
        return 30;
    if(strcmp(str,"call") == 0)
        return 27;
    if(strcmp(str,"begin") == 0)
        return 21;
    if(strcmp(str,"end") == 0)
        return 22;
    if(strcmp(str,"if") == 0)
        return 23;
    if(strcmp(str,"then") == 0)
        return 24;
    if(strcmp(str,"else") == 0)
        return 33;
    if(strcmp(str,"while") == 0)
        return 25;
    if(strcmp(str,"do") == 0)
        return 26;
    if(strcmp(str,"read") == 0)
        return 32;
    if(strcmp(str,"write") == 0)
        return 31;
    
    return 0;
}

// Checks for symbols. If it's something like a colon, the parseStr() function will check what follows to see if it's := for example.
int checkSymbols(char c)
{
    if(c == ' ' || c == '\t')
        return 1;
    if(c == ':')
        return 2;
    if(c == '+')
        return 4;
    if(c == '-')
        return 5;
    if(c == '*')
        return 6;
    if(c == '/')
        return 7;
    if(c == 'o')
        return -1;
    if(c == '=')
        return 9;
    if(c == '<')
        return 11;
    if(c == '>')
        return 13;
    if(c == '(')
        return 15;
    if(c == ')')
        return 16;
    if(c == ',')
        return 17;
    if(c == ';')
        return 18;
    if(c == '.')
        return 19;
    if(c == '$' || c == '#' || c == '&' || c == '!' || c == '_' || c == '~' || c== '`' || c == '?')
        return -1;
    
    return 0;
}

// After checking first symbol char, check if the following ones imply anything. This allows : to be invalid but := to be valid.
int checkSymbolExt(char c, char d, char e)
{
    if(c == ':' && d == '=')
        return 20;
    else
        return 0;

    if(c == '/' && d == '*')
        return 40;

    if(c == '*' && d == '/')
        return 41;

    if(c == 'o' && d == 'd' && e == 'd')
        return 8;
    else
        return 39;
    
    if(c == '<' && d == '=')
        return 12;
    
    if(c == '>' && d == '=')
        return 14;
    
    return 1;
}

// Makes a substring of string passed into it.
char* makeSubStr(char* str, int l, int r)
{
    char* subStr = malloc(sizeof(char) * (r - l + 2));
    for (int i  = l; i <= r; i++)
    {
        if (str[i] != ' ' && str[i] != '\0' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t')
            subStr[i - l] = str[i];
    }

    subStr[r - l + 1] = '\0';

    return subStr; 
}

// Check if int is acceptable int -- not too long and only consists of numbers.
int checkInt(char* str)
{
    int length = strlen(str);

    if (length == 0 || str[0] < '0' || str[0] > '9')
        return 0;

    for (int i = 0; i < length; i++)
    {
        if (str[i] <= '0' || str[i] >= '9')
            throwError(27);
    }

    if (length > MAX_NUM_LENGTH)
        throwError(25);

    return 1;
}

// Check if substring is possibly a variable name, i.e. a string of characters and ints beginning with a char. Return error if not.
int checkVars(char* str)
{
    int length = strlen(str);

    if (str[0] == ' ' || str[0] == '\0' || str[0] == '\n' || str[0] == '\r' || str[0] == '\t')
        return 0;
    if (length > MAX_IDENT_LENGTH)
        throwError(26);
    if (str[0] >= '0' && str[0] <= '9')
        return 0;
    if (checkSymbols(str[0]) < 0)
        return 0;

    for (int i = 1; i < length; i++)
    {
        if (checkSymbols(str[i]) > 0)
            return 0;
    }

    return 1;
}
