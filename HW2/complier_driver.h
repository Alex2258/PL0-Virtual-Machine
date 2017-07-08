/* Header(s) & Define(s) */
#include <stdbool.h>
#ifndef PARSER_COMPLIER_DRIVER_H
#define PARSER_COMPLIER_DRIVER_H

/* Struct(s) */
typedef struct
{
    int l; /* Flag for Lexemes/Tokens */
    int a; /* Flag for Generated Assembly Code */
    int v; /* Flag for Virtual Machine Execution Trace */
}directives;

/* Global Variable(s) */
directives directive;

/* Function Declaration(s)/Prototype(s) */
void parseDirectives( int argc, char **argv );

/* Function(s) */
void parseDirectives( int argc, char **argv )
{
    /* Local Variable(s) */
    int i;
    bool flag = false;

    /* Check for each directive, set true where appropriate
       if an invalid command is given, print error to stdout and exit */
    for( i = 1; i < argc; i++ )
    {
        if(strcmp(argv[i], "-l") == 0)
            directive.l = 1;
        else if( strcmp(argv[i], "-a") == 0)
            directive.a = 1;
        else if( strcmp(argv[i], "-v") == 0)
            directive.v = 1;
        else
        {
            printf("\nInvalid Compiler Argument: (%s)\n Please use a valid argument", argv[i]);
            flag = true;
        }
    }

    /* Once all errors are printed, exit if error occurred */
    if(flag == true)
    {
        exit(-1);
    }
}/* END parseDirectives */

#endif /* compiler_driver.h */
