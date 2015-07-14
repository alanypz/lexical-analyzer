Team Members: Alan Yepez, Justin MacKenzie

Our program can be compiled and run using the following files/commands

Files:
1) driver.c
2) input.txt
3) lexicalAnalyzer.h
4) parser.h
5) vm.h

Commands:
gcc driver.c
./a.out -l -a -v 
(Where -l, -a, and -v refer to the lexeme table, assembly code,-
and stack trace respectively)

If you have the necessary files and use the provided commands our program
will do the following.

1) Strip comments and read the given input file, creating a list of tokens

2) Parse the list of tokens, creating an intermediate code representation

3) Execute the assembly code through the pl/0 virtual machine

Each one of the listed steps writes its own output file which can be displayed
based on the commands listed earlier (-l,-a,-v) respectively.

*Note: If during any one of these steps we encounter an error, our program will
       halt and display the error message accordingly.