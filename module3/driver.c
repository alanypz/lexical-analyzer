//
//  driver.c
//  Parser & Code Generator
//
//  UCF - COP 3402: Systems Software
//  Summer 2015
//
//  Team Members:
//  Justin Mackenzie
//  Alan Yepez
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

//  I/O file names.
#define INPUT_FILE "input.txt"
#define CLEAN_OUTPUT_FILE "cleaninput.txt"
#define TABLE_OUTPUT_FILE "lexemetable.txt"
#define LIST_OUTPUT_FILE "lexemelist.txt"

#define MAX_SYMBOL_TABLE_SIZE 100

//  Internal representation of PL\0 Tokens.
typedef enum Tokens{
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4,
    minussym = 5, multsym = 6,  slashsym = 7, oddsym = 8,
    eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
    commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
    beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
    varsym = 29, procsym = 30, writesym = 31, readsym = 32,
    elsesym = 33
}token_type;

typedef struct symbol {
    int kind;       // const = 1, var = 2, proc = 3
    char name[12];  // name up to 11 chars
    int val;        // number (ASCII value)
    int level;      // L level
    int addr;       // M address
} symbol;

//  Global variable declaration.
FILE *fp = NULL;
int *token;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int symbol_index = 0;

//  Functions from previous module.
int isLetter(char ch);
int isNumber(char ch);
int isSymbol(char ch);
int isValid(char ch);

//  New function declarations.
void driver(int argc, const char * argv[]);
void getToken();
void parser();
void programParser();
void procedureParser();
void blockParser();
void statementParser();
void conditionParser();
void expressionParser();
void termParser();
void factorParser();
void printError();  //  Temperory function until error handeling is implemented.

int main(int argc, const char * argv[]) {
    
    /*
     
        ====
        TODO
        ====
     
        > Implement symbol table
        > Implement compiler driver (printToScreen())
        > Finish coding parser()
        > Implent code generation *inside* of parser() functions, as dictated by rubric
     
    
     */
    
    fp = fopen("test.txt", "r");
    
    parser();
    
    
//    driver(argc, argv);
    
    return 0;
    
}

// Wrapper function for the parsing logic.
//
void parser()
{
    
    fp = fopen("test.txt", "r");
    
    //  Symbol table instantiation. (Probaly going to be moved.)
    

    //  Initiate parsing by calling procedure parser function.
    programParser();
    
//    while ( !feof(fp) )   //  conditional to test token retrieval (bug testing)
//    
//        getToken();
    
}

void programParser()
{
    
    getToken();
    
    blockParser();
    
    if (*token != periodsym)
        
        printError();
    
    printf("\nPARSER COMPLETE\n");
    
}

void blockParser()
{
    
    if (*token == constsym)
    {
        
        do
        {
            
            getToken();
            
            if (*token != identsym)
                
                printError();
            
            getToken();
            
            if (*token != eqlsym)
                
                printError();
            
            getToken();
            
            if (*token != numbersym)
                
                printError();
            
        }
        while ( *token != commasym );
        
        
        if (*token != semicolonsym)
            
            printError();
        
        getToken();
        
    }
    
    if (*token == varsym)
    {
        
        do
        {
            getToken();
            
            if (*token != identsym)
                
                printError();
            
            getToken();

        }
        while ( *token != commasym );
        
        if (*token != semicolonsym)
            
            printError();
        
        getToken();
        
    }
    
    while (*token == procsym)
    {
        
        getToken();
        
        if (*token != identsym)
            
            printError();
        
        getToken();
        
        if (*token != semicolonsym)
            
            printError();
        
        getToken();
        
        blockParser();
        
        if (*token != semicolonsym)
            
            printError();
        
        getToken();
        
    }
    
    statementParser();

}

void statementParser()
{
    
    
    if (*token == identsym)
    {
        
        getToken();
        
        if (*token != becomessym)
            
            printError();
        
        getToken();
        
        expressionParser();
        
    }
    else if (*token == callsym)
    {
        
        getToken();
        
        if (*token != identsym)
            
            printError();
        
        getToken();
        
    }
    else if (*token == beginsym)
    {
        
        getToken();
        
        statementParser();
        
        while (*token == semicolonsym)
        {
            
            getToken();
            
            statementParser();
            
        }
        
        if (*token != endsym)
            
            printError();
        
        getToken();
        
        if (*token != endsym)
            
            printError();
        
    
    }
    else if (*token == ifsym)
    {
        
        getToken();
        
        conditionParser();
        
        if (*token != thensym)
            
            printError();
        
        getToken();
        
        statementParser();
        
    }
    else if (*token == whilesym)
    {
        
        getToken();
        
        conditionParser();
        
        if (*token != dosym)
            
            printError();
        
        getToken();
        
        statementParser();

    }
    
}

void conditionParser()
{
    
    if (*token == oddsym)
    {
        
        getToken();
        
        expressionParser();
        
    }
    else
    {
        
        expressionParser();
        
        if (*token != nulsym)   //  bug: change this conditional to check for relation symbols.

            printError();
        
        getToken();
        
        expressionParser();
        
    }
    
}

void expressionParser()
{
    
    if (*token == plussym || *token == minussym)
        
        getToken();
    
    termParser();
    
    while (*token == plussym || *token == minussym)
    {
        
        getToken();
        
        termParser();
        
    }
    
}

void termParser()
{
    
    factorParser();
    
    while (*token == multsym || *token == slashsym)
    {
        
        getToken();
        
        factorParser();
        
    }
    
}

void factorParser()
{
    
    if (*token == identsym)
        
        getToken();
    
    else if (*token == numbersym)
        
        getToken();

    else if (*token == lparentsym)
    {
        
        getToken();
        
        expressionParser();
        
        if (*token != rparentsym)
            
            printError();
        
        getToken();
        
    }
    else
        
        printError();
    
}

//
// Function to retrieve lexeme class from lexeme list output file.
//
void getToken()
{
    
    char str[12];
    char *ptr;
    
        fscanf(fp, "%s", str);

        int num = (int)strtol(str, &ptr, 10);
        
        if (num >= nulsym && num <= elsesym)    //  unecessary conditionals (bug testing)
        {
            
            *token = num;
            
            if ( num == identsym || num == numbersym )
            {
                
                if ( symbol_index < MAX_SYMBOL_TABLE_SIZE )
                {
                    
                    symbol_table[symbol_index].kind = num;
                    fscanf(fp, "%s", symbol_table[symbol_index++].name);
                    
                }
                else
                {
                    printf("overflow symbol table size bug\n");
                    
                }
                    
            }
            
        }
        else
        {
            printf("invalid symbol read bug\n");
        }
    
}

void driver(int argc, const char * argv[]) {
    
    int i;
    
    for ( i = 0; argc > 0; i++, argc-- )
    {
        
        const char* directive = argv[i];
        
        if ( strcmp(directive, "-l") )
            
            printf("Print the list of lexemes/tokens.\n");
        
        else if ( strcmp(directive, "-a") )
            
            printf("Print the generated assembly code.\n");
        
        else if ( strcmp(directive, "-v") )
            
            printf("Print virtual machine execution trace.\n");
        
    }
    
}

//
//  Determines whether char is a valid alpha character.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, returns 1 if true, 0 otherwise.
//
int isLetter(char ch)
{
   
   return ( ch >= 'a' && ch <= 'z' );
   
}

//
//  Determines whether char is a valid numerical character.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, returns 1 if true, 0 otherwise.
//
int isNumber(char ch)
{
   
   return ( ch >= '0' && ch <= '9' );
   
}

//
//  Determines whether char is a valid symbol character.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, returns 1 if true, 0 otherwise.
//
int isSymbol (char ch)
{
   
   return ( ch >= ':' && ch <= '>' )  || ( ch >= '(' && ch <= '/' );
   
}

//
//  Determines whether char is either a alpha, numerical, or symbol character.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, returns 1 if true, 0 otherwise.
//
int isValid(char ch)
{
   
   return ( isLetter(ch) || isNumber(ch) || isSymbol(ch) );
   
}

//  Temporary function to display message when error occurs. Needs logic to differentiate between errors.
void printError()
{
    
    printf("ERROR (%d) \n", *token);
    
}