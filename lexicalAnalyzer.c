//
//  lexicalAnalyzer.c
//  Lexical Analyzer
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

//  Constant declarations.

#define norw    15
#define imax    32767
#define cmax    11
#define nestmax 5
#define strmax  256

#define START_SIZE  256

//  I/O file names.

#define INPUT_FILE "input.txt"
#define CLEAN_OUTPUT_FILE "cleaninput.txt"
#define TABLE_OUTPUT_FILE "lexemetable.txt"
#define LIST_OUTPUT_FILE "lexemelist.txt"

//  Internal representation of PL\0 Tokens

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

//  Symbol table structure declaration.

typedef struct symbolTable{
    
    int kind;
    char name[10];
    int val;
    int level;
    int adr;
    
}symTable;

//  List of reserved word names.

char *word[]={
    "null", "begin", "call", "const", "do", "else", "end",
    "if", "odd", "procedure","read", "then", "var", "while", "write"
};

//  Internal representation of reserved words.

int wsym[]={
    nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym,
    oddsym, procsym, readsym, thensym, varsym, whilesym, writesym};

//  Special symbol array.

int ssym[256];

//  Global pointer to output file.

//static FILE *ofp = NULL;

//  Function declaration.

char* initialize();

int main(int argc, const char * argv[]) {
    
    //  Method call to read input.
    
    char *code = initialize();
    
    printf("%s", code);
    
    //  Free allocated memory.
    
    free(code);
    
    return 0;
    
}

//
//  Reads input file (PL/0 code) to string and return char pointer.
//
//  @return
//      char*, pointer to the first character in the String.
//
char* initialize() {
    
    char *head = (char *)malloc(START_SIZE);
    char *index = head;
    char c;
    
    FILE *ifp = fopen(INPUT_FILE, "r");
    
    if ( head )
    
        while ( (c = getc(ifp)) != EOF ) {
        
            if ( !index )
            
                head = (char *)realloc(head,START_SIZE);
        
            *index++ = c;
            
        }
    
    // /* Reallocating memory */
    // str = (char *) realloc(str, 25);
    // strcat(str, ".com");
    
    return head;
    
}
