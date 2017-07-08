/* Header(s) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#ifndef PARSER_P_MACHINE_H
#define PARSER_P_MACHINE_H

/* Constant(s) */
#define SET -2
#define NOTSET -1
#define UNUSED_INDEX 0
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3


/* Struct(s) */
struct instruction
{
    int op; /* Opcode */
    int l;  /* Lexicographical Level */
    int m;  /* Modifier */
};

/* Global Variable(s) */
struct instruction codeTwo[MAX_CODE_LENGTH];       /* Array of Instructions */
int stack[MAX_STACK_HEIGHT];                    /* Array of Integers for the stack */
struct instruction ir;                          /* Instruction Register */
int pcTwo = 0;                                     /* Program Counter */
int bpTwo = 1;                                     /* Base Pointer */
int spTwo = 0;                                     /* Stack Pointer */

bool firstInst = true;
int instructCount;                              /* Counter to hold the amount of instructions read into memory */
int new_ar[MAX_STACK_HEIGHT];
int tempSp = NOTSET;
FILE* streamTwo;


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
void run_vm(void);

/* Main-Like Function */
void run_vm()
{

    /* Local Variable(s) */
    bool flag = true;
    int counterVar = 0;

    /* Open Assembly Code & Output File */
    streamTwo = fopen("assemblyCode.txt", "r");
    FILE* vmoutput = fopen("vmoutput.txt", "w");

    initialize();  /* Initialize Stack */
    readInstructions(); /* Read Instructions from input file */
    printInterpretedInst();

    printf("\t\t\t\t\tPC\tBP\tSP\tStack\n");fprintf(vmoutput,"\t\t\t\t\tPC\tBP\tSP\tStack\n");
    printf("Initial Values\t\t\t\t%d\t%d\t%d\n", pcTwo, bpTwo, spTwo);fprintf(vmoutput,"Initial Values\t\t%d\t%d\t%d\n", pcTwo, bpTwo, spTwo);

    while(flag == true)
    {
        fetch();
        flag = execute();
        counterVar++;

        if(counterVar % 10 == 0)
            system("pause");
    }

    printf("%d\t%d\t%d\n", pcTwo, bpTwo, spTwo);fprintf(vmoutput,"%d\t%d\t%d\n", pcTwo, bpTwo, spTwo);
}

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

    if(streamTwo == NULL)/* Failed to Open */
    {
        fclose(streamTwo); /* Close the file */
        return;
    }
    while((fscanf(streamTwo, "%d %d %d", &codeTwo[i].op, &codeTwo[i].l, &codeTwo[i].m)) == 3)
    {
        /* Read File and Store the instructions appropriately */
        i++;
    }

    instructCount = i;
    fclose(streamTwo);/* Close the file */

    return;
}/* END readInstructions */

void printInterpretedInst(void)
{
    /* Local Variable(s) */
    int i;

    printf("Line\tOP\tL\tM\n");fprintf(streamTwo,"Line\tOP\tL\tM\n"); /* Print Headers */

    for(i = 0; i < instructCount; i++)
    {
        printf("%d\t%s\t%d\t%d\n", i, translateOp(codeTwo[i].op), codeTwo[i].l, codeTwo[i].m); /* Print out Interpreted Instruction for each Opcode */
        fprintf(streamTwo,"%d\t%s\t%d\t%d\n", i, translateOp(codeTwo[i].op), codeTwo[i].l, codeTwo[i].m);
    }

    printf("\n\n");fprintf(streamTwo,"\n\n");
}/* END printInterpretedInst */

void printStack(void)
{
    /* Local Variable(s) */
    int i;
    int j;

    for(i = 1; i < spTwo + 1; i++)
    {
        for(j = 0; j < MAX_STACK_HEIGHT; j++)
        {
            if(new_ar[j] == i)
            {
                printf("| ");fprintf(streamTwo, "| ");
            }
        }

        printf("%d ",stack[i]);fprintf(streamTwo, "%d ", stack[i]);
    }

    if(tempSp == SET)
    {
        printf("| ");fprintf(streamTwo, "| ");
        printf("%d ",stack[i]);fprintf(streamTwo, "%d ",stack[i]);
        printf("%d ",stack[i+1]);fprintf(streamTwo, "%d ",stack[i+1]);
        printf("%d ",stack[i+2]);fprintf(streamTwo, "%d ",stack[i+2]);
        printf("%d ",stack[i+3]);fprintf(streamTwo, "%d ",stack[i+3]);
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
        printf("%d\t%d\t%d\t", pcTwo, bpTwo, spTwo);fprintf(streamTwo, "%d\t%d\t%d\t", pcTwo, bpTwo, spTwo);
        printStack();
        printf("\n"); fprintf(streamTwo, "\n");
    }

    /* Get Instruction Information */
    ir.op = codeTwo[pcTwo].op;
    ir.l = codeTwo[pcTwo].l;
    ir.m = codeTwo[pcTwo].m;

    //printf("\nOp:(%d) -- L:(%d) -- M:(%d)\n", ir.op, ir.l, ir.m);

    /* Print values before execution */
    printf("%d\t%s\t%d\t%d\t\t", pcTwo, translateOp(ir.op), ir.l, ir.m);
    fprintf(streamTwo, "%d\t%s\t%d\t%d\t\t", pcTwo, translateOp(ir.op), ir.l, ir.m);

    /* Increment PC by 1 */
    pcTwo = pcTwo +1;

    firstInst = false;
    return;
}/* END fetch */

bool execute(void)
{
    switch(ir.op)
    {
        case 1:
            /* LIT(eral) */
            spTwo += 1;
            stack[spTwo] = ir.m;
            break;
        case 2:
            /* OPR(Operation) */
            executeModifier();
            break;
        case 3:
            /* LOD(Load) */
            spTwo += 1;
            stack[spTwo] = stack[base(ir.l, bpTwo) + ir.m];
            break;
        case 4:
            /* STO(Store) */
            stack[(base(ir.l, bpTwo)) + ir.m] = stack[spTwo];
            sp -= 1;
            break;
        case 5:
            /* CAL(Call Function) */
            stack[spTwo + 1] = 0;              /* Space to Return a Value if needed */
            stack[spTwo + 2] = base(ir.l, bpTwo); /* Static Link */
            stack[spTwo + 3] = bpTwo;             /* Dynamic Link */
            stack[spTwo + 4] = pcTwo;             /* Return Address */
            bpTwo = (spTwo + 1);                  /* Move Base Pointer up */
            pcTwo = ir.m;                      /* Set PC to the location of called function */
            setNewAr(spTwo+1); tempSp = SET;
            break;
        case 6:
            /* INC(Allocate M Locals on Stack) */
            spTwo = (spTwo + ir.m);
            break;
        case 7:
            /* JMP(Jump) */
            pcTwo = ir.m;
            break;
        case 8:
            /* JPC (Jump if Top of Stack == 0) */
            if(stack[spTwo] == 0)
                pcTwo = ir.m;

            spTwo -= 1;
            break;
        case 9:
            /* SIO(Write Stack, Read Stack, HALT) */
            if(ir.m == 1)
            {
                printf("Top Item in Stack: (%d)\n", stack[spTwo]);
                spTwo -= 1;
            }
            else if(ir.m == 2)
            {
                spTwo += 1;
                readItemToStack();
            }
            else if(ir.m == 3)
            {
                pcTwo = 0; bpTwo = 0; spTwo = 0;
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
            spTwo = bpTwo - 1;
            pcTwo = stack[spTwo + 4];
            bpTwo = stack[spTwo + 3];
            removeAr();
            break;
        case 1:
            /* NEG */
            stack[spTwo] = -stack[spTwo];
            break;
        case 2:
            /* ADD */
            spTwo -= 1;
            stack[spTwo] = (stack[spTwo] + stack[spTwo + 1]);
            break;
        case 3:
            /* SUB */
            spTwo -= 1;
            stack[spTwo] = (stack[spTwo] - stack[spTwo + 1]);
            break;
        case 4:
            /* MUL */
            spTwo -= 1;
            stack[spTwo] = (stack[spTwo] * stack[spTwo + 1]);
            break;
        case 5:
            /* DIV */
            spTwo -= 1;
            stack[spTwo] = (stack[spTwo] / stack[spTwo + 1]);
            break;
        case 6:
            /* ODD */
            stack[spTwo] = (stack[spTwo] % 2);
            break;
        case 7:
            /* MOD */
            spTwo -= 1;
            stack[spTwo] = (stack[spTwo] % stack[spTwo + 1]);
            break;
        case 8:
            /* EQL */
            spTwo -= 1;

            if(stack[spTwo] == stack[spTwo + 1])
                stack[spTwo] = 1;
            else
                stack[spTwo] = 0;

            break;
        case 9:
            /* NEQ */
            sp -= 1;

            if(stack[spTwo] != stack[spTwo + 1])
                stack[spTwo] = 1;
            else
                stack[spTwo] = 0;

            break;
        case 10:
            /* LSS */
            spTwo -= 1;

            if(stack[spTwo] < stack[spTwo + 1])
                stack[spTwo] = 1;
            else
                stack[spTwo] = 0;

            break;
        case 11:
            /* LEQ */
            spTwo -= 1;

            if(stack[spTwo] <= stack[spTwo + 1])
                stack[spTwo] = 1;
            else
                stack[spTwo] = 0;

            break;
        case 12:
            /* GTR */
            spTwo -= 1;

            if(stack[spTwo] > stack[spTwo + 1])
                stack[spTwo] = 1;
            else
                stack[spTwo] = 0;

            break;
        case 13:
            /* GEQ */
            spTwo -= 1;

            if(stack[spTwo] >= stack[spTwo + 1])
                stack[spTwo] = 1;
            else
                stack[spTwo] = 0;

            break;
        default:
            return;
    }

    return;
}/* END executeModifier */

void readItemToStack(void)
{
    printf("Please enter an Integer to be placed into the TOP of the STACK: ");
    scanf("%d", &stack[spTwo]);
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
#endif
