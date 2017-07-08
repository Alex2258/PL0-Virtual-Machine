/* Libraries */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

/* Header(s) */
#include "complier_driver.h"
#include "lexical_analyser.h"
#include "parser_code_generator.h"
#include "p_machine.h"

/* Constant(s) */
#define INPUT_FILE "in.txt"

/* Function Declaration(s)/Prototype(s) */
void directives_handler();
FILE* run_scanner(void);
void run_parser(FILE* list);
int run_p_machine(void);
void intermediary_output(FILE *fin, FILE *temp_fout);
void examine_tokens(FILE *fin, FILE *table, FILE *list);
void create_source_program(FILE *fin);
void print_output(void);

/* Main Function */
int main(int argc, char **argv)
{
    /* Local Variable(s) */
    int ret_val;

    /* Initialize Command-Line Directions */
    directive.l = directive.a = directive.v = 0;

    /* Check Directions for execution */
    parseDirectives(argc, argv);

    /* Run Scanner */
    FILE *lexi_list = run_scanner();

    /* Run Parser */
    run_parser(lexi_list);

    /* Run P_Machine */
    ret_val = run_p_machine();

    /* Run Directives Handler */
    directives_handler();

    /* Print Desired Output */
    print_output();

    /* End */
    return 0;

}//END Main

void print_output(void)
{
    /* TODO */
}/* END print_output */

void directives_handler(void)
{
    if(directive.l == 0)
    {
        remove("source_program.txt");
        remove("lexeme_table.txt");
        remove("lexeme_list.txt");
    }

    if(directive.a == 0)
        remove("assembly.txt");

    if(directive.v == 0)
    {
        remove("output1.txt");
        remove("output2.txt");
    }
}/*END directives_handler */

/* Returns (1) on success */
FILE * run_scanner(void)
{
    /* Open File(s) */
    FILE *fin = fopen(INPUT_FILE, "r");
    FILE *temp_fout = fopen("temp_output.txt", "w");

    intermediary_output(fin, temp_fout);

    fin = fopen("temp_output.txt", "r");
    FILE *table = fopen("lexeme_table.txt", "w");
    FILE *list = fopen("lexeme_list.txt", "w");

    examine_tokens(fin, table, list);

    create_source_program(fin);

    return list;
}/* END run_scanner */

/* Returns (1) on success */
void run_parser(FILE *list)
{
    /* Local Variable(s) */
    int i;
    int x = 0;

    /* Open File(s) */
    token_fin = fopen("lexeme_list.txt", "r");

    /* Initialize Counters */
    symbol_ctr = 0;
    reg_ptr = 0;
    cx = 0;
    sp = 1;
    level = curr_level = 1;

    /* Initialize Struct(s) */
    for(i = 0; i < CODE_SIZE; i++)
    {
        code_ds[i].op = 0;
        code_ds[i].l = 0;
        code_ds[i].m = 0;
    }
    /* Recursively Parse Tokens */
    program();

    /* Finished w/ NO Errors */
    printf("No Errors Detected, Syntax is correct.\n");

    /* Open File to Store Assembly Code */
    FILE *fout = fopen( "assemblyCode.txt", "w" );

    /* Print Assembly Code to File */
    while(!(code_ds[x].op == 0 && code_ds[x].l == 0 && code_ds[x].m == 0))
    {
        fprintf(fout, "%d %d %d\n", code_ds[x].op,code_ds[x].l, code_ds[x].m);
        x++;
    }

    /* Close File(s) */
    fclose(token_fin);
    fclose(fout);

    return;
}/* END run_parser */

/* Returns (1) on success */
int run_p_machine()
{
    run_vm();

   return 0;
}/* END run_p_machine */

void intermediary_output(FILE *fin, FILE *temp_fout)
{
    while(fscanf(fin, "%s", buffer) != EOF) /*While where are tokens to read */
    {

        int len = (int)strlen(buffer); /* Grab the length of the token */

        /*For the length of the token */
        for(loop_counter = 0; loop_counter < len; loop_counter++ )
        {
            /*If we've reached end of token and NO special symbol is present, simply print token.
              If there are invalid tokens, they are ignored and handled later */
            if(loop_counter == (len-1) && !isSpecialSymbol(buffer[loop_counter]))
            {
                fprintf(temp_fout, "%s ", buffer); /*Print to file */
                temp_token_counter++;
            }
            /*If we hit a special symbol */
            else if(isSpecialSymbol(buffer[loop_counter]))
            {

                /* Examine EVERY particular special symbol token case (comment, not-equal, etc) & handle accordingly */
                if(len > 1)
                {
                    /* If start-comment */
                    if(buffer[loop_counter] == '/' && buffer[loop_counter+1 ] == '*')
                    {
                        strncpy(temp, buffer, loop_counter);
                        temp[loop_counter] = '\0';

                        fprintf(temp_fout, "%s ", temp);
                        temp_token_counter++;

                        fprintf(temp_fout, "/* ");
                        temp_token_counter++;

                        strncpy(buffer, &buffer[loop_counter]+2, len-1);
                        buffer[len - 1] = '\0';
                        len = (int) strlen(buffer);
                        loop_counter = -1;
                    }
                    /* If end-comment */
                    else if(buffer[loop_counter] == '*' && buffer[loop_counter+1] == '/')
                    {
                        strncpy(temp, buffer, loop_counter);
                        temp[ loop_counter ] = '\0';

                        fprintf(temp_fout, "%s ", temp);
                        temp_token_counter++;

                        fprintf(temp_fout, "*/ ");
                        temp_token_counter++;

                        strncpy(buffer, &buffer[loop_counter]+2, len-1);
                        buffer[len - 1] = '\0';
                        len = (int) strlen(buffer);
                        loop_counter = -1;
                    }
                    /*If NOT-EQUAL */
                    else if(buffer[loop_counter] == '!' && buffer[ loop_counter+1 ] == '=')
                    {
                        strncpy(temp, buffer, loop_counter);
                        temp[loop_counter] = '\0';

                        fprintf(temp_fout, "%s ", temp);
                        temp_token_counter++;

                        fprintf(temp_fout, "!= ");
                        temp_token_counter++;

                        strncpy(buffer, &buffer[loop_counter]+2, len-1);
                        buffer[len - 1] = '\0';
                        len = (int) strlen(buffer);
                        loop_counter = -1;
                    }
                    /* If LESS OR EQUAL */
                    else if(buffer[loop_counter] == '<' && buffer[loop_counter+1] == '=')
                    {
                        strncpy(temp, buffer, loop_counter);
                        temp[loop_counter] = '\0';

                        fprintf(temp_fout, "%s ", temp);
                        temp_token_counter++;

                        fprintf(temp_fout, "<= ");
                        temp_token_counter++;

                        strncpy(buffer, &buffer[loop_counter]+2, len-1);
                        buffer[len - 1] = '\0';
                        len = (int) strlen(buffer);
                        loop_counter = -1;
                    }
                    /* If GREATER OR EQUAL */
                    else if(buffer[loop_counter] == '>' && buffer[loop_counter+1] == '=')
                    {
                        strncpy(temp, buffer, loop_counter);
                        temp[loop_counter] = '\0';

                        fprintf(temp_fout, "%s ", temp );
                        temp_token_counter++;

                        fprintf(temp_fout, ">= ");
                        temp_token_counter++;

                        strncpy(buffer, &buffer[loop_counter]+2, len-1);
                        buffer[len - 1] = '\0';
                        len = (int) strlen(buffer);
                        loop_counter = -1;
                    }
                    /* if BECOME symbol */
                    else if(buffer[loop_counter] == ':' && buffer[loop_counter+1] == '=')
                    {
                        strncpy(temp, buffer, loop_counter);
                       temp[loop_counter] = '\0';

                        fprintf(temp_fout, "%s ", temp);
                        temp_token_counter++;

                        fprintf(temp_fout, ":= ");
                        temp_token_counter++;

                        strncpy(buffer, &buffer[loop_counter]+2, len-1);
                        buffer[len - 1] = '\0';
                        len = (int) strlen(buffer);
                        loop_counter = -1;
                    }
                    /* if NULL */
                    else if(len > 3 && buffer[loop_counter] == 'n' && buffer[loop_counter+1] == 'u' && buffer[loop_counter+2] == 'l' && buffer[loop_counter+3] =='l')
                    {
                        strncpy(temp, buffer, loop_counter);
                        temp[loop_counter] = '\0';

                        fprintf(temp_fout, "%s ", temp);
                        temp_token_counter++;

                        fprintf(temp_fout, "null ");
                        temp_token_counter++;

                        strncpy(buffer, &buffer[loop_counter]+4, len-1);
                        buffer[len - 1] = '\0';
                        len = (int) strlen(buffer);
                        loop_counter = -1;
                    }
                    /* if ODD */
                    else if(len > 2 && buffer[loop_counter] == 'o' && buffer[loop_counter+1] == 'd' && buffer[loop_counter+2] == 'd')
                    {
                        strncpy(temp, buffer, loop_counter);
                        temp[loop_counter] = '\0';

                        fprintf(temp_fout, "%s ", temp);
                        temp_token_counter++;

                        fprintf(temp_fout, "odd ");
                        temp_token_counter++;

                        strncpy(buffer, &buffer[loop_counter]+3, len-1);
                        buffer[len - 1] = '\0';
                        len = (int) strlen(buffer);
                        loop_counter = -1;
                    }

                }

                /* Special Symbol At Beginning */
                if(loop_counter == 0)
                {
                    /* Print Sym as its own token */
                    fprintf(temp_fout, "%c ", buffer[loop_counter]);
                    temp_token_counter++;

                    /* Shift the  buffer, and change index for loop */
                    strncpy(buffer, &buffer[loop_counter]+1, len-1);
                    buffer[len - 1] = '\0';
                    len = (int) strlen(buffer);
                    loop_counter = -1;
                }

                /* Valid symbol is splitting tokens within the buffer; split, print, and shift the tokens left of the valid symbol, including the symbol. */
                if(loop_counter > 0)
                {
                    strncpy(temp, buffer, loop_counter);
                    temp[loop_counter] = '\0';

                    fprintf(temp_fout, "%s ", temp);
                    temp_token_counter++;

                    fprintf(temp_fout, "%c ", buffer[loop_counter]);
                    temp_token_counter++;

                    strncpy(buffer, &buffer[loop_counter]+1, len-1);
                    buffer[len - 1] = '\0';
                    len = (int) strlen(buffer);
                    loop_counter = -1;
                }
            }
        }
    }

    /* Close File(s) */
    fclose(temp_fout);
    fclose(fin);
}/* END intermediary_output */

void examine_tokens(FILE *fin, FILE *table, FILE *list)
{
    /* Local Variable(s) */
    int isResWord = -1; /* If reserverd word != -1, otherwise = -1 */
    int len = 0;
    int isComment = 0; /* If comment = 1, otherwise = 0 */

    /* While there are tokens to read */
    while(fscanf(fin, "%s ", buffer) != EOF)
    {
        len = (int) strlen(buffer); /* Get Tok Lenth */

        /* Check for comment start */
        if(!strcmp(buffer, "/*"))
        {
            isComment = 1;
        }

        /* Check for comment end */
        else if(isComment == 1 && !strcmp(buffer, "*/"))
        {
            isComment = 0;
        }

        /* Only read tokens while not in a comment block */
        else if(!isComment)
        {

            /* Check for Errors in Tokens */
            if(detect_error(buffer))
            {
                fclose(fin);
                fclose(table);
                fclose(list);
                exit(0);
            }

            /* If reserved word, print to table and list accordingly */
            isResWord = isReservedWord(buffer);

            if(isResWord != -1)
            {
                fprintf(table, "%15s%15d\n", buffer, isResWord);
                fprintf(list, "%d ", isResWord);
            }
            /* If identifier, print to table and list accordingly */
            else if(isalpha(buffer[0]))
            {
                fprintf(table, "%15s%15d\n", buffer, 2);
                fprintf(list, "2 %s ", buffer);
            }
            /* If a number, print to table and list accordingly */
            else if(isdigit(buffer[0]))
            {
                fprintf(table, "%15s%15d\n", buffer, 3);
                fprintf(list, "3 %s ", buffer);
            }
            /* Otherwise its another VALID symbol as errors checked previously, print to table and list accordingly */
            else
            {
                fprintf(table, "%15s%15d\n", buffer, isOtherValid(buffer));
                fprintf(list, "%d ", isOtherValid(buffer));
            }
        }
    }

    /* Close File(s) */
    fclose(fin);
    fclose(table);
    fclose(list);

} /* END examine_tokens */

void create_source_program(FILE *fin)
{
    /* Local Variable(s) & File Open */
    fin = fopen(INPUT_FILE, "r");
    FILE *source = fopen("source_program.txt", "w");
    char copy;

    /* Copy until EOF reached */
    while((copy = fgetc( fin ))!= EOF)
        fputc(copy, source);

    /* Close File(s) */
    fclose(fin);
    fclose(source);

    remove("temp_output.txt"); /* Remove Temporary and Unnecessary File(s) */

}/* END create_source_program */

