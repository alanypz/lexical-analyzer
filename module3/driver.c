//
//  parser.c
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
#define INPUT_FILE "lexemelist.txt"
#define VM_OUTPUT_FILE "mcode.txt"

#define MAX_SIZE 12
#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_CODE_LENGTH 500         //  From module 1.

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

typedef enum Kinds{
    cons = 1, vari = 2, proc = 3
}kind_type;

typedef struct {
    int kind;       // const = 1, var = 2, proc = 3
    char name[MAX_SIZE];  // name up to 11 chars
    int val;        // number (ASCII value)
    int level;      // L level
    int addr;       // M address
} symbol;

typedef struct {
    int op;
    int l;
    int m;
} instruction;

//  Global variable declaration.
FILE *ifp, *ofp = NULL;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
instruction vm_code[MAX_CODE_LENGTH];
int c = 0,      //  Index for the vm code.
    lex = 0,    //  Lexigraphical level.
    symi = 0,   //  Index for symbol table.
    token = -1, //  Token class.
    valid = 1;  //  Boolean. 1 = no errors, 0 = errors.

//  Function declarations.
void parser();
void program();
void procedure();
void block();
void statement();
void condition();
void expression();
void termParser();
void factor();
void error();

void record(int kind, char name[], int val, int level, int addr);  //  Write a new symbol to symbol table.
int find(char name[]);    //  Searches for an existing symbol in symbol table.
void translate(int op, int l, int m);   //  Conversion to VM language.

int main(int argc, const char * argv[]) {
    
    ifp = fopen(INPUT_FILE, "r");
    ofp = fopen(VM_OUTPUT_FILE, "w");
    
    parser();
    
    if (valid == 1)
        
        fprintf(ofp, "No errors, program is syntactically correct.\n");
    
    int i;
    
    for ( i = 0; i <= c; i ++ )

        fprintf(ofp, "%d %d %d\n", vm_code[i].op, vm_code[i].l, vm_code[i].m);
    
    fclose(ifp);
    fclose(ofp);
    
    return 0;
    
}

// Wrapper function for the parsing logic.
void parser()
{

    //  Initiate parsing by calling procedure parser function.
    program();
    
    
}

void program()
{
    
    fscanf(ifp, "%d", &token);
    
    translate(6, lex, 5);
    
    block();
    
    if (token != periodsym && valid)
        
        error(9);
    
    if (valid)
        
        translate(2, 0, 0);
    
}

void block()
{
    
    int kind, val;
    char name[MAX_SIZE];
    
    if (token == constsym)
    {
        
        do
        {
            
            getToken();
            
            if (token != identsym)
                
                error();
            
            getToken();
            
            if (token != eqlsym)
                
                error();
            
            getToken();
            
            if (token != numbersym)
                
                error();
            
        }
        while ( token != commasym );
        
        
        if (token != semicolonsym)
            
            error();
        
        getToken();
        
    }
    
    if (token == varsym)
    {
        
        do
        {
            getToken();
            
            if (token != identsym)
                
                error();
            
            getToken();

        }
        while ( token != commasym );
        
        if (token != semicolonsym)
            
            error();
        
        getToken();
        
    }
    
    while (token == procsym)
    {
        
        getToken();
        
        if (token != identsym)
            
            error();
        
        getToken();
        
        if (token != semicolonsym)
            
            error();
        
        getToken();
        
        block();
        
        if (token != semicolonsym)
            
            error();
        
        getToken();
        
    }
    
    statement();

}

void statement()
{
    
    
    if (token == identsym)
    {
        
        getToken();
        
        if (token != becomessym)
            
            error();
        
        getToken();
        
        expression();
        
    }
    else if (token == callsym)
    {
        
        getToken();
        
        if (token != identsym)
            
            error();
        
        getToken();
        
    }
    else if (token == beginsym)
    {
        
        getToken();
        
        statement();
        
        while (token == semicolonsym)
        {
            
            getToken();
            
            statement();
            
        }
        
        if (token != endsym)
            
            error();
        
        getToken();
        
        if (token != endsym)
            
            error();
    
    }
    else if (token == ifsym)
    {
        
        getToken();
        
        condition();
        
        if (token != thensym)
            
            error();
        
        getToken();
        
        statement();
        
    }
    else if (token == whilesym)
    {
        
        getToken();
        
        condition();
        
        if (token != dosym)
            
            error();
        
        getToken();
        
        statement();

    }
    
}

void condition()
{
    
    if (token == oddsym)
    {
        
        getToken();
        
        expression();
        
    }
    else
    {
        
        expression();
        
        if (token != nulsym)   //  bug: change this conditional to check for relation symbols.

            error();
        
        getToken();
        
        expression();
        
    }
    
}

void expression()
{
    
    if (token == plussym || token == minussym)
        
        getToken();
    
    termParser();
    
    while (token == plussym || token == minussym)
    {
        
        getToken();
        
        termParser();
        
    }
    
}

void termParser()
{
    
    factor();
    
    while (token == multsym || token == slashsym)
    {
        
        getToken();
        
        factor();
        
    }
    
}

void factor()
{
    
    if (token == identsym)
        
        getToken();
    
    else if (token == numbersym)
        
        getToken();

    else if (token == lparentsym)
    {
        
        getToken();
        
        expression();
        
        if (token != rparentsym)
            
            error();
        
        getToken();
        
    }
    else
        
        error();
    
}

void driver(int argc, const char * argv[])      //  TODO: Move logical to another .c.
{
    
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

void record(int kind, char name[], int val, int level, int addr)
{
    
    if ( find(name) == -1 )
    {
        
        if (kind == cons)
        {
            
            symbol_table[symi].kind = kind;
            strcpy(symbol_table[symi].name, name);
            symbol_table[symi].val = val;
            
        }
        
        else
        {
            
            symbol_table[symi].kind = kind;
            strcpy(symbol_table[symi].name, name);
            symbol_table[symi].level = level;
            symbol_table[symi].addr = addr;
            
        }
        
        symi++;
        
    }
    
}

int find(char name[])
{
    
    int i = 0;
    
    if( c != 0 )
    {
        
        while ( i != c )
        {
            
            if ( strcmp(symbol_table[i].name, name) == 0 )
            
                return i;
            
            else
                
                i++;
            
        }
        
    }
    
    return -1;
    
}

void translate(int op, int l, int m)
{
    
    if ( c <= MAX_CODE_LENGTH )
    {
        
        vm_code[c].op = op;
        vm_code[c].l = l;
        vm_code[c++].m = m;
        
    }
    else
        
        error(26);
    
}

//  Prints error message corresponding to an error code.
void error(int err)
{
    
    //  Mark code as not valid.
    valid = 0;
    
    switch (err)
    {
        
        case 1:
        {
            
            printf("Error 1: Use = instead of :=.\n");
            
            break;
            
        }
        case 2:
        {
            
            printf("Error 2: = must be followed by a number.\n");
            
            break;
            
        }
        case 3:
        {
            
            printf("Error 3: Identifier must be followed by =.\n");
            
            break;
            
        }
        case 4:
        {
            
            printf("Error 4: const, int, procedure must be followed by identifier.\n");
            
            break;
            
        }
        case 5:
        {
            
            printf("Error 5: Semicolon or comma missing.\n");
            
            break;
            
        }
        case 6:
        {
            
            printf("Error 6: Incorrect symbol after procedure declaration.\n");
            
            break;
            
        }
        case 7:
        {
            
            printf("Error 7: Statement expected.\n");
            
            break;
            
        }
        case 8:
        {
            
            printf("Error 8: Incorrect symbol after statement part in block.\n");
            
            break;
            
        }
        case 9:
        {
            
            printf("Error 9: Period expected.\n");
            
            break;
            
        }
        case 10:
        {
            
            printf("Error 10: Semicolon between statements missing.\n");
            
            break;
            
        }
        case 11:
        {
            
            printf("Error 11: Undeclared identifier.\n");
            
            break;
            
        }
        case 12:
        {
            
            printf("Error 12: Assignment to constant or procedure is not allowed.\n");
            
            break;
            
        }
        case 13:
        {
            
            printf("Error 13: Assignment operator expected.\n");
            
            break;
            
        }
        case 14:
        {
            
            printf("Error 14: call must be followed by an identifier.\n");
            
            break;
            
        }
        case 15:
        {
            
            printf("Error 15: Call of a constant or variable is meaningless.\n");
            
            break;
            
        }
        case 16:
        {
            
            printf("Error 16: then expected.\n");
            
            break;
            
        }
        case 17:
        {
            
            printf("Error 17: Semicolon or } expected.\n");
            
            break;
            
        }
        case 18:
        {
            
            printf("Error 18: do expected.\n");
            
            break;
            
        }
        case 19:
        {
            
            printf("Error 19: Incorrect symbol following statement.\n");
            
            break;
            
        }
        case 20:
        {
            
            printf("Error 20: Relational operator expected.\n");
            
            break;
            
        }
        case 21:
        {
            
            printf("Error 21: Expression must not contain a procedure identifier.\n");
            
            break;
            
        }
        case 22:
        {
            
            printf("Error 22: Right parenthesis missing.\n");
            
            break;
            
        }
        case 23:
        {
            
            printf("Error 23: The preceding factor cannot begin with this symbol.\n");
            
            break;
            
        }
        case 24:
        {
            
            printf("Error 24: An expression cannot begin with this symbol.\n");
            
            break;
            
        }
        case 25:
        {
            
            printf("Error 25: This number is too large.\n");
            
            break;
            
        }
        case 26:
        {
            
            printf("Error: Code size has been exceed.\n");
            
            break;
            
        }
        default:
            
            printf("Error: Error not vald=id.\n");    //bug
    
    }

}