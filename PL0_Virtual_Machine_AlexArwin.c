//Header(s)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//Constant(s)
#define UNUSED_INDEX 2001
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

/* TODO
    --CREATE OUTPUT FILE
    --PRINT OUT EXECUTION OF PROGRAM AS IT IS RUNNING (MUST INCLUDE | SYMBOL BETWEEN AR's
    --TEST ENTIRE THING
*/

//Struct(s)
struct instruction
{
    int op; //Opcode
    int l;  //Lexicographical Level
    int m;  //Modifier
};

//Global Variable(s)
struct instruction code[MAX_CODE_LENGTH];       //Array of Instructions
int stack[MAX_STACK_HEIGHT];                    //Array of Integers for the stack
struct instruction ir;                          //Instruction Register
int pc = 0;                                     //Program Counter
int bp = 1;                                     //Base Pointer
int sp = 0;                                     //Stack Pointer


//Function Declaration(s)/Prototype(s)
void initialize(void);
void readInstructions(void);
void fetch(void);
bool execute(void);
void executeModifier(void);
void readItemToStack(void);
int base(int, int);

//Main
int main(void)
{
    //Local Variable(s)
    bool flag = true;

    initialize();  //Initialize Stack
    readInstructions(); //Read Instructions from input file

    while(flag == true)
    {
        fetch();
        flag = execute();

    }

   return;
}//END main

//Function(s)
void initialize(void)
{
    //Local Variable(s)
    int i;

    for(i = 0; i < MAX_STACK_HEIGHT; i++)
    {
        stack[i] = UNUSED_INDEX;
    }

    return;
}//END initializeStack

void readInstructions(void)
{
    //Local Variable(s)
    int i = 0;

    FILE* stream = fopen("vminput.txt", "r");

    if(stream == NULL)//Failed to Open
    {
        fclose(stream); //Close the file
        return;
    }

    while((fscanf(stream, "%d %d %d", &code[i].op, &code[i].l, &code[i].m)) == 3)
    {
        //Read File and Store the instructions appropriately
        i++;
    }

    fclose(stream);//Close the file

    return;
}//END readInstructions

void fetch(void)
{
    //Get Instruction Information
    ir.op = code[pc].op;
    ir.l = code[pc].l;
    ir.m = code[pc].m;

    //Increment PC by 1
    pc = pc +1;

    return;
}//END fetch

bool execute(void)
{
    switch(ir.op)
    {
        case 1:
            //LIT(eral)
            sp += 1;
            stack[sp] = ir.m;
            break;
        case 2:
            //OPR(Operation)
            executeModifier();
            break;
        case 3:
            //LOD(Load)
            sp += 1;
            stack[sp] = stack[base(ir.l, bp) + ir.m];
            break;
        case 4:
            //STO(Store)
            stack[base(ir.l, bp) + ir.m] = stack[sp];
            sp -= 1;
            break;
        case 5:
            //CAL(Call Function)
            stack[sp + 1] = 0;              //Space to Return a Value if needed
            stack[sp + 2] = base(ir.l, bp); //Static Link
            stack[sp + 3] = bp;             //Dynamic Link
            stack[sp + 4] = pc;             //Return Address
            bp = (sp + 1);                  //Move Base Pointer up
            pc = ir.m;                      //Set PC to the location of called function
            break;
        case 6:
            //INC(Allocate M Locals on Stack)
            sp = (sp + ir.m);
            break;
        case 7:
            //JMP(Jump)
            pc = ir.m;
            break;
        case 8:
            //JPC (Jump if Top of Stack == 0)
            if(stack[sp] == 0)
                pc = ir.m;

            sp -= 1;
            break;
        case 9:
            //SIO(Write Stack, Read Stack, HALT)
            if(ir.m == 1)
            {
                printf("Top Item in Stack: (%d)\n", stack[sp]);
                sp -= 1;
            }
            else if(ir.m == 2)
            {
                sp += 1;
                readItemToStack();
            }
            else if(ir.m == 3)
            {
                pc = 0; bp = 0; sp = 0;
                return false;
            }

            break;
        default:
            return false;
    }

    return true;
}//END execute

void executeModifier(void)
{
    switch(ir.m)
    {
        case 0:
            //RET
            sp = bp - 1;
            pc = stack[sp + 4];
            bp = stack[sp + 3];
            break;
        case 1:
            //NEG
            stack[sp] = -stack[sp];
            break;
        case 2:
            //ADD
            sp -= 1;
            stack[sp] = (stack[sp] + stack[sp + 1]);
            break;
        case 3:
            //SUB
            sp -= 1;
            stack[sp] = (stack[sp] - stack[sp + 1]);
            break;
        case 4:
            //MUL
            sp -= 1;
            stack[sp] = (stack[sp] * stack[sp + 1]);
            break;
        case 5:
            //DIV
            sp -= 1;
            stack[sp] = (stack[sp] / stack[sp + 1]);
            break;
        case 6:
            //ODD
            stack[sp] = (stack[sp] % 2);
            break;
        case 7:
            //MOD
            sp -= 1;
            stack[sp] = (stack[sp] % stack[sp + 1]);
            break;
        case 8:
            //EQL
            sp -= 1;

            if(stack[sp] == stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 9:
            //NEQ
            sp -= 1;

            if(stack[sp] != stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 10:
            //LSS
            sp -= 1;

            if(stack[sp] < stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 11:
            //LEQ
            sp -= 1;

            if(stack[sp] <= stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 12:
            //GTR
            sp -= 1;

            if(stack[sp] > stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 13:
            //GEQ
            sp -= 1;

            if(stack[sp] >= stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        default:
            return;
    }

    return;
}//END executeModifier

void readItemToStack(void)
{
    printf("Please enter an Integer to be placed into the TOP of the STACK: ");
    scanf("%d", stack[sp]);
    printf("\n");
}//END readStack

int base(l, base) // l stand for L in the instruction format
{
    int baseLevel = base; //find base L levels down

    while (l > 0)
    {
        baseLevel = stack[baseLevel + 1];
        l--;
    }

    return baseLevel;
}
