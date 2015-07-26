//
//  parser.h
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

//  I/O file names.
#define INPUT_FILE_PARSER "lexemelist.txt"
#define VM_OUTPUT_FILE "mcode.txt"

#define MAX_SIZE_PARSER 12
#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_CODE_LENGTH 500         //  From module 1.

//  Internal representation of PL\0 token_parsers.
typedef enum token_Parser{
    nulsymP = 1, identsymP = 2, numbersymP = 3, plussymP = 4,
    minussymP = 5, multsymP = 6,  slashsymP = 7, oddsymP = 8,
    eqlsymP = 9, neqsymP = 10, lessymP = 11, leqsymP = 12,
    gtrsymP = 13, geqsymP = 14, lparentsymP = 15, rparentsymP = 16,
    commasymP = 17, semicolonsymP = 18, periodsymP = 19, becomessymP = 20,
    beginsymP = 21, endsymP = 22, ifsymP = 23, thensymP = 24,
    whilesymP = 25, dosymP = 26, callsymP = 27, constsymP = 28,
    varsymP = 29, procsymP = 30, writesymP = 31, readsymP = 32,
    elsesymP = 33
}token_type_parser;

typedef struct {
    int kind;       // const = 1, var = 2, proc = 3
    char name[MAX_SIZE_PARSER];  // name up to 11 chars
    int val;        // number (ASCII value)
    int level;      // L level
    int addr;       // M address
} symbol;

typedef struct {
    int op;
    int l;
    int m;
} instruction_parser;

//  Global variable declaration.
FILE *ifp, *ofp_parser = NULL;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
instruction_parser vm_code[MAX_CODE_LENGTH];
int c = 0,      //  Index for the vm code.
symi = 0,   //  Index for symbol table.
token_parser = -1, //  token_parser class.
valid = 1;  //  Boolean. 1 = no errors, 0 = errors.

//  Function declarations.
void parser();
void program(int lex);
void block(int lex);
void statement(int lex);
void condition(int lex);
void expression(int lex);
void term(int lex);
void factor(int lex);
void error();
void getToken();
void getString(char *str);
void record(int kind, char name[], int val, int level, int addr);  //  Write a new symbol to symbol table.
int find(char name[]);    //  Searches for an existing symbol in symbol table.
void emit(int op, int l, int m);   //  Conversion to VM language.

int startParser() {
    
    ifp = fopen(INPUT_FILE_PARSER, "r");
    ofp_parser = fopen(VM_OUTPUT_FILE, "w");
    
    if (!ifp)
    {
        
        printf("Error: File not found.\n");
        
        return 1;
        
        
    }
    
    parser();
    
    if (valid == 1)
        
        printf("No errors, program is syntactically correct.\n\n");
    
    int i;
    
    for ( i = 0; i < c; i ++ )
        
        fprintf(ofp_parser, "%d %d %d\n", vm_code[i].op, vm_code[i].l, vm_code[i].m);
    
    fclose(ifp);
    fclose(ofp_parser);
    
    return (valid == 1) ? 0 : 1;
    
}

// Wrapper function for the parsing logic.
void parser()       //  TODO: Maybe function should be comfined with main()?
{
    
    int lex = 0;
    //  Initiate parsing by calling procedure parser function.
    program(lex);
    
    //    emit(9, 0, 2);
    
}

void program(int lex)
{
    
    getToken( );
    
    block(lex);
    
    if ( token_parser == 1000 )
        
        printf("STUFF");
    
    if (token_parser != periodsymP && valid)
        
        error(9);
    
    if (valid)
        
        emit(9, 0, 2);
    
}

void block(int lex)    //  Alan
{
    
    int kind, val, j = 4;
    char name[MAX_SIZE_PARSER];
    int jump_to_block = c;
    emit(7, 0, c);
    
    if (token_parser == constsymP)
    {
        
        kind = constsymP;
        
        do
        {
            
            getToken( );
            
            if (token_parser != identsymP )
            {
                error(4);
                
                return;
                
            }
            else
            {
                
                getString(name);
                
            }
            
            getToken( );
            
            if (token_parser != eqlsymP)
            {
                
                if (token_parser == becomessymP)
                    
                    error(1);
                
                else
                    
                    error(3);
                
                return;
                
            }
            
            getToken( );
            
            if (token_parser != numbersymP)
            {
                
                error(2);
                
                return;
                
            }
            else
            {
                
                getToken( );
                
                val = token_parser;
                
            }
            
            record(kind, name, val, 0, 0);
            
            getToken( );

        }
        while (token_parser == commasymP);
        
        if (token_parser != semicolonsymP)
        {
            
            error(5);
            
            return;
            
        }
        
        getToken( );
        
    }
    
    if (token_parser == varsymP)
    {
                
        kind = varsymP;
        
        do
        {
            getToken( );
            
            if (token_parser != identsymP && valid)
            {
                error(4);
                
                return;
                
            }
            else
            {
                
                getString(name);
                
            }
            
            record(kind, name, 0,  lex, j);
            
            getToken( );
            
            j++;
            
        }
        while ( token_parser == commasymP );
        
        if (token_parser != semicolonsymP && valid)
        {
            
            error(5);
            
            return;
            
        }
        
        getToken( );

    }
    
    while (token_parser == procsymP)
    {
        
        kind = procsymP;
        
        getToken( );
        
        if (token_parser != identsymP && valid)
        {
            error(4);
            
            return;
            
        }
        else
        {
            
            getString(name);
            
        }
        
        record(kind, name, 0,  lex, c+1);
        
        getToken( );
        
        if (token_parser != semicolonsymP && valid)
        {
            
            error(6);
            
            return;
            
        }
        
        getToken( );
        
        block(lex + 1);
        
        if (token_parser != semicolonsymP && valid)
        {
            
            error(5);
            
            return;
            
        }
        
        getToken( );
        
        emit(2, 0, 0);  
        
    }

    vm_code[jump_to_block].m = c;   

    //emit(6, lex, j);
    emit(6, 0, j);

    statement(lex);
    
}

void statement(int lex)    //  Justin
{
    
    if (token_parser == identsymP)
    {
        
        char name[MAX_SIZE_PARSER];
        
        getString(name);
        
        int index = find(name);
        
        if (index == -1 && valid)
        {
            
            error(11);
            
            return;
            
        }
        
        if (symbol_table[index].kind != varsymP  && valid)
        {
            
            error(12);
            
            return;
            
        }
        
        getToken( );
        
        if (token_parser != becomessymP && valid)
        {
            
            error(13);
            
        }
        
        getToken( );
        
        expression(lex);

        int lex_diff = lex - symbol_table[index].level;
        emit(4, lex_diff, symbol_table[index].addr);
        
    }
    else if (token_parser == callsymP)
    {
        
        getToken( );
        
        if (token_parser != identsymP && valid)
        {
            
            error(14);
            
            return;
            
        }
        
        char name[MAX_SIZE_PARSER];
        
        getString(name );
        
        int index = find(name);
        
        if (index == -1  && valid)
        {
            
            error(11);
            
            return;
            
        }
        
        emit(5, lex -symbol_table[index].level, symbol_table[index].addr);
        
        // emit(6, lex, 4);
        
        getToken( );
        
    }
    else if (token_parser == beginsymP)
    {
        
        getToken( );
        
        statement(lex);
        
        while (token_parser == semicolonsymP)
        {
            
            getToken( );
            
            statement(lex);
            
        }
        
        if (token_parser != endsymP && valid)
        {
            
            error(19);
            
            return;
            
        }
        
        getToken( );
        
    }
    else if (token_parser == ifsymP)
    {
        
        getToken( );
        
        condition(lex);
        
        if (token_parser != thensymP && valid)
        {
            
            error(16);
            
            return;
        }
        
        else
        {
            
            getToken( );
            
        }
        
        int c1 = c;
        
        emit(8, 0, 0);
        
        statement( lex);
        
        if (token_parser == elsesymP)
        {
            
            getToken( );
            
            int c2 = c;
            
            emit(7, 0, 0);
            
            vm_code[c1].m = c;
            
            statement(lex);
            
            vm_code[c2].m = c;
            
        }
        
        else
        {
            
            vm_code[c1].m = c;
            
        }
        
    }
    if (token_parser == readsymP)
    {
        
        getToken( );
        
        if (token_parser != identsymP && valid)
        {
            
            error(19);
            
            return;
            
        }
        
        char name[12];
        
        getString(name );
        
        int position = find(name);
        
        if (position == -1  && valid)
        {
            
            error(11);
            
            return;
            
        }
        
        emit(9, 0, 1);
        
        int lex_diff = lex - symbol_table[position].level;
        emit(4, lex_diff, symbol_table[position].addr);
        
        getToken( );
        
    }
    
    if (token_parser == writesymP)
    {
        
        getToken( );
        
        if (token_parser != identsymP && valid)
        {
            
            error(19);
            
            return;
            
        }
        
        char name[12];
        
        getString(name);
        
        int position = find(name);
        
        if (position == -1  && valid)
        {
            
            error(11);
            
            return;
            
        }

        
        
        if (symbol_table[position].kind ==  varsymP)
        {    
            int lex_diff = lex - symbol_table[position].level;
            emit(3, lex_diff, symbol_table[position].addr);
        }
        else if (symbol_table[position].kind ==  constsymP)
            
            emit(1, 0, symbol_table[position].val);
        
        emit(9, 0, 0);
        
        getToken( );
        
    }
    else if (token_parser == whilesymP)
    {
        
        int c1 = c;
        
        getToken( );
        
        condition(lex);
        
        int c2 = c;
        
        emit(8, 0, 0);
        
        if (token_parser != dosymP && valid)
        {
            
            error(18);
            
            return;
            
        }
        
        else
        {
            
            getToken( );
            
        }
        
        statement(lex);
        
        emit(7, 0, c1);
        
        vm_code[c2].m = c;
        
    }
    
}

void condition(int lex)    //  Alan
{
    
    if (token_parser == oddsymP)
    {
        
        getToken( );
        
        expression(lex);
        
        emit(2, 0, 6);
        
    }
    else
    {
        
        expression(lex);
        
        int rational = token_parser;
        
        if ((token_parser < eqlsymP || token_parser > geqsymP) && valid)
        {
            
            error(20);
            
            return;
            
        }
        
        getToken( );
        
        expression(lex);
        
        emit(2, 0, rational - 1);
        
    }
    
}

void expression(int lex) //    Justin
{
    
    int operation;
    
    if (token_parser == plussymP || token_parser == minussymP)
    {
        
        operation = token_parser;
        
        getToken( );
        
        term(lex);
        
        if (operation == minussymP)
        {
            emit(2, 0, 1);
        }
        
    }
    else
        
        term(lex);
    
    
    while (token_parser == plussymP || token_parser == minussymP)
    {
        
        operation = token_parser;
        
        getToken( );
        
        term(lex);
        
        if (operation == plussymP)
        {
            
            emit(2, 0, 2);
            
        }
        else
        {
            
            emit(2, 0, 3);
            
        }
        
        
    }
    
}

void term(int lex)   // Alan
{
    
    int operation = 0;
    
    factor(lex);
    
    while (token_parser == multsymP || token_parser == slashsymP)
    {
        
        operation = token_parser;
        
        getToken( );
        
        factor(lex);
        
        if (operation == multsymP)
        {
            
            emit(2, 0, 4);
            
        }
        else
        {
            
            emit(2, 0, 5);
            
        }
        
    }
    
}

void factor(int lex) //    Justin
{
    
    char name[12];
    
    int index, val;
    
    if (token_parser == identsymP)
    {
        
        getString(name);
        
        index = find(name);
        
        if (symbol_table[index].kind == constsymP)
        {
            
            emit(1, 0, symbol_table[index].val);
            
        }
        else if (symbol_table[index].kind == varsymP)
        {

            int lex_diff = lex - symbol_table[index].level;   
            emit(3, lex_diff, symbol_table[index].addr);
            
        }
        else if (symbol_table[index].kind == procsymP)
        {
            
            error(21);
            
            return;
            
            
        }
        else
        {
            
            error(11);
            
            return;
            
        }
        
        getToken( );
        
    }
    
    else if (token_parser == numbersymP)
    {
        
        getToken( );
        
        val = token_parser;
        
        emit(1, 0, val);
        
        getToken( );
        
    }
    
    else if (token_parser == lparentsymP)
    {
        
        getToken( );
        
        expression(lex);
        
        if (token_parser != rparentsymP && valid)
        {
            
            error(22);
            
            return;
            
        }
        
        getToken( );
        
    }
    
    else
    {
        
        error(19);
        
        return;
        
    }
    
}

void driver(int argc, const char * argv[])      //  TODO: Move logical to another .c?
{
    
    int i;
    
    for ( i = 0; argc > 0; i++, argc-- )
    {
        
        const char* directive = argv[i];
        
        if ( strcmp(directive, "-l") )
            
            printf("Print the list of lexemes/token_parsers.\n");
        
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
        
        if (kind == constsymP)
        {
            
            symbol_table[symi].kind = constsymP;
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
    
    if( symi != 0 )
    {
        
        while ( i < symi )
        {
            
            if ( strcmp(symbol_table[i].name, name) == 0 )
                
                return i;
            
            else
                
                i++;
            
        }
        
    }
    
    return -1;
    
}

void emit(int op, int l, int m)
{
    
    if ( c <= MAX_CODE_LENGTH )
    {
        
        vm_code[c].op = op;
        vm_code[c].l = l;
        vm_code[c].m = m;
        
        c++;
        
    }
    else
        
        error(26);
    
}

void getToken()
{
    
    
    
    if (!feof(ifp) )
    {
        
        fscanf(ifp, "%d", &token_parser);
        
    }
    else
        
        error(27);
    
}

void getString(char *str)
{
    
    if (!feof(ifp))
        
    
        fscanf(ifp, "%s", str);
    
    else
        
        error(27);
    
}

//  Prints error message corresponding to an error code.
void error(int err)
{
    
    //  Mark code as not valid.
    valid = 0;

    // printf("ERROR TOKEN: %d\n", token_parser );
    // getToken();
    // printf("NEXT TOKEN: %d\n", token_parser );
    
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
        case 27:
        {
            
            printf("Error: End of file reached.\n");
            
            exit(1);
            
        }
        default:
            
            printf("Error: Error not vald=id.\n");    //bug
            
    }
    
    exit(1);
    
}
