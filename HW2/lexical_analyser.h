/* Header(s) & Define(s) */
#ifndef PARSER_LEXICAL_ANALYSER_H
#define PARSER_LEXICAL_ANALYSER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID_LEN 11
#define MAX_NUM_LEN 5
#define MAX_BUFFER_LEN 10000

/* Global Variable(s) */
int loop_counter;
int temp_token_counter = 0;

char buffer[MAX_BUFFER_LEN]; /* Buffer for Token Reading */
char temp[MAX_BUFFER_LEN]; /* Additional Buffer for Token Reading if needed */

/* Function Declaration(s) & Prototype(s) */
int isSpecialSymbol(char);

/*Function(s) */
int detect_error(char *s)
{
    int i;
    int len = (int)strlen(s);

    //Check for single colon token.
    if(len == 1 && s[0] == ':' )
    {
        printf( "Error: Invalid Symbol: :\n" );
        return 1;
    }

    //Check for invalid symbol.
    for( i = 0; i < len; i++ )
    {

        if(!isalnum(s[i]) && !isSpecialSymbol(s[i]))
        {
            printf("Error: (%c) is an invalid Symbol\n", s[i]);
            return 1;
        }
    }

    //Single letter & single number accepted, so if s > 1.
    if( len > 1 ){

        //Check identifier length.
        if( isalpha( s[0] ) ){
            for( i = 1; i < len; i++ ){
                if( i > MAX_ID_LEN ){
                    printf( "Error: Variable name: %s is too long.\n", s );
                    return 1;
                }
            }
        }

        if( isdigit( s[0] ) ){

            //Check invalid identifier (starts with number).
            for( i = 1; i < len; i++ ){

                if( isalpha( s[i] ) ){
                    printf( "Error: Variable name: %s begins with a number.\n", s );
                    return 1;
                }

            }

            //Check number length.
            if( i > MAX_NUM_LEN ){
                printf( "Error: Number %s is too long.\n", s );
                return 1;
            }

        }

    }

    return 0;
}/* END detect_error */

/*Return 1 if Special Symbol, 0 otherwise */
int isSpecialSymbol(char c)
{

    if( c == '+' )
        return 1;
    else if( c == '-' )
        return 1;
    else if( c == '*' )
        return 1;
    else if( c == '/' )
        return 1;
    else if( c == '(' )
        return 1;
    else if( c == ')' )
        return 1;
    else if( c == '=' )
        return 1;
    else if( c == ',' )
        return 1;
    else if( c == '.' )
        return 1;
    else if( c == '<' )
        return 1;
    else if( c == '>' )
        return 1;
    else if( c == ';' )
        return 1;
    else if( c == ':' )
        return 1;

    return 0;

}/* END isSpecialSymbol */

int isReservedWord(char* s)
{

    if( !strcmp( s, "const" ) )
        return 28;
    else if( !strcmp( s, "var" ) )
        return 29;
    else if( !strcmp( s, "procedure" ) )
        return 30;
    else if( !strcmp( s, "call" ) )
        return 27;
    else if( !strcmp( s, "begin" ) )
        return 21;
    else if( !strcmp( s, "end" ) )
        return 22;
    else if( !strcmp( s, "if" ) )
        return 23;
    else if( !strcmp( s, "then" ) )
        return 24;
    else if( !strcmp( s, "else" ) )
        return 33;
    else if( !strcmp( s, "while" ) )
        return 25;
    else if( !strcmp( s, "do" ) )
        return 26;
    else if( !strcmp( s, "read" ) )
        return 32;
    else if( !strcmp( s, "write" ) )
        return 31;

    return -1;

}/* END isReservedWord */

int isOtherValid(char *s)
{

    if(!strcmp( s, "null" ))
        return 1;
    else if(!strcmp( s, "+" ))
        return 4;
    else if(!strcmp( s, "-" ))
        return 5;
    else if(!strcmp( s, "*" ))
        return 6;
    else if(!strcmp( s, "/" ))
        return 7;
    else if(!strcmp( s, "<=" ))
        return 12;
    else if(!strcmp( s, ">=" ))
        return 14;
    else if(!strcmp( s, "(" ))
        return 15;
    else if(!strcmp( s, ")" ))
        return 16;
    else if(!strcmp( s, "," ))
        return 17;
    else if(!strcmp( s, ";" ))
        return 18;
    else if(!strcmp( s, "." ))
        return 19;
    else if(!strcmp( s, ":=" ))
        return 20;
    else if(!strcmp( s, "odd" ))
        return 8;
    else if(!strcmp( s, "=" ))
        return 9;
    else if(!strcmp( s, "!=" ))
        return 10;
    else if(!strcmp( s, "<" ))
        return 11;
    else if(!strcmp( s, ">" ))
        return 13;

}/* END isOtherValid */

#endif /*END lexical_analyzer.h */
