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
#include "lexicalAnalyzer.h"
#include "parser.h"
#include "vm.h"

void printToScreen(char* filename, char* command, int argc, const char * argv[]);

int main(int argc, const char * argv[])
{
    
    char lexemes[2] = "-l";
    char assemblyCode[2] = "-a";
    char virtualMachine[2] = "-v";
    
    //  Execute program files.
    
    if (startLex() == 0)    //  Run lexicalAnalyzer.c
    {
        
        printToScreen("lexemetable.txt", lexemes, argc, argv);
        
        if (startParser() == 0)    //  Run parser.c
        {
            
            printToScreen("mcode.txt", assemblyCode, argc, argv);
            
            if (startVM() == 0)    //  Run vm.c
            {
            
                printToScreen("stacktrace.txt",virtualMachine, argc, argv);
                
            }
            else
            {
                
                printf("Error occurred in Virtual Machine.\n");
                
            }
            
        }
        else
        {
            
            printf("Error occurred in Parser.\n");
            
        }
        
    }
    else
    {
        
        printf("Error occurred in Lexical Analyzer.\n");
        
    }
    
    system("pause");
}

void printToScreen(char* filename, char* command, int argc, const char * argv[])
{


    char c;
    int i, found = 0;

    FILE *fp = fopen(filename,"r");

    for (i = 0; i < argc; i++)
    {

        found = (strcmp(argv[i], command) == 0) ? 1 : 0;

    }

    if (found == 1)

        break;
    
    while ((c = fgetc(fp)) != EOF)
    {
            
        putchar(c);
        
    }
    
    printf("\n\n");
    
    fclose(fp);
    
};
