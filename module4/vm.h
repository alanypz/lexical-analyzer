#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

//Struct for instructions
typedef struct instructionFields{
    int op;  // opcode
    int  l;  // L
    int  m;  // M
    char instructionName[2];
}instruction;

//Struct for stack
typedef struct registerFileRegisters{
    int sp;
    int bp;
    int pc;
    instruction ir;
}registerFile;

//List of function prototypes
int printAssembly(FILE* ptr, instruction* code);
FILE* fileRead(char* fileName, char mode);
int base(int l, int base, int stack[]);
void doIt(FILE* ptr);
int getInstructionCount(FILE* ptr);
void initializeInstructionArray(instruction *code, FILE* ptr);
char* getInstructionName(int op);
void performInstruction(instruction* code, int* stack, registerFile cpuRegisters, int maxPc);
void printInstruction(instruction currentInstruction, int pc);
void printPcBpSp(registerFile rf);
void printStack(int* stack, int stackSize, int* bp);

//Globals
FILE* optr = NULL;

//Temporary hard-coded main function
int startVM()
{
    //Declare and initialize file pointers
    FILE *fptr = fileRead("mcode.txt", 'r');
    optr = fileRead("stacktrace.txt", 'w');
    
    //Function to generate the assembly pm/0 code
    doIt(fptr);
    
    //Close the files
    fclose(fptr);
    fclose(optr);
    
    return 0;
}

/*
 *Function to print the assembly code
 *generated to the screen
 *Input: FILE pointer of the input file, pointer to the instruction array
 *Output: VOID
 */
int printAssembly(FILE* ptr, instruction* code)
{
    //Declare and initialize variables
    int i;
    ptr = fileRead("mcode.txt", 'r');
    int maxInstructionCount = getInstructionCount(ptr);
    fprintf(optr,"Line %1sOP %3sL %2sM\n", "", "", "");
    
    for(i=0;i<maxInstructionCount;i++)
    {
        fprintf(optr,"%2d %6s %3d   %-d\n", i, code[i].instructionName, code[i].l, code[i].m);
    }
    
    fprintf(optr,"\n%25s", "pc");
    fprintf(optr,"\tbp");
    fprintf(optr,"\tsp");
    fprintf(optr,"\tstack\n");
    fprintf(optr,"Initial values\t %5s 0\t1\t0\n", "");
    
    return maxInstructionCount;
}

/*
 *Function to read a given filename
 *Additionally will check for FILENOTFOUNDEXCEPTION
 *Input: String of the filename
 *Output: File pointer to input filename
 */
FILE* fileRead(char* fileName, char mode)
{
    FILE* fp = NULL;
    if(mode == 'r')
    {
        fp = fopen(fileName, "r");
    }
    else
    {
        fp = fopen(fileName, "w");
    }
    
    if(fp == NULL)
    {
        printf("Could not open file\n");
        return;
    }
    return fp;
}

/**********************************************/
/*		Find base L levels down  		 */
/*							           */
/**********************************************/

int base(int l, int base, int stack[]) // l stand for L in the instruction format
{
    int b1; //find base L levels down
    b1 = base;
    while (l > 0)
    {
        b1 = stack[b1 + 1];
        l--;
    }
    return b1;
}

/*
 *Function that runs the PL/0 Virtual Machine, generates
 *corresponding assembly code and prints it as well as
 *current stack values to the screen
 *Input: FILE pointer to the input file
 *Ouput: VOID
 */
void doIt(FILE* ptr)
{
    //Declare and initialize variables
    int stack[MAX_STACK_HEIGHT];
    instruction code[MAX_CODE_LENGTH];
    registerFile cpuRegisters;
    int maxPc;
    
    //Initialize instruction array
    initializeInstructionArray(code, ptr);
    
    //Function to print the assembly instructions to the screen
    maxPc = printAssembly(ptr, code);
    
    //Execute instructions
    performInstruction(code, stack, cpuRegisters, maxPc);
    
    return;
}

/*
 *Function to get the total
 *count of instructions in the
 *input file for printing
 *Input: FILE pointer to input file
 *Output: Integer representing the instruction count
 */
int getInstructionCount(FILE* ptr)
{
    char buffer[60];
    int instructionCount = 0;
    
    while(fgets(buffer, 60, ptr) != NULL)
    {
        ++instructionCount;
    }
    return instructionCount;
}

/*
 *Function to intialize the instruction array with
 *data from the input file
 *Input: pointer to the instruction array, FILE pointer of the input file
 *Output: VOID
 */
void initializeInstructionArray(instruction* code, FILE* ptr)
{
    int maxInstructions = getInstructionCount(ptr);
    int i;
    ptr = fileRead("mcode.txt", 'r');
    
    for(i=0; i<maxInstructions; i++)
    {
        fscanf(ptr, "%d %d %d", &code[i].op, &code[i].l, &code[i].m);
        strcpy(code[i].instructionName, getInstructionName(code[i].op));
    }
    
    return;
}

/*
 *Function to return the name of the instruction based on its op-code
 *Input: Integer op code
 *Output: String name of the instruction
 */
char* getInstructionName(int op)
{
    switch(op){
        case 1 :
            return "lit";
        case 2 :
            return "opr";
        case 3 :
            return "lod";
        case 4 :
            return "sto";
        case 5 :
            return "cal";
        case 6 :
            return "inc";
        case 7 :
            return "jmp";
        case 8 :
            return "jpc";
        case 9 :
            return "sio";
        default:
            break;
    }
    
    return "\0";
}

/*
 *Function to execute the assembly instructions by a switch statement
 *Instruction execution depends on the various values of the instruction structure
 *This function also calls a few helper printing functions
 *Input: Instruction array, stack array, cpu registers, the maximum instruction count
 *Output: VOID
 */
void performInstruction(instruction* code, int* stack, registerFile cpuRegisters, int maxPc)
{
    //Declare and initialize variables
    cpuRegisters.bp = 1;
    cpuRegisters.sp = 0;
    cpuRegisters.pc = 0;
    int halt = 0;
    int basePointers[3];
    
    int i;
    for(i=0; i<3; i++)
    {
        basePointers[i] = 0;
    }
    i=0;
    
    
    while(cpuRegisters.bp > 0 && cpuRegisters.pc < maxPc)
    {
        if(halt == 1)
        {
            halt = 0;
            return;
            //system("pause");
        }
        
        cpuRegisters.ir = code[cpuRegisters.pc];
        
        if(!(cpuRegisters.ir.op == 9 && cpuRegisters.ir.m == 1))
            printInstruction(cpuRegisters.ir, cpuRegisters.pc);
        
        switch(cpuRegisters.ir.op){
            case 1 :
                //lit
                cpuRegisters.pc+=1;
                cpuRegisters.sp += 1;
                stack[cpuRegisters.sp] = cpuRegisters.ir.m;
                
                break;
            case 2 :
                //opr
                switch(cpuRegisters.ir.m){
                    case 0 :
                        //RET
                        cpuRegisters.sp = cpuRegisters.bp - 1;
                        cpuRegisters.pc = stack[cpuRegisters.sp + 4] - 1;
                        cpuRegisters.bp = stack[cpuRegisters.sp + 3];
                        
                        --i;
                        basePointers[i] = 0;
                        
                        break;
                        
                    case 1 :
                        //NEG
                        stack[cpuRegisters.sp]*=-1;
                        break;
                        
                    case 2 :
                        //ADD
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] += stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 3 :
                        //SUB
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] -= stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 4 :
                        //MUL
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] *= stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 5 :
                        //DIV
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp]/stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 6 :
                        //ODD
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp] % 2;
                        break;
                        
                    case 7 :
                        //MOD
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp] % stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 8 :
                        //EQL
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp] == stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 9 :
                        //NEQ
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp] != stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 10:
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp] < stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 11:
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp] <= stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 12:
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp] > stack[cpuRegisters.sp + 1];
                        break;
                        
                    case 13:
                        cpuRegisters.sp -= 1;
                        stack[cpuRegisters.sp] = stack[cpuRegisters.sp] >= stack[cpuRegisters.sp + 1];
                        break;
                        
                    default :
                        break;
                }
                cpuRegisters.pc+=1;
                break;
            case 3 :
                //lod
                cpuRegisters.sp += 1;
                stack[cpuRegisters.sp] = stack[base(cpuRegisters.ir.l, cpuRegisters.bp, stack) + cpuRegisters.ir.m];
                cpuRegisters.pc+=1;
                break;
            case 4 :
                //sto
                stack[base(cpuRegisters.ir.l, cpuRegisters.bp, stack) + cpuRegisters.ir.m] = stack[cpuRegisters.sp];
                cpuRegisters.sp -= 1;
                cpuRegisters.pc+=1;
                
                break;
            case 5 :
                //cal
                
                //Function return value
                stack[cpuRegisters.sp + 1] = 0;
                
                //Static link encapsulating base pointer
                stack[cpuRegisters.sp + 2] = base(cpuRegisters.ir.l, cpuRegisters.bp, stack);
                
                //Dynamic link (pointer to previous frame)
                stack[cpuRegisters.sp + 3] = cpuRegisters.bp;
                
                //Return address
                stack[cpuRegisters.sp + 4] = cpuRegisters.pc + 1;
                
                //Current base pointer
                cpuRegisters.bp = cpuRegisters.sp + 1;
                basePointers[i] = cpuRegisters.sp + 1;
                ++i;
                
                //Program counter overwritten
                cpuRegisters.pc = cpuRegisters.ir.m;
                
                break;
            case 6 :
                //inc
                cpuRegisters.sp += cpuRegisters.ir.m;
                cpuRegisters.pc+=1;
                
                break;
            case 7 :
                //jmp
                cpuRegisters.pc = cpuRegisters.ir.m;
                break;
            case 8 :
                //jpc
                if(stack[cpuRegisters.sp] == 0)
                {
                    cpuRegisters.pc = cpuRegisters.ir.m;
                }
                else
                {
                    cpuRegisters.pc+=1;
                }
                cpuRegisters.sp -= 1;
                break;
            case 9 :
                //sio
                switch(cpuRegisters.ir.m){
                        
                    case 0 :
                        //printf("%d", stack[cpuRegisters.sp]);
                        cpuRegisters.sp -= 1;
                        break;
                        
                    case 1 :
                        cpuRegisters.sp += 1;
                        //printf("Enter value to push onto the stack: ");
                        //scanf("%d", &stack[cpuRegisters.sp]);
                        break;
                        
                    case 2 :
                        halt = 1;
                        break;
                }
                
                cpuRegisters.pc+=1;
                break;
            default:
                break;
        }
        
        if(cpuRegisters.ir.op == 9 && cpuRegisters.ir.m == 1)
        {
            printf("Enter value to push onto the stack: ");
            scanf("%d", &stack[cpuRegisters.sp]);
            printInstruction(cpuRegisters.ir, cpuRegisters.pc - 1);
        }
        
        printPcBpSp(cpuRegisters);
        printStack(stack, cpuRegisters.sp, basePointers);
        
        if(cpuRegisters.ir.op == 9 && cpuRegisters.ir.m == 0)
        {
            printf("The value popped from the stack was %d\n", stack[cpuRegisters.sp + 1]);
        }
        
    }
}

/*
 *Helper print function used to print the current instruciton
 *Input: current instruction, current program counter
 *Output: VOID
 */
void printInstruction(instruction currentInstruction, int pc)
{
    fprintf(optr,"%2d %6s %3d\t %2d    ", pc, currentInstruction.instructionName,
            currentInstruction.l, currentInstruction.m);
}

/*
 *Helper print function used to print the current values of Pc Bp and Sp
 *Input: cpu registers
 *Output: VOID
 */
void printPcBpSp(registerFile rf)
{
    fprintf(optr,"%-8d %-7d %-5d", rf.pc, rf.bp, rf.sp);
}

/*
 *Helper print function used to print the current instruciton
 *Input: stack, last element of the stack to be printed, array of base pointer locations
 *Output: VOID
 */
void printStack(int* stack, int stackSize, int* bp)
{
    int i;
    fprintf(optr,"%3s", "");
    for(i=1; i<=stackSize; i++)
    {
        if(i==bp[0] || i==bp[1] || i==bp[2])
        {
            fprintf(optr,"| ");
        }
        
        fprintf(optr,"%d ", stack[i]);
    }
    
    fprintf(optr,"\n");
    return;
}








