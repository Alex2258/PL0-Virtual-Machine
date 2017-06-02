/* Header(s) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/* Constant(s) */
#define SET -2
#define NOTSET -1
#define UNUSED_INDEX 0
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

/* TODO
    --CREATE OUTPUT FILE
    --PRINT OUT EXECUTION OF PROGRAM AS IT IS RUNNING (MUST INCLUDE | SYMBOL BETWEEN AR's
    --TEST ENTIRE THING
*/

/* Struct(s) */
struct instruction
{
    int op; /* Opcode */
    int l;  /* Lexicographical Level */
    int m;  /* Modifier */
};

/* Global Variable(s) */
struct instruction code[MAX_CODE_LENGTH];       /* Array of Instructions */
int stack[MAX_STACK_HEIGHT];                    /* Array of Integers for the stack */
struct instruction ir;                          /* Instruction Register */
int pc = 0;                                     /* Program Counter */
int bp = 1;                                     /* Base Pointer */
int sp = 0;                                     /* Stack Pointer */

bool firstInst = true;
int instructCount;                              /* Counter to hold the amount of instructions read into memory */
int new_ar[MAX_STACK_HEIGHT];
int tempSp = NOTSET;
FILE* stream;


/* Function Declaration(s)/Prototype(s) */
void initialize(void);
void readInstructions(void);
void printInterpretedInst(void);
void printStack(void);
char* translateOp(int);
void fetch(void);
bool execute(void);
void executeModifier(void);
void readItemToStack(void);
int base(int, int);
void setNewAr(int);

/* Main */
int main(void)
{
    /* Local Variable(s) */
    bool flag = true;
    stream = fopen("vmoutput.txt", "w");

    initialize();  /* Initialize Stack */
    readInstructions(); /* Read Instructions from input file */
    printInterpretedInst();

    printf("\t\t\t\t\tPC\tBP\tSP\tStack\n");fprintf(stream,"\t\t\t\t\tPC\tBP\tSP\tStack\n");
    printf("Initial Values\t\t\t\t%d\t%d\t%d\n", pc, bp, sp);fprintf(stream,"Initial Values\t\t%d\t%d\t%d\n", pc, bp, sp);

    while(flag == true)
    {
        fetch();
        flag = execute();
    }

    printf("%d\t%d\t%d\n", pc, bp, sp);fprintf(stream,"%d\t%d\t%d\n", pc, bp, sp);

   return 0;
}/* END main */

/* Function(s) */
void initialize(void)
{
    /* Local Variable(s) */
    int i;

    for(i = 0; i < MAX_STACK_HEIGHT; i++)
    {
        stack[i] = UNUSED_INDEX;
        new_ar[i] = NOTSET;
    }

    return;
}/* END initializeStack */

void readInstructions(void)
{
    /* Local Variable(s) */
    int i = 0;

    FILE* stream = fopen("vminput.txt", "r");

    if(stream == NULL)/* Failed to Open */
    {
        fclose(stream); /* Close the file */
        return;
    }

    while((fscanf(stream, "%d %d %d", &code[i].op, &code[i].l, &code[i].m)) == 3)
    {
        /* Read File and Store the instructions appropriately */
        i++;
    }

    instructCount = i;
    fclose(stream);/* Close the file */

    return;
}/* END readInstructions */

void printInterpretedInst(void)
{
    /* Local Variable(s) */
    int i;

    printf("Line\tOP\tL\tM\n");fprintf(stream,"Line\tOP\tL\tM\n"); /* Print Headers */

    for(i = 0; i < instructCount; i++)
    {
        printf("%d\t%s\t%d\t%d\n", i, translateOp(code[i].op), code[i].l, code[i].m); /* Print out Interpreted Instruction for each Opcode */
        fprintf(stream,"%d\t%s\t%d\t%d\n", i, translateOp(code[i].op), code[i].l, code[i].m);
    }

    printf("\n\n");fprintf(stream,"\n\n");
}/* END printInterpretedInst */

void printStack(void)
{
    /* Local Variable(s) */
    int i;
    int j;

    for(i = 1; i < sp + 1; i++)
    {
        for(j = 0; j < MAX_STACK_HEIGHT; j++)
        {
            if(new_ar[j] == i)
            {
                printf("| ");fprintf(stream, "| ");
            }
        }

        printf("%d ",stack[i]);fprintf(stream, "%d ", stack[i]);
    }

    if(tempSp == SET)
    {
        printf("| ");fprintf(stream, "| ");
        printf("%d ",stack[i]);fprintf(stream, "%d ",stack[i]);
        printf("%d ",stack[i+1]);fprintf(stream, "%d ",stack[i+1]);
        printf("%d ",stack[i+2]);fprintf(stream, "%d ",stack[i+2]);
        printf("%d ",stack[i+3]);fprintf(stream, "%d ",stack[i+3]);
        tempSp = NOTSET;
    }

}/* END printStack */

void setNewAr(int loc)
{
    /* Local Variable(s) */
    int i;

    for(i=0; i<MAX_STACK_HEIGHT; i++)
    {
        if(new_ar[i] == NOTSET)
        {
            new_ar[i] = loc;
            return;
        }
    }

}/* end setNewAr */

void removeAr(void)
{
    /* Local Variable(s) */
    int i;

    for(i=0; i<MAX_STACK_HEIGHT; i++)
    {
        if(new_ar[i] != NOTSET)
            ;
        else
        {
            new_ar[i-1] = NOTSET;
            return;
        }
    }

}/* END removeAr */

void fetch(void)
{
    /* Print Values after execution of previous instruction and before fetch of next instruction */
    if(firstInst == false)
    {
        printf("%d\t%d\t%d\t", pc, bp, sp);fprintf(stream, "%d\t%d\t%d\t", pc, bp, sp);
        printStack();
        printf("\n"); fprintf(stream, "\n");
    }

    /* Get Instruction Information */
    ir.op = code[pc].op;
    ir.l = code[pc].l;
    ir.m = code[pc].m;

    /* Print values before execution */
    printf("%d\t%s\t%d\t%d\t\t", pc, translateOp(ir.op), ir.l, ir.m);
    fprintf(stream, "%d\t%s\t%d\t%d\t\t", pc, translateOp(ir.op), ir.l, ir.m);

    /* Increment PC by 1 */
    pc = pc +1;

    firstInst = false;
    return;
}/* END fetch */

bool execute(void)
{
    switch(ir.op)
    {
        case 1:
            /* LIT(eral) */
            sp += 1;
            stack[sp] = ir.m;
            break;
        case 2:
            /* OPR(Operation) */
            executeModifier();
            break;
        case 3:
            /* LOD(Load) */
            sp += 1;
            stack[sp] = stack[base(ir.l, bp) + ir.m];
            break;
        case 4:
            /* STO(Store) */
            stack[(base(ir.l, bp)) + ir.m] = stack[sp];
            sp -= 1;
            break;
        case 5:
            /* CAL(Call Function) */
            stack[sp + 1] = 0;              /* Space to Return a Value if needed */
            stack[sp + 2] = base(ir.l, bp); /* Static Link */
            stack[sp + 3] = bp;             /* Dynamic Link */
            stack[sp + 4] = pc;             /* Return Address */
            bp = (sp + 1);                  /* Move Base Pointer up */
            pc = ir.m;                      /* Set PC to the location of called function */
            setNewAr(sp+1); tempSp = SET;
            break;
        case 6:
            /* INC(Allocate M Locals on Stack) */
            sp = (sp + ir.m);
            break;
        case 7:
            /* JMP(Jump) */
            pc = ir.m;
            break;
        case 8:
            /* JPC (Jump if Top of Stack == 0) */
            if(stack[sp] == 0)
                pc = ir.m;

            sp -= 1;
            break;
        case 9:
            /* SIO(Write Stack, Read Stack, HALT) */
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
}/* END execute */

void executeModifier(void)
{
    switch(ir.m)
    {
        case 0:
            /* RET */
            sp = bp - 1;
            pc = stack[sp + 4];
            bp = stack[sp + 3];
            removeAr();
            break;
        case 1:
            /* NEG */
            stack[sp] = -stack[sp];
            break;
        case 2:
            /* ADD */
            sp -= 1;
            stack[sp] = (stack[sp] + stack[sp + 1]);
            break;
        case 3:
            /* SUB */
            sp -= 1;
            stack[sp] = (stack[sp] - stack[sp + 1]);
            break;
        case 4:
            /* MUL */
            sp -= 1;
            stack[sp] = (stack[sp] * stack[sp + 1]);
            break;
        case 5:
            /* DIV */
            sp -= 1;
            stack[sp] = (stack[sp] / stack[sp + 1]);
            break;
        case 6:
            /* ODD */
            stack[sp] = (stack[sp] % 2);
            break;
        case 7:
            /* MOD */
            sp -= 1;
            stack[sp] = (stack[sp] % stack[sp + 1]);
            break;
        case 8:
            /* EQL */
            sp -= 1;

            if(stack[sp] == stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 9:
            /* NEQ */
            sp -= 1;

            if(stack[sp] != stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 10:
            /* LSS */
            sp -= 1;

            if(stack[sp] < stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 11:
            /* LEQ */
            sp -= 1;

            if(stack[sp] <= stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 12:
            /* GTR */
            sp -= 1;

            if(stack[sp] > stack[sp + 1])
                stack[sp] = 1;
            else
                stack[sp] = 0;

            break;
        case 13:
            /* GEQ */
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
}/* END executeModifier */

void readItemToStack(void)
{
    printf("Please enter an Integer to be placed into the TOP of the STACK: ");
    scanf("%d", &stack[sp]);
    printf("\n");
}/* END readStack */

int base(int L, int base) /*  l stand for L in the instruction format */
{
    int bl;

    bl = base;

    while (L>0)
    {
        bl = stack[bl + 1];
        L--;
    }

    return bl;
}/* END base */

char* translateOp(int opcode)
{
    switch(opcode)
    {
        case 1:
            /* LIT(eral) */
            return "LIT";
            break;
        case 2:
            /* OPR(Operation) */
            return "OPR";
            break;
        case 3:
            /* LOD(Load) */
            return "LOD";
            break;
        case 4:
            /* STO(Store) */
            return "STO";
            break;
        case 5:
            /* CAL(Call Function) */
            return "CAL";
            break;
        case 6:
            /* INC(Allocate M Locals on Stack) */
            return "INC";
            break;
        case 7:
            /* JMP(Jump) */
            return "JMP";
            break;
        case 8:
            /* JPC (Jump if Top of Stack == 0) */
            return "JPC";
            break;
        case 9:
            /* SIO(Write Stack, Read Stack, HALT) */
            return "SIO";
            break;
        default:
            return "ERR";
            break;
    }

}/* END translateOp */
