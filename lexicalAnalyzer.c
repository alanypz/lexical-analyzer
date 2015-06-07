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
#define MAX_SIZE 11
#define MAX_NUMS 5

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

typedef enum StateLabels{
    init, firstState, stringState, numberState, symbolState, beginState, cState, callState, constState,
    ifState, procedureState, readState, whileState, writeState
    
}state;


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
    
    int class;
    char* lexeme;
    char* tokenStr;
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
void tokenize(char * code);
int isLetter(char ch);
int isNumber(char ch);
int isSymbol(char ch);
int isValid(char ch);

int main(int argc, const char * argv[]) {
    
//    printf("Val of char is %d.\n\n", (int)strlen(""));  //  bug testing
    
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

void tokenize(char * code)
{

    int i = 0, j = 1;
    char ch;
    int state = init;
    
    token *head = NULL;
    token *this = NULL;
    
    this = createToken(head);
    state = firstState;
    
    while ( i < strlen(code) )
    {
        
        ch = code[i++];
        
        //  TODO: Insert classes into switches.
    
        switch ( state ) {
                
            case firstState:    //  First state for new lexeme.
            {
                
                if ( !isValid(ch) )
                {
                    
                    break;
                    
                }
                else if ( ch == 'b' )
                {
                    
                    state = beginState;
                    
                }
                else if ( ch == 'c' )
                {
                    
                    state = cState;
                    
                }
                else if ( ch == 'i' )
                {
                    
                    state = ifState;
                    
                }
                else if ( ch == 'p' )
                {
                    
                    state = procedureState;
                    
                }
                else if ( ch == 'r' )
                {
                    
                    state = readState;
                    
                }
                else if ( ch == 'w' )
                {
                    
                    state = writeState;
                    
                }
                else if ( isLetter(ch) )
                {
                    
                    state = stringState;
                    
                }
                else if ( isNumber(ch) )
                {
                    
                    state = numberState;
                    
                }
                else if ( isSymbol(ch) )
                {
                        
                    state = symbolState;
                        
                }
                else
                {
                    
                    continue;
                    
                }
                
                this->lexeme[strlen(this->lexeme)] = ch;
                j++;
                
                break;
                
            }
            case stringState:
            {
                
                if ( !isValid(ch) )
                {
                    
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    
                    break;
                    
                }
                else if ( isSymbol(ch) )
                {
                    
                    this = createToken(head);
                    state = symbolState;
                    j = 1;
                    
                    break;
                    
                }
                
                this->lexeme[strlen(this->lexeme)] = ch;
                j++;
                
                if ( strlen(this->lexeme) > MAX_SIZE )
                    
                    //  TODO: Figure out how to handle this error.
                    printf("Error: Exceeded maximum identifier size.");
                
                break;
                
            }
            case numberState:
            {
                
                if ( !isValid(ch) )
                {
                    
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    
                    break;
                    
                }
                else if ( !isNumber(ch) )
                {
                    //  TODO: Display some kind of error, per the rubric.
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    i--;
                    
                    break;
                    
                }
                
                this->lexeme[strlen(this->lexeme)] = ch;
                j++;
                
                if ( strlen(this->lexeme) > MAX_NUMS )
                    
                    //  TODO: Figure out how to handle this error.
                    printf("Error: Exceeded maximum number size.");
                
                break;
                
            }
            case symbolState:
            {
                
                if ( !isValid(ch) )
                {
                
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    
                    break;
                    
                }
                else if ( !isSymbol(ch) )
                {
                    
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    i--;
                    
                }
                else if ( ch != '>' || ch != '=' )
                {
                    
                    //  TODO:   Figure out to handle this error.
                    
                    
                }
                
                this->lexeme[strlen(this->lexeme)] = ch;
                j++;
                
                break;
                
            }
            case beginState:
            {
                
                if ( !isValid(ch) )
                {
                    //  TODO: Signify that class is "begin." Check next char too.
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    
                    break;
                    
                }
                else if ( isSymbol(ch) )
                {
                    
                    this = createToken(head);
                    state = firstState;
                    i--;
                    j = 0;
                    
                    break;
                    
                }
                
                this->lexeme[strlen(this->lexeme)] = ch;
                
                char* str = "begin";
                
                if ( ch == str[j++] )
                {
                    
                    if ( j == 4 && ch == 'n' )
                    {
                        
                        this = createToken(head);
                        state = firstState;
                        j = 0;
                        
                        break;
                        
                    }

                    
                }
                else if ( isLetter(ch) || isNumber(ch) )
                {
                    
                    state = stringState;
                    
                }
                
                j++;
                
                break;
                
            }
            case cState:
            {
                
                if ( !isValid(ch) )
                {
                    
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    
                    break;
                    
                }
                else if ( isSymbol(ch) )
                {
                    
                    this = createToken(head);
                    state = firstState;
                    i--;
                    j = 0;
                    
                    break;
                    
                }
                
                char* str1 = "call";
                char* str2 = "const";
                
                this->lexeme[strlen(this->lexeme)] = ch;
                
                if ( ch == str1[j] )
                {
                    
                    state = callState;
                    
                }
                else if ( ch == str2[j] )
                {
        
                    state = constState;
                
                }
                else if ( isLetter(ch) || isNumber(ch) )
                {
                    
                    state = stringState;
                    
                }
                
                j++;

                break;
                
            }
            case callState:
            {
                
                if ( !isValid(ch) )
                {
                    
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    
                    break;
                    
                }
                
                char* str = "call";
                
                this->lexeme[strlen(this->lexeme)] = ch;
                
                if ( ch == str[j++] )
                {
                    
                    if ( j == 4 && ch == 'l' )
                    {
                        //  TODO: Signify that class is "call." Check next char too.
                        this = createToken(head);
                        state = callState;
                        j = 0;
                        
                    }
                    
                }
                
                break;
                
            }
            case constState:
            {
                
                if ( !isValid(ch) )
                {
                    
                    this = createToken(head);
                    state = firstState;
                    j = 0;
                    
                    break;
                    
                }
                
                char* str = "const";
                
                this->lexeme[strlen(this->lexeme)] = ch;
                
                if ( ch == str[j++] )
                {
                    
                    if ( j == 5 && ch == 't' )
                    {
                        //  TODO: Signify that class is "const." Check next char too.
                        this = createToken(head);
                        state = constState;
                        j = 0;
                        
                    }
                    
                    else
                        
                        j++;
                    
                }
                
                break;
                
            }
            case ifState:
            {
                
                
                break;
                
            }
            case procedureState:
            {
                
                
                break;
                
            }
            case readState:
            {
                
                
                break;
                
            }
            case whileState:
            {
                
                
                break;
                
            }
            case writeState:
            {
                
                
                break;
                
            }
   
            default:
            {
                
                printf("Error: Could not find case for character.\n");
                
                break;
                
            }
                
                
                
        }
    
    
    
    
    
    
    
    
    }
    
    
    
}

int isLetter(char ch)
{
    
    return ( ch >= 'a' && ch <= 'z' );
    
}

int isNumber(char ch)
{
    
    return ( ch >= '0' && ch <= '9' );
    
}

int isSymbol (char ch)
{
    
    return ( ch >= ':' && ch <= '>' )  || ( ch >= '*' && ch <= '/' );
    
}

int isValid(char ch)
{
    
    return ( isLetter(ch) || isNumber(ch) || isSymbol(ch) );
    
}


//
//  Takes in string and parses tokens.
//
//  @param *code
//      char, pointer to the char array.
//
void analyzer(char *code)
{
    
    return; //  bug testing
    
    //  Initialize head pointer of token struct linked list.
    token *head = (token*)malloc(sizeof(token));
    head->next = NULL;
    head->class = -1;
    
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
    
    //  Max length of digit
    int maxLength = 5;
    
    //  Method call to allocate token struct node.
    token *temp = createToken(head);
    
    int i = 0;
    
    while ( isdigit(code[++i]) )
    {
        
        if ( strlen(temp->tokenStr) >= maxLength )
        {
            
            maxLength *= 2;
            temp->tokenStr = realloc( temp->tokenStr, maxLength );
            
        }
        
        temp->tokenStr[i] = code[i];
        
    }
    
    return index + i;
    
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
    
    //  Max string length
    int maxLength = 11;
    
    //  Method call to allocate token struct node.
    token *temp = createToken(head);
    
    int i;
    
    //  TODO: Add logic to prevent out-of-bounds error.
    while ( isalpha(code[++i]) )
    {
        
        if( strlen(temp->tokenStr) >= maxLength )
        {
            
            maxLength *= 2;
            temp->tokenStr = realloc(temp->tokenStr, maxLength);
            
        }
        
        temp->tokenStr[i] = code[i];
        
    }
    
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
    
    int maxLength = 2;
    
    //  Method call to allocate token struct node.
    token *temp = createToken(head);
    
    int i = 0;
    
    //  TODO: Switch statement/while loop logic.
    while ( ispunct(code[++i]) )
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
//  @param len
//      int, starting size of the String.
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
    last->lexeme = "";
    last->class = -1;
    
    return last;
    
}




