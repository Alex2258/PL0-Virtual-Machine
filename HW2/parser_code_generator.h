/* Header(s) & Define(s)*/
#ifndef PARSER_PARSER_CODE_GENERATOR_H
#define PARSER_PARSER_CODE_GENERATOR_H

#define MAX_SYMBOL_TABLE_SIZE 10000
#define CODE_SIZE 500

/* Symbols & Reserved Words */
char symbols[] = {'+','-','*','/','(',')','=',',','.','<','>',';',':'};
char *reserved_word[] = {"const","var","procedure","call","begin","end","if","then","else","while","do","read","read","write"};

/* Struct(s) */
typedef struct
{
    char type[3];   /* Designates the TYPE of token (1-33) */
    char value[12]; /* Value of token: Identifier or Value */
} token;

typedef struct
{
    int kind;      /* Const = 1 OR var = 2 */
    char name[12]; /* Name up to 11 chars long */
    int val;       /* Number in (ASCII value) */
    int level;     /* Lexi level */
    int addr;      /* M address */
} symbol;

typedef struct
{
    int op; /* OpCode */
    int l;  /* Lexi level */
    int m;  /* Modifier */
} code;

/*Global Variable(s) */
token curr_token;

int reg_ptr;        /* Top of Stack Pointer */
int sp;             /* Stack Pointer */
int level;          /* Maximum Allowed Lexi Level */
int curr_level;     /* The Current Lexi Level */

int symbol_ctr;                             /* Counter to maintain number of entires in table */
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE]; /* Symbol Table Data Struct */

int cx;                     /* Code Index */
code code_ds[CODE_SIZE];    /* Code Data Struct */

FILE *token_fin;

/* Function Declaration(s)/Prototype(s) */
int get_token();
void error(int n);
void insert_symbol(int k, char name[], int val, int addr);
void emit(int op, int l, int m);
void statement();
void expression();
void condition();
void block();

void program()
{
    get_token();
    block();

    if(strcmp(curr_token.type, "19") != 0)
        error(9);

    emit(9,0,3);
}/* END program */

void block()
{

    /* For insertion to symbol table:
     * When a constant or variable declaration is successful parsed,
     * store the identifier and value, to create a new symbol table entry.
     */
    char name[12];    //Symbol identifier.
    int val;          //Symbol value.

    int space = 4; //Reserve space for procedure.
    int proc_x; //Procedure index in symbol table.

    /*
     * For a procedure, space needs to be reserved for the return value, static link,
     * dynamic link, and return address. Starting at the fifth position from a lexicographical
     * level's memory space, variables are stored.
     */
    sp = 5; //Stack pointer.

    //Jump to procedure code. Need to save address of jump instruction generated.
    int jmpaddr = cx;

    //emit( 7 /*jmp*/, 0, 0 );

    //const-declaration ::= ["const" ident "=" number {"," ident "=" number} ";"].
    if(!strcmp(curr_token.type, "28" /*constsym*/))
    {
        do{

            get_token();

            if( strcmp(curr_token.type, "2" /*identsym*/) != 0 ) error(4);

            //Keep track of symbol identifier for symbol table entry.
            strcpy(name, curr_token.value);

            get_token();

            if( strcmp(curr_token.type, "9" /*eqlsym*/) != 0 ) error(3);

            get_token();

            if( strcmp(curr_token.type, "3" /*numbersym*/) != 0 ) error(2);

            //Keep track of symbol value for symbol table entry.
            val = atoi(curr_token.value);

            //Create symbol table entry, store in memory, and update stack pointer.
            insert_symbol( 1 /*constant*/, name, val, 0 );

            get_token();


        }while(!strcmp(curr_token.type, "17" /*commasym*/));

        if(strcmp(curr_token.type, "18" /*semicolonsym*/) != 0)
            error(5);


        get_token();

    }

    //var-declaration ::= ["int" ident {"," ident} ";"].
    if(!strcmp(curr_token.type, "29" /*intsym*/))
    {
        do{

            get_token();

            if( strcmp(curr_token.type, "2" /*identsym*/) != 0 )
                error(4);

            //Keep track of symbol identifier for symbol table.
            strcpy(name, curr_token.value);

            //Create symbol table entry, store in memory, and update stack pointer.
            //Variables are initialized to 0.
            insert_symbol( 2 /*variable*/, name, 0, sp );
            symbol_table[ symbol_ctr - 1 ].level = level;
            space++;
            sp++;

            get_token();

        }while( !strcmp(curr_token.type, "17" /*commasym*/) );

        if( strcmp(curr_token.type, "18" /*semicolonsym*/) != 0 )
            error(5);

        get_token();
    }

    //procedure-declaration ::= { "procedure" ident ";" block ";" }
    while(!strcmp( curr_token.type, "30" /*procsym*/ ))
    {

        get_token();

        //procedure must be followed by identifier
        if(strcmp(curr_token.type, "2" /*identsym*/) != 0)
            error(4);

        //Keep track of symbol identifier for symbol table entry.
        strcpy(name, curr_token.value);

        //Create symbol table entry, store in memory, and update stack pointer.
        insert_symbol( 3 /*procedure*/, name, 0, 0 );
        proc_x = symbol_ctr - 1; //Procedure index.
        symbol_table[ proc_x ].level = level;
        symbol_table[ proc_x ].addr = jmpaddr + 1;

        get_token();

        //semicolon expected
        if(strcmp( curr_token.type, "18" /*semicolonsym*/ ) != 0 )
            error(17);

        get_token();

        level++;

        emit( 7 /*jmp*/, 0, 0 );
        block();

        //semicolon expected
        if(strcmp(curr_token.type, "18" /*semicolonsym*/ )!= 0)
            error(17);

        emit( 2 /*rtn*/, 0, 0 );
        curr_level--;
        get_token();

    }

    code_ds[ jmpaddr ].m = cx;
    emit( 6 /*inc*/, 0, space );

    //statement.
    statement();

    //emit( 2 /*rtn*/, 0, 0 );
    //curr_level--;

}//END block

void statement()
{

    int i; //index.

    //Boolean to check if identifier is declared. If so, = 1; else, = 0.
    int dec = 0;

    //Save symbol table index for found identifier.
    int ident_index;

    /*
     * production rule:
     * statement ::= [ ident ":=" expression
     *               | "begin" statement { ";" statement } "end"
     *		   | "if" condition "then" statement
     *		   | "while" condition "do" statement
     *		   | "read" ident
     *		   | "write" ident
     *		   | e ].
     */

    //Follow grammar.
    // ident ":=" expression
    if( !strcmp(curr_token.type, "2" /*identsym*/) ){

        //Check if identifier has been declared.
        for( i = symbol_ctr - 1; i >= 0; i-- )
        {
            if( !strcmp( curr_token.value, symbol_table[i].name ) )
            { /*found*/

                if( symbol_table[i].kind == 1 /*const*/ )
                    error(12);
                else if( symbol_table[i].kind == 2 /*var*/ )
                {
                    dec = 1; //Variable, and declared!
                    ident_index = i; //Save identifier index.
                }

            }
        }

        //Undeclared identifier?
        if(!dec)
            error(11);

        get_token();

        if( strcmp(curr_token.type, "20" /*becomessym*/) != 0 ) error(3);

        get_token();

        expression();

        /*
         * Store the result of the expression at the memory address assigned to the symbol at
         * the left-hand side of the assignment statement.
         */
        emit( 4 /*sto*/, curr_level - symbol_table[ ident_index ].level, symbol_table[ ident_index ].addr - 1 /*symbol address*/ );

        reg_ptr--;

    }

        //"call" ident
    else if( !strcmp(curr_token.type, "27" /*callsym*/) ){

        int declared = 0;

        get_token();

        //identifier expected.
        if( strcmp(curr_token.type, "2" /*identsym*/) ) error(14);

        //Check if identifier has been declared.
        for( i = symbol_ctr - 1; i >= 0; i-- )
            if( !strcmp( curr_token.value, symbol_table[i].name ) ){ //Found!
                ident_index = i; //Save identifier index.
                declared = 1;
            }

        if( !declared ) { error( 11 ); }

        if( symbol_table[ident_index].kind == 3 /*proc*/ ){
            emit( 5 /*cal*/, level, symbol_table[ ident_index ].addr );
            curr_level ++;
        }
        else
            error( 14 ); //Call must be followed by a procedure identifier.

        get_token();

    }

    else if( !strcmp(curr_token.type, "21" /*beginsym*/) ){

        get_token();

        statement();

        while( !strcmp(curr_token.type, "18" /*semicolonsym*/) ){

            get_token();

            statement();

        }

        if( strcmp(curr_token.type, "22" /*endsym*/) != 0 ) error(26);

        get_token();

    }

        // if <condition> then <statement> [ "else" statement ]
    else if( !strcmp(curr_token.type, "23" /*ifsym*/) ){

        get_token();

        condition();

        if( strcmp(curr_token.type, "24" /*thensym*/) != 0 ) error(16);

        get_token();

        /*begin code generation for if-statement*/

        int ctemp = cx; //Save current code index.

        /*
         * Generate conditional jump instruction to execute or skip assembly code
         * generated for the inside of the if statement. We won't know where to jump until
         * the statement is recursively parsed, and code is generated for it. We'll use
         * ctemp to modify the jump instruction after the desired code index is calculated.
         */

        emit( 8 /*jpc*/, 0, 0 );

        statement();

        get_token();

        // [ "else" statement ]
        if( !strcmp( curr_token.type, "33" /*elsesym*/ ) ){

            int ctemp2 = cx; //Save current code index.

            emit( 7 /*jmp*/, 0, 0 );

            code_ds[ ctemp ].m = cx; //Have correct cx, so update jpc.

            get_token();

            statement();

            code_ds[ ctemp2 ].m = cx; //Have correct cx, so update jpc.

            reg_ptr--; //Done with condition result. Free up register.

        }

        else{
            code_ds[ ctemp ].m = cx; //Have correct cx, so update jpc.

            reg_ptr--; //Done with condition result. Free up register.
        }

        //Debugging statement:
        //printf("cx = %d\n", cx);

    }

        // while <condition> do <statement>
    else if( !strcmp(curr_token.type, "25" /*whilesym*/) ){

        int cx1 = cx;

        get_token();

        condition();

        int cx2 = cx; //Code index for JPC.

        emit( 8 /*jpc*/, 0, 0 );

        if( strcmp(curr_token.type, "26" /*dosym*/) != 0 ) error(18);

        get_token();

        statement();

        emit( 7 /*jmp*/, 0, cx1 );

        code_ds[ cx2 ].m = cx;

        reg_ptr--; //Done with condition result. Free up register.

    }

        // read ident
    else if( !strcmp( curr_token.type, "32" /*readsym*/ ) ){

        get_token();

        if( strcmp( curr_token.type, "2" /*identsym*/ ) != 0 ) error(29);

        //Check if identifier has been declared.
        for( i = symbol_ctr - 1; i >= 0; i-- )
            if( !strcmp( curr_token.value, symbol_table[i].name ) ){ /*found*/

                dec = 1; //Variable, and declared!
                ident_index = i; //Save identifier index.

            }

        //Undeclared identifier?
        if( !dec ) error(11);

        /*
         * Want to read input from user, and store into identifier.
         * Need to read from user and store in register, then store from register into
         * the memory location for the identifier.
         */

        emit( 9 /*sio*/, 0, 2 /*read*/ ); //User input to register.

        //Reading into variable.
        if( symbol_table[ ident_index ].kind == 2 /*var*/ )
            emit( 4 /*sto*/, curr_level - symbol_table[ ident_index ].level, symbol_table[ ident_index ].addr - 1 ); //Register to memory.

            //Attempting to read into constant.
        else if( symbol_table[ ident_index ].kind == 1 /*const*/ )
            error( 12 ); //Assignment to constant is not allowed.

        get_token();

    }

        // write ident
    else if( !strcmp( curr_token.type, "31" /*writesym*/ ) ){

        get_token();

        if( strcmp( curr_token.type, "2" /*identsym*/ ) != 0 ) error(29);

        //Check if identifier has been declared.
        for(i = symbol_ctr - 1; i >= 0; i--)
        {
            if(!strcmp(curr_token.value, symbol_table[i].name))
            { /*found*/

                if( symbol_table[i].kind == 1 || symbol_table[i].kind == 2 /*const or var*/ )
                {
                    dec = 1; //Declared!
                    ident_index = i; //Save identifier index.
                }

            }
        }

        //Undeclared identifier
        if(!dec)
            error(11);


        //Get variable from memory.
        if( symbol_table[ ident_index ].kind == 2 /*var*/ )
        {
            emit( 3 /*lod*/, curr_level - symbol_table[ ident_index ].level, symbol_table[ ident_index ].addr - 1 ); //Memory to register.
            emit( 9 /*sio*/, 0, 1 /*write*/ ); //Register to screen.
        }

            //Get constant from symbol table.
        else if( symbol_table[ ident_index ].kind == 1 /*const*/ )
        {
            emit( 1 /*lit*/, 0, symbol_table[ ident_index ].val ); //Symbol table to register.
            emit( 9 /*sio*/, 0, 1 /*write*/ ); //Register to screen.
        }

        get_token();

    }

}/* END statement */

void condition()
{

    /*
     * production rule:
     * condition ::= "odd" expression | expression rel-op expression.
     */

    //Follow the grammar.

    // "odd" expression
    if( !strcmp(curr_token.type, "8" /*oddsym*/) ){

        get_token();

        expression();

        //Generate code to store truth value of condition in RF.
        emit(2 /*odd*/, reg_ptr - 1, 6 );

    }

        // | expression rel-op expression
    else{

        expression();

        //If token is relational operator, get assembly code op-code.
        int op = rel_op();

        if( !op ) error(20);

        get_token();

        expression();

        //Generate code to store truth value of condition in RF.
        emit( op, reg_ptr - 2, reg_ptr - 1 );
        reg_ptr--; //Update RP.

    }

}/* END condition */

int rel_op()
{
    /*
     * production rule:
     * rel-op ::= "="|"<>"|"<"|"<="|">"|">=".
     */

    //Follow grammar.

    //If relational operator, return appropriate assembly op-code.
    if( !strcmp( curr_token.type, "9" /*eqlsym*/ ) )
        return 18;
    else if( !strcmp( curr_token.type, "10" /*neqsym*/ ) )
        return 19;
    else if( !strcmp( curr_token.type, "11" /*lessym*/ ) )
        return 20;
    else if( !strcmp( curr_token.type, "12" /*leqsym*/ ) )
        return 21;
    else if( !strcmp( curr_token.type, "13" /*gtrsym*/ ) )
        return 22;
    else if( !strcmp( curr_token.type, "14" /*geqsym*/ ) )
        return 23;

        //Else, return 0 (false).
    else
        return 0;

}/* END rel_op */

void expression()
{

    //Keep track of current operator token, for code generation.
    char addop[3];

    /*
     * production rule:
     * expression ::= ["+"|"-"] term { ("+"|"-") term }.
     */

    //Follow grammar.
    if( !strcmp(curr_token.type, "4" /*plussym*/) || !strcmp(curr_token.type, "5" /*minussym*/) ){

        //Save current operator token type.
        strcpy( addop, curr_token.type );

        get_token();

        term();

        //If term (operand) is preceded by a minus symbol, generate code to negate.
        if( !strcmp( addop, "5" /*minussym*/ ) )
            emit( 2 /*neg*/, reg_ptr - 1, 1 );

    }

    else term();

    while( !strcmp(curr_token.type, "4" /*plussym*/) || !strcmp(curr_token.type, "5" /*minussym*/) ){

        //Save current operator token type.
        strcpy( addop, curr_token.type );

        get_token();

        term();

        //If term (operand) is preceded by a plus symbol, generate code to add to prior operand.
        if( !strcmp( addop, "4" /*plussym*/ ) ){
            emit( 2 /*add*/, reg_ptr - 2, 2 );
            reg_ptr--;
        }

            // " " minus symbol " " subtract " ".
        else{
            emit( 2 /*sub*/, reg_ptr - 2, 3 );
            reg_ptr--;
        }

    }

}/* END expression */

void term()
{

    //Will hold token type, for code generation.
    char mulop[3];

    /*
     * production rule:
     * term ::= factor {("*"|"/" factor}.
     */

    //Follow grammar.

    factor();

    while( !strcmp(curr_token.type, "6" /*multsym*/) || !strcmp(curr_token.type, "7" /*slashsym*/) ){

        //Save token type.
        strcpy( mulop, curr_token.type );

        get_token();

        factor();

        //If operator is multiplication.
        if( !strcmp( mulop, "6" /*multsym*/ ) ){

            //Generate multiplication instruction. Operands are stored in RF.
            emit( 2 /*mult*/, reg_ptr - 2, 4);
            reg_ptr--;

        }

            //If operator is division.
        else{

            //Generation division instruction. Operands are stored in RF.
            //emit( 15 /*div*/, reg_ptr - 2, reg_ptr - 2, reg_ptr - 1 );
            emit( 2 /*div*/, reg_ptr - 2, 5);
            reg_ptr--;

        }

    }

}/* END term */

void factor()
{

    int i; //index.

    //Boolean to check if identifier is declared. If so, = 1; else, = 0.
    int dec = 0;

    //Save symbol table index for found identifier.
    int ident_index;

    // ident
    if( !strcmp(curr_token.type, "2" /*identsym*/) ){

        //Check for undeclared.
        for(  i = symbol_ctr - 1; i >= 0; i-- )
            if( !strcmp( curr_token.value, symbol_table[i].name ) ){
                dec = 1; //Declared!
                ident_index = i; //Save index.
            }

        //Undeclared identifier?
        if( !dec ) error(11);

        //Place identifier on Register File "stack" for operations.

        //If variable, load from memory.
        if( symbol_table[ident_index].kind == 2 /*var*/ )
            emit( 3 /*lod*/, curr_level - symbol_table[ ident_index ].level, symbol_table[ident_index].addr - 1 );

            //If constant, load literal value from symbol table.
        else if( symbol_table[ident_index].kind == 1 /*const*/ )
            emit( 1 /*lit*/, 0, symbol_table[ident_index].val );

        reg_ptr++; //Increment RP.

        get_token();

    }

        // | number
    else if( !strcmp(curr_token.type, "3") ){

        //Place number on Register File "stack" for operations.
        emit( 1 /*lit*/, 0, atoi(curr_token.value) );
        reg_ptr++; //Increment RP.

        get_token();

    }

        // | "(" expression ")".
    else if( !strcmp(curr_token.type, "15" /*left parenthesis*/) ){

        get_token();

        expression();

        if( strcmp(curr_token.type, "16" /*right parenthesis*/) != 0 ) error(22);

        get_token();

    }

    else
        error(23);

}/* END factor */

int get_token()
{

    /* Grab Next Token Type */
    if(fscanf(token_fin, "%s", curr_token.type) != EOF)
    {
        /* If Ident OR Num symbol then grab token value */
        if(!strcmp(curr_token.type, "2") || !strcmp(curr_token.type, "3"))
            fscanf(token_fin, "%s", curr_token.value);
        else
            curr_token.value[0] = '\0';/* All Other Tokens */


        /* Successful Token Fetch */
        return 1;
    }
    else /* No More Tokens */
    {
        curr_token.type[0] = '\0'; /* NULL token type to stop token fetch */
        curr_token.value[0] = '\0'; /* NULL token value to stop token fetch */
        return 0;
    }

}/* END get_token */

void error(int n)
{

    //Debugging statement:
    //printf("Token: Type: %s Val: %s\n", curr_token.type, curr_token.value);

    printf("**Error Num (");

    switch(n)
    {

        case 1:
            printf("1): Use '=' instead of ':='\n");
            break;
        case 2:
            printf("2): Must be followed by a numerical\n");
            break;
        case 3:
            printf("3): Identifier must be followed by =\n");
            break;
        case 4:
            printf("4): Procedure, int, and const must be followed by an identifier\n");
            break;
        case 5:
            printf("5): Missing Semicolon or Colon\n");
            break;
        case 6:
            printf("6): Incorrect symbol following a procedure declaration\n");
            break;
        case 7:
            printf("7): Statement expected\n");
            break;
        case 8:
            printf("8): Incorrect symbol after statement part block\n");
            break;
        case 9:
            printf("9): Period expected and missing\n");
            break;
        case 10:
            printf("10): Semicolon between statements is missing\n");
            break;
        case 11:
            printf("11): Undeclared identifier\n");
            break;
        case 12:
            printf("12): Assignment to constant or procedure is not allowed\n");
            break;
        case 13:
            printf("13): Assignment operator expected\n");
            break;
        case 14:
            printf("14): Call must be followed by an identifier\n");
            break;
        case 15:
            printf("15): Meaningless call to a constant or variable\n");
            break;
        case 16:
            printf("16): Then expected\n");
            break;
        case 17:
            printf("17): Semicolon or '}' expected\n");
            break;
        case 18:
            printf("18): 'do' expected\n");
            break;
        case 19:
            printf("19): Incorrect symbol following statement\n");
            break;
        case 20:
            printf("20): Relational operator expected\n");
            break;
        case 21:
            printf("21): Expression must not contain a procedure identifier\n");
            break;
        case 22:
            printf("22): Missing ')'\n");
            break;
        case 23:
            printf("23): Preceding factor cannot begin with this symbol\n");
            break;
        case 24:
            printf("24): Expression cannot begin with this symbol\n");
            break;
        case 25:
            printf("25): Number is too large\n");
            break;
        case 26:
            printf("26): 'end' expected\n");
            break;
        case 27:
            printf("27): All registers in use\n");
            break;
        case 28:
            printf("28): Variable is not initialized\n");
            break;
        case 29:
            printf("29): Identifier expected after read or write\n");
            break;
        case 30:
            printf("30): The generated code exceeds the maximum code size\n");
            break;
        default:
            break;
    }

    /* Close File(s) */
    fclose(token_fin);

    /* Error Found, Terminate Program */
    exit(0);

}/* END error */

void emit(int op, int l, int m)
{

    if(cx > CODE_SIZE)
        error(30); /* Code Exceeds MAX size allowed */
    else
    {
        code_ds[cx].op = op;    /* Opcode */
        code_ds[cx].l = l;      /* Lexi Level */
        code_ds[cx].m = m;      /* Modifier */
        cx++;
    }
}/* END emit */

void insert_symbol(int k, char name[], int val, int addr)
{

    /* Set the Symbol Fields */
    symbol_table[symbol_ctr].kind = k;
    strcpy(symbol_table[symbol_ctr].name, name);
    symbol_table[symbol_ctr].val = val;
    symbol_table[symbol_ctr].addr = addr;

    symbol_ctr++;

}/* END insert_symbol */

#endif /* END parser_code_generator.h */
