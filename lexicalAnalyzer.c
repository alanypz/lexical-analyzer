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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//  Constant declarations.

#define norw    15
#define imax    32767
#define cmax    11
#define nestmax 5
#define strmax  256

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
    
    int kind;  //Integer val of Token
    char name[12];
    int val;
    int level; //L
    int adr;   //M
    
}symTable;

//  Token-Struct definition
typedef struct tokenStruct{
    
    char tokenStr[11];
    struct tokenStruct* next;
}token;

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
FILE* cleanOutput = NULL;

//static FILE *ofp = NULL;

//  Function declaration.

char* initialize();
void lexical(char codeNoComments[]);
int wasDigit(token *head, char *code, int index);
int wasAlpha(token *head, char *code, int index);
int wasSymbol(token *head, char *code, int index);

int main(int argc, const char * argv[]) {
    
    //Declare and initialize variables
    cleanOutput = fopen(CLEAN_OUTPUT_FILE, "w");
    int i,j;
    //  Method call to read input.
    
    char *code = initialize();
    char codeNoComments[strlen(code)];
    memset(codeNoComments,'\0',strlen(code));
    
    printf("%s\n", code);
    
    
    for(i=0,j=0;i<strlen(code);i++)
    {
        if(code[i] == '/' && code[i+1] == '*')
        {
            i+=2;
            while(code[i] != '*' || code[i+1] != '/')
            {
                i++;  
            }
            i+=2;
        }
        
        codeNoComments[j++] = code[i];
    }
    
    
    lexical(codeNoComments);
    
    fprintf(cleanOutput,"%s\n",codeNoComments);
    
    //  Free allocated memory.
    free(code);
    
    //  Close file for writing
    fclose(cleanOutput);
    
    system("pause");
    return 0;
    
}

//
//  Reads input file (PL/0 code) to string and return char pointer.
//
//  @return
//      char*, pointer to the first character in the String.
//
char* initialize( )
{
    
    char *head = NULL;
    
    //  Create pointer to input file.
    
    FILE* ifp = fopen(INPUT_FILE, "r");
    
    //  Return null if file could not be read.
    
    if (!ifp)
    {
        
        printf("Error: File not found.\n");
        
        return head;
        
    }
    
    //  Determine the length of the input file.
    
    fseek(ifp, 0, SEEK_END);
    int len = (int)ftell(ifp);
    fseek(ifp, 0, SEEK_SET);
    
    //  Dynamically size string.
    
    head = (char*)calloc(len + 1, sizeof(char));
    char *index = head;
    char c;
    
    if ( head )
    {
        while ( (c = getc(ifp)) != EOF )
            
            *index++ = c;
    
        *index = '\0';
        
    }
    else
        
        printf("Error: Memory allocation failed.\n");
        
    return head;
    
}

void lexical(char *code)
{
    
    token *head = (token*)malloc(sizeof(token));
    head->next = NULL;
    
    int index = 0;
    char sym = '\0';
    
    while ( index < strlen(code) )
    {
        
        sym = code[index];
        
        if ( isdigit(sym) )
        {
            
            index = wasDigit(head, code, index);
            
        }
        else if ( isalpha(sym) )
        {
            
            index = wasAlpha(head, code, index);
            
        }
        else if( ispunct(sym) )
        {
            
            index = wasSymbol(head, code, index);
            
        }
        else
        {
            
            
        }
        
    }
    
}


int wasDigit(token *head, char code[], int index)
{
 
    int i = 0;
    
    token *temp = head;
    
    while (temp != NULL)
        
        temp = temp->next;
        
    temp = (token*)malloc(sizeof(token));
    temp->next = NULL;
    temp->tokenStr[i];
    
    while ( isdigit(code[++i]) )
    {
        temp->tokenStr[i] = code[i];
        
    }
    
    return index + i;
    
}

int wasAlpha(token *head, char* code, int index)
{
    int i;
    
    
    
    return index + i;
    
}

int wasSymbol(token *head, char* code, int index)
{
    int i;
    
    return index + i;
    
}




