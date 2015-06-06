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
char *word[] = {
    "null", "begin", "call", "const", "do", "else", "end",
    "if", "odd", "procedure","read", "then", "var", "while", "write"
};

//  Internal representation of reserved words.
int wsym[]={
    nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym,
    oddsym, procsym, readsym, thensym, varsym, whilesym, writesym
};

//  Special symbol array.
int ssym[256];

//  Global pointer to output file.
FILE *ofp = NULL;

//  Function declaration.
char* initialize();
char* clean(char *code);
void analyzer(char codeNoComments[]);
int wasDigit(token *head, char *code, int index);
int wasAlpha(token *head, char *code, int index);
int wasSymbol(token *head, char *code, int index);
token* createToken(token *head);

int main(int argc, const char * argv[]) {
    
    //  Method call to read input.
    char *code = initialize();
    
    //  Method to remove comments.
    char *cleaned = clean(code);
    
    //  Method call to parse tokens.
    analyzer(cleaned);
    
    //  Free allocated memory.
    free(cleaned);
    
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
    
    //  Continue appending to String until end of character is reached.
    if ( head )
    {
        while ( (c = getc(ifp)) != EOF )
            
            *index++ = c;
        
        *index = '\0';
        
    }
    else
        
        printf("Error: Memory allocation failed.\n");
    
    printf("%s\n\n",head);  //  bug printing
    
    return head;
    
}

//
//  Removes comments in code from input file.
//
//  @param *code
//      char, pointer to character array.
//  @return
//      char, pointer to the revised character array.
//
char* clean(char *code)
{
    
    //  Create and initalize temp char array.
    char *codeNoComments = (char*)calloc(strlen(code) + 1, sizeof(char));
    
    int i, j;
    
    //  Iterate through input string and only append content not in comments.
    for( i = 0, j = 0; i < strlen(code); i++ )
    {
        
        if( code[i] == '/' && code[i+1] == '*' )
        {
            
            i += 2;
            
            while( code[i] != '*' || code[i+1] != '/' )
            {
                
                i++;
                
            }
            
            i += 2;
            
        }
        
        codeNoComments[j++] = code[i];
        
    }
    
    //  Free original input array.
    free(code);
    
    //  Print to output file.
    
    FILE * ofp = fopen(CLEAN_OUTPUT_FILE, "w");
    printf("%s\n",codeNoComments);  //  bug printing
    fprintf(ofp,"%s\n",codeNoComments);
    fclose(ofp);
    
    return codeNoComments;
    
}

//
//  Takes in string and parses tokens.
//
//  @param *code
//      char, pointer to the char array.
//
void analyzer(char *code)
{
    
    //  Initialize head pointer of token struct linked list.
    token *head = (token*)malloc(sizeof(token));
    head->next = NULL;
    
    int index = 0;
    char sym = '\0';
    
    //  Iterate until index surpases input string.
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
            
            //  TODO: Add logic for invalid chars.
            
        }
        
        index++;    //  added to prevent infinite loops while bug testing
        
    }
    
}

//
//  Creates token when symbol detected as digit.
//
//  @param *head
//      token, pointer to token structure.
//  @param *code
//      char, pointer to character array.
//  @param index
//      int, the starting index.
//  @return
//      int, the updated index.
//
int wasDigit(token *head, char* code, int index)
{
    
    //  Method call to allocate token struct node.
    token *temp = createToken(head);
    
    int i = 0;
    
    //  TODO: Add logic to prevent out-of-bounds error.
    while ( isdigit(code[++i]) )
    {
        temp->tokenStr[i] = code[i];
        
    }
    
    return index + i;
    
}

//
//  Allocates memory for a new token struct node.
//
//  @param *head
//      token, pointer to token structure.
//  @return
//      token*, pointer to the newly created struct node.
//
token* createToken(token *head)
{
    
    token *last = head;
    
    //  Iterate to what is pointed to by last node.
    while (last != NULL)
        
        last = last->next;
    
    last = (token*)malloc(sizeof(token));
    last->next = NULL;
    last->tokenStr[0];
    
    return last;
    
}

//
//  Creates token when symbol detected as alpha character.
//
//  @param *head
//      token, pointer to token structure.
//  @param *code
//      char, pointer to character array.
//  @param index
//      int, the starting index.
//  @return
//      int, the updated index.
//
int wasAlpha(token *head, char* code, int index)
{
    
    //  Method call to allocate token struct node.
        token *temp = createToken(head);
    
    //  TODO: Switch statement/while loop logic.
    
    int i;
    
    return index + i;
    
}

//
//  Creates token when symbol detected.
//
//  @param *head
//      token, pointer to token structure.
//  @param *code
//      char, pointer to character array.
//  @param index
//      int, the starting index.
//  @return
//      int, the updated index.
//
int wasSymbol(token *head, char* code, int index)
{
    
    //  Method call to allocate token struct node.
    token *temp = createToken(head);
    
    //  TODO: Switch statement/while loop logic.
    
    int i;
    
    return index + i;
    
}




