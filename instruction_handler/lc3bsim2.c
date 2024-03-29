/*
    Name 1: Hyokwon Chung
    UTEID 1: HC27426
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

    int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

#define DR_MASKED(x) (((x) & 0x0E00) >> 9)
#define SR1_MASKED(x) (((x) & 0x01C0) >> 6)
#define SR2_MASKED(x) ((x) & 0x0007)
#define BaseR_MASKED(x) (((x) & 0x01E0) >> 6)

void setcc(int cond){
    int isNegative = (cond & 0x8000) != 0; // Check if bit 15 is set
    int isZero = (cond & 0xFFFF) == 0; // Check if all bits are 0

    if (isNegative){
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }
    else if (isZero){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else{
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;
    }
}

int LSHF(int num, int shift) {
    return num << shift;
}

int RSHF(int num, int shift, int arith) {
    // arith = 1 for arithmetic right shift, 0 for logical right shift
    if (arith) {
        if (num & 0x8000) {
            // If the number is negative, fill the leftmost bits with 1s after the shift
            num = num - 65536; // Convert to negative integer value
        }
    }
    num = num >> shift;
    return num;
}

int SEXT(int num, int bits){
    int pos = bits - 1;  // calculate the position of the MSB
    int mask = 1 << pos; // create a mask with the MSB set
    if (num & mask) { // if negative (MSB is 1)
        num = num - (1 << bits); // convert to negative number
        num = num | ~((1 << bits) - 1); // sign extend
    }
    return Low16bits(num);
}

/***************************************************************/


void ADD(int instruction){
    int DR = DR_MASKED(instruction); // contains the register number
    int SR1 = SR1_MASKED(instruction);
    int bit_5 = (((instruction) & 0x0020) >> 5);
    if(bit_5 == 0){
        int SR2 = SR2_MASKED(instruction);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + CURRENT_LATCHES.REGS[SR2]);
    }
    else{
        int imm5 = ((instruction) & 0x001F);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + SEXT(imm5, 5));
    }
    setcc(NEXT_LATCHES.REGS[DR]);
}

void AND(int instruction){
    int DR = DR_MASKED(instruction); // contains the register number
    int SR1 = SR1_MASKED(instruction);
    int bit_5 = (((instruction) & 0x0020) >> 5);
    if(bit_5 == 0){
        int SR2 = SR2_MASKED(instruction);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & CURRENT_LATCHES.REGS[SR2]);
    }
    else{
        int imm5 = ((instruction) & 0x001F);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & SEXT(imm5, 5));
    }
    setcc(NEXT_LATCHES.REGS[DR]);
}
void BR(int instruction){
    int n = (((instruction) & 0x0800) >> 11);
    int z = (((instruction) & 0x0400) >> 10);
    int p = (((instruction) & 0x0200) >> 9);
    int PCoffset9 = (((instruction) & 0x01FF));
    // printf("%x %x\n", NEXT_LATCHES.PC, LSHF(SEXT(PCoffset9, 9), 1));

    if((n && CURRENT_LATCHES.N) || (z && CURRENT_LATCHES.Z) || (p && CURRENT_LATCHES.P)){
        NEXT_LATCHES.PC = Low16bits(Low16bits(NEXT_LATCHES.PC) + LSHF(SEXT(PCoffset9, 9), 1));
    }
}

void JMP(int instruction){
    int BaseR = BaseR_MASKED(instruction);
    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[BaseR]);
}

void JSR(int instruction){
    /* TEMP = PC†
	if (bit(11)==0)
	    PC = BaseR;
	else
	    PC = PC† + LSHF(SEXT(PCoffset11), 1);
	R7 = TEMP;

	* PC†: incremented PC
    */

    int TEMP = NEXT_LATCHES.PC;
    int bit_11 = (((instruction) & 0x0800) >> 11);
    if(bit_11 == 0){
        int BaseR = BaseR_MASKED(instruction);
        NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[BaseR]);
    }
    else{
        int PCoffset11 = (((instruction) & 0x07FF));
        NEXT_LATCHES.PC =  Low16bits(NEXT_LATCHES.PC + LSHF(SEXT(PCoffset11, 11), 1));
    }
    NEXT_LATCHES.REGS[7] = TEMP;
}

void LDB(int instruction){
    int DR = DR_MASKED(instruction);
    int BaseR = BaseR_MASKED(instruction);
    // printf("%d\n", SEXT(((instruction) & 0x003F), 6));
    int loc = Low16bits((CURRENT_LATCHES.REGS[BaseR] + SEXT(((instruction) & 0x003F), 6)));
    // printf("%x\n", loc);
    NEXT_LATCHES.REGS[DR] = Low16bits(SEXT((MEMORY[loc/2][loc&1] & 0x00FF), 8));
    setcc(NEXT_LATCHES.REGS[DR]);
}

void LDW(int instruction){
    int DR = DR_MASKED(instruction);
    int BaseR = BaseR_MASKED(instruction);
    int offset6 = ((instruction) & 0x003F);
    // printf("%d\n" ,offset6);
    // printf("%x\n", Low16bits((CURRENT_LATCHES.REGS[BaseR] + LSHF(offset6, 1))/2));
    NEXT_LATCHES.REGS[DR] = Low16bits((((MEMORY[(CURRENT_LATCHES.REGS[BaseR] + LSHF(offset6, 1))/2][1])& 0x00FF) << 8)  +((MEMORY[(Low16bits(CURRENT_LATCHES.REGS[BaseR]) + LSHF(offset6, 1))/2][0]) & 0x00FF));
    setcc(NEXT_LATCHES.REGS[DR]);
}

void LEA(int instruction){
    int PCoffset9 = (((instruction) & 0x01FF));
    NEXT_LATCHES.REGS[DR_MASKED(instruction)] = Low16bits(NEXT_LATCHES.PC + LSHF(SEXT(PCoffset9, 9), 1));
    // setcc(Low16bits(CURRENT_LATCHES.REGS[DR_MASKED(instruction)]) // possibly deprecated (mentioned in the pdf);
}

void XOR(int instruction){
    int DR = DR_MASKED(instruction); // contains the register number
    int SR1 = SR1_MASKED(instruction);
    int bit_5 = (((instruction) & 0x0020) >> 5);
    if(bit_5 == 0){
        int SR2 = SR2_MASKED(instruction);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ CURRENT_LATCHES.REGS[SR2]);
    }
    else{
        int imm5 = ((instruction) & 0x001F);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ SEXT(imm5, 5));
    }
    setcc(NEXT_LATCHES.REGS[DR]); // whether the binary value produced, taken as 2's complement integer, is negative, zero, or positive
}

void RTI(int instruction){
    // printf("RTI is not implemented");
}

void SHF(int instruction){
    int DR = DR_MASKED(instruction); // contains the register number
    int SR = SR1_MASKED(instruction); // contains the register number
    int amount4 = ((instruction) & 0x000F);
    int bit_4 = (((instruction) & 0x0010) >> 4);
    int bit_5 = (((instruction) & 0x0020) >> 5);
    if(bit_4 == 0){
        NEXT_LATCHES.REGS[DR] = Low16bits(LSHF(CURRENT_LATCHES.REGS[SR], amount4));
    }
    else{
        if (bit_5 == 0){
            NEXT_LATCHES.REGS[DR] = Low16bits(RSHF(CURRENT_LATCHES.REGS[SR], amount4, 0));
        }
        else{
            // the right shift is an arithmetic shift; thus the original SR[15] is shifted into the vacated bit positions.
            NEXT_LATCHES.REGS[DR] = Low16bits(RSHF(CURRENT_LATCHES.REGS[SR], amount4, (CURRENT_LATCHES.REGS[SR] & 0x8000) >> 15));
        }
    }
    setcc(NEXT_LATCHES.REGS[DR]);
}

void STB(int instruction){
    int SR = DR_MASKED(instruction); // contains the register number
    int BaseR = BaseR_MASKED(instruction);
    int boffset6 = ((instruction) & 0x003F);
    int loc = Low16bits((CURRENT_LATCHES.REGS[BaseR] + SEXT(boffset6, 6)));
    MEMORY[loc/2][loc & 1] = (CURRENT_LATCHES.REGS[SR] & 0x00FF);
}

void STW(int instruction){
    int SR = DR_MASKED(instruction); // contains the register number
    int BaseR = BaseR_MASKED(instruction);
    int offset6 = ((instruction) & 0x003F);
    //printf("%x\n", offset6);
    //printf("%d\n", SEXT(offset6, 6));
    int loc = Low16bits((CURRENT_LATCHES.REGS[BaseR] + LSHF(SEXT(offset6, 6), 1)));
    //printf("%x\n", loc);
    MEMORY[loc/2][0] = (CURRENT_LATCHES.REGS[SR] & 0x00FF);
    MEMORY[loc/2][1] = ((CURRENT_LATCHES.REGS[SR] >> 8)& 0x00FF);
}

void TRAP(int instruction){
    int trapvect8 = ((instruction) & 0x00FF);
    NEXT_LATCHES.REGS[7] = Low16bits(NEXT_LATCHES.PC);
    NEXT_LATCHES.PC = Low16bits((MEMORY[LSHF(SEXT(trapvect8, 8), 1)][0] & 0x00FF) + ((MEMORY[LSHF(SEXT(trapvect8, 8), 1)][1] << 8) & 0xFF00)); // load the service routine address into PC
}

/***************************************************************/


int get_opcode(int instruction){
    return (instruction >> 12) & 0x0F;
}

int get_instruction(){
    int instruction = (MEMORY[CURRENT_LATCHES.PC >> 1][0] & 0x00FF);
    instruction += (MEMORY[CURRENT_LATCHES.PC >> 1][1] & 0x00FF) << 8;
    instruction = Low16bits(instruction);
    return instruction;
}

void process_instruction(){
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES
     */
    // int instruction = get_instruction();
    // int opcode = get_opcode(instruction);
    int instruction = MEMORY[CURRENT_LATCHES.PC >> 1][0] & 0x00FF;
    instruction |= (MEMORY[CURRENT_LATCHES.PC >> 1][1] & 0x00FF) << 8;
    instruction = Low16bits(instruction);
    int opcode = (instruction >> 12) & 0x0F;

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
    // printf("%x\n", opcode);
    switch (opcode) {
        case 0x00: BR(instruction); break;
        case 0x01: ADD(instruction); break;
        case 0x02: LDB(instruction); break;
        case 0x03: STB(instruction); break;
        case 0x04: JSR(instruction); break;
        case 0x05: AND(instruction); break;
        case 0x06: LDW(instruction); break;
        case 0x07: STW(instruction); break;
        case 0x08: RTI(instruction); break;
        case 0x09: XOR(instruction); break; // also NOT
        case 0x0A: printf("unused"); break;
        case 0x0B: printf("unused"); break;
        case 0x0C: JMP(instruction); break; // also RET
        case 0x0D: SHF(instruction); break;
        case 0x0E: LEA(instruction); break;
        case 0x0F: TRAP(instruction); break;
    }
}
