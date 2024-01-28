/*
	Name 1: Hyokwon Chung
	UTEID 1: hc27426
*/
#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

// Opening And Closing Files
FILE* infile = NULL;
FILE* outfile = NULL;

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];	
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];
int symbolcount = 0;

// Parsing Command Line Arguments
int parse_cl(int argc, char* argv[]) { 
  char *prgName   = NULL;
  char *iFileName = NULL;
  char *oFileName = NULL;

  prgName   = argv[0];
  iFileName = argv[1];
  oFileName = argv[2];

  printf("program name = '%s'\n", prgName);
  printf("input file name = '%s'\n", iFileName);
  printf("output file name = '%s'\n", oFileName);
}

// Convert a String To a Number (take hex(x) and bin(#) into int)
int toNum( char * pStr )
{
  char * t_ptr;
  char * orig_pStr;
  int t_length,k;
  int lNum, lNeg = 0;
  long int lNumLong;

  orig_pStr = pStr;
  if( *pStr == '#' )				/* decimal */
  { 
    pStr++;
    if( *pStr == '-' )				/* dec is negative */
    {
      lNeg = 1;
      pStr++;
    }
    t_ptr = pStr;
    t_length = strlen(t_ptr);
    for(k=0;k < t_length;k++)
    {
      if (!isdigit(*t_ptr))
      {
  printf("Error: invalid decimal operand, %s\n",orig_pStr);
  exit(4);
      }
      t_ptr++;
    }
    lNum = atoi(pStr);
    if (lNeg)
      lNum = -lNum;

    return lNum;
  }
  else if( *pStr == 'x' )	/* hex     */
  {
    pStr++;
    if( *pStr == '-' )				/* hex is negative */
    {
      lNeg = 1;
      pStr++;
    }
    t_ptr = pStr;
    t_length = strlen(t_ptr);
    for(k=0;k < t_length;k++)
    {
      if (!isxdigit(*t_ptr))
      {
  printf("Error: invalid hex operand, %s\n",orig_pStr);
  exit(4);
      }
      t_ptr++;
    }
    lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
    lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
    if( lNeg )
      lNum = -lNum;
    return lNum;
  }
  else
  {
printf( "Error: invalid operand, %s\n", orig_pStr);
exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
  }
}

/* isOpcode() Checks if a string is an opcode, called by readAndParse()
  Returns 1 if the input string is a valid opcode, -1 otherwise.
  {ADD, AND, BR(all 8 variations), HALT, JMP, JSR, JSRR, LDB, LDW,
  LEA, NOP, NOT, RET, LSHF, RSHFL, RSHFA, RTI, STB, STW, TRAP, XOR}

*/
int isOpcode(char* opcode) {
  if (
    strcmp(opcode, "add") == 0 || strcmp(opcode, "and") == 0 ||
    strcmp(opcode, "brn") == 0 || strcmp(opcode, "brz") == 0 ||
    strcmp(opcode, "brp") == 0 || strcmp(opcode, "brnz") == 0 ||
    strcmp(opcode, "brnp") == 0 || strcmp(opcode, "brzp") == 0 ||
    strcmp(opcode, "br") == 0 || strcmp(opcode, "brnzp") == 0 ||
    strcmp(opcode, "halt") == 0 || strcmp(opcode, "jmp") == 0 ||
    strcmp(opcode, "jsr") == 0 || strcmp(opcode, "jsrr") == 0 ||
    strcmp(opcode, "ldb") == 0 || strcmp(opcode, "ldw") == 0 ||
    strcmp(opcode, "lea") == 0 || strcmp(opcode, "nop") == 0 ||
    strcmp(opcode, "not") == 0 || strcmp(opcode, "ret") == 0 ||
    strcmp(opcode, "lshf") == 0 || strcmp(opcode, "rshfl") == 0 ||
    strcmp(opcode, "rshfa") == 0 || strcmp(opcode, "rti") == 0 ||
    strcmp(opcode, "stb") == 0 || strcmp(opcode, "stw") == 0 ||
    strcmp(opcode, "trap") == 0 || strcmp(opcode, "xor") == 0
  ) {
    return 1;  // The opcode is valid.
  }
  return -1;  // The opcode is not valid.
}

/* readAndParse() Reads and parses one line of the assembly language file.
  Returns DONE, OK, or EMPTY_LINE
  DONE when the end of the file is encountered
  OK when a line is successfully read and parsed
  EMPTY_LINE when a blank line is encountered
  The function passes back the label, opcode, and operands to input parameters
*/
#define MAX_LINE_LENGTH 255
enum
{
    DONE, OK, EMPTY_LINE
};
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char** pOpcode, 
char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
{
  char * lRet, * lPtr;
  int i;
  if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
  return( DONE );
  for( i = 0; i < strlen( pLine ); i++ )
  pLine[i] = tolower( pLine[i] );
  
      /* convert entire line to lowercase */
  *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

  /* ignore the comments */
  lPtr = pLine;

  while( *lPtr != ';' && *lPtr != '\0' &&
  *lPtr != '\n' ) 
  lPtr++;

  *lPtr = '\0';
  if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
  return( EMPTY_LINE );

  if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
  {
  *pLabel = lPtr;
  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
  }
  
      *pOpcode = lPtr;

  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
  
      *pArg1 = lPtr;
  
      if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

  *pArg2 = lPtr;
  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

  *pArg3 = lPtr;

  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

  *pArg4 = lPtr;

  return( OK );
}

int reg_to_binary(char* arg){
  int reg;
  if (arg[0] != 'r') {
    printf("Error: register does not start with r\n");
    exit(4);
  }
  if (strlen(arg) > 3) {
    printf("Error: register val to long\n");
    exit(4);
  }
  reg = atoi(++arg);
  if (reg < 0 || reg > 7) {
    printf("Error: register val out of range\n");
    exit(4);
  }
  return reg;
}

int pass1(char* input_file) {
  // variables for readAndParse()
  char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
  int lRet;
  int linecount = 0;
  FILE *lInfile;
	lInfile = fopen( input_file, "r" );	/* open the input file */

  /* first pass */
  /* keep track of label/symbol to the symbol table */

  do
  {
  lRet = readAndParse( lInfile, lLine, &lLabel,
      &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
  if( lRet != DONE && lRet != EMPTY_LINE )
  {
    // check if the label is valid
    if (!strcmp(lLabel, "getc") || (!strcmp(lLabel, "in")) || (!strcmp(lLabel, "out")) || (!strcmp(lLabel, "puts"))) {
      printf("Error: label cannot be a trap vector\n");
      exit(4);
    }
    if (isOpcode(lLabel) == 1) {
      printf("Error: label cannot be an opcode\n");
      exit(4);
    }
    if (lLabel[0] == 'x') {
      printf("Error: label cannot start with an x\n");
      exit(4);
    }
    if (strlen(lLabel) > MAX_LABEL_LEN) {
      printf("Error: label cannot be longer than 20 characters\n");
      exit(4);
    }
    for (int i = 0; i < strlen(lLabel) && lLabel[i] != '\0'; i++) {
      if (!isalnum(lLabel[i])) {
        printf("Error: label must be alphanumeric\n");
        exit(4);
      }
    }
    for (int i = 0; i < symbolcount; i++) {
      if (strcmp(symbolTable[i].label, lLabel) == 0) {
        printf("Error: label cannot be a duplicate\n");
        exit(4);
      }
    }

    // all condtions are met, add to symbol table
    strcpy(symbolTable[symbolcount].label, lLabel); // set label to lLabel
    symbolTable[symbolcount].address = linecount; // placeholder for address
    symbolcount++; // increment symbolcount
  }
  linecount++; // increment linecount

  } while( lRet != DONE );
}

int get_label_address(char* label) {
  /* get the address of the label */
  /* return the address as an int */
  for (int i = 0; i < symbolcount; i++) {
    if (strcmp(symbolTable[i].label, label) == 0) {
      return symbolTable[i].address;
    }
  }
  printf("Error: label not found\n");
  exit(4);
}

int translate(char* opcode, char* arg1, char* arg2, char* arg3, char* arg4, int currentAddr){
  /* translate each instruction to machine code */
  /* return the machine code as an int */

  int ret_hex;
  if(strcmp(opcode, "add") == 0) {
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b0001;
    int dr = reg_to_binary(arg1);
    int sr1 = reg_to_binary(arg2);
    if (arg3[0] == 'r') {
      int sr2 = reg_to_binary(arg3);
      ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | sr2;
    }
    else {
      int imm5 = toNum(arg3);
      // TODO: check if imm5 is in range
      ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | (1 << 5) | imm5;
    }
    return ret_hex;
  }
  else if(strcmp(opcode, "and") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b0101;
    int dr = reg_to_binary(arg1);
    int sr1 = reg_to_binary(arg2);
    if (arg3[0] == 'r') {
      int sr2 = reg_to_binary(arg3);
      ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | sr2;
    }
    else {
      int imm5 = toNum(arg3);
      // TODO: check if imm5 is in range
      if(!(imm5 <= 15) || !(imm5 >= -16)) {
        printf("Error: imm5 out of range\n");
        exit(4);
      }
      ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | (1 << 5) | imm5;
    }
    return ret_hex;
  }
  else if(strncmp(opcode, "br", 2) == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b0000;
    int nzp = 0b000;
    if (strcmp(opcode, "brn") == 0) {
      nzp = 0b100;
    }
    else if (strcmp(opcode, "brz") == 0) {
      nzp = 0b010;
    }
    else if (strcmp(opcode, "brp") == 0) {
      nzp = 0b001;
    }
    else if (strcmp(opcode, "brnz") == 0) {
      nzp = 0b110;
    }
    else if (strcmp(opcode, "brnp") == 0) {
      nzp = 0b101;
    }
    else if (strcmp(opcode, "brzp") == 0) {
      nzp = 0b011;
    }
    else if (strcmp(opcode, "br") == 0) {
      nzp = 0b111;
    }
    else if (strcmp(opcode, "brnzp") == 0) {
      nzp = 0b111;
    }
    else {
      printf("Error: invalid br opcode\n");
      exit(4);
    }
    // get address from label;
    for (int i = 0; i < symbolcount; i++) {
      if (strcmp(symbolTable[i].label, arg1) == 0) {
        int lab_address = (symbolTable[i].address * 2) + currentAddr;
        int pc = currentAddr + 2;
        int PCoffset9 = lab_address- pc;
        PCoffset9 &= 0x1ff;
        ret_hex = (op_bin << 12) | (nzp << 9) | PCoffset9;
        return ret_hex;
      }
    }
    printf("Error: label not found\n");
    exit(4);
  }
  else if(strcmp(opcode, "halt") == 0){
    if (arg1 != NULL || arg2 != NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1111;
    ret_hex = (op_bin << 12) | 0x25;
    return ret_hex; // trap 0x25
  }
  else if(strcmp(opcode, "jmp") == 0){
    if (arg1 == NULL || arg2 != NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1100;
    int br = reg_to_binary(arg1);
    ret_hex = (op_bin << 12) | (br << 6);
    return ret_hex;
  }
  else if(strcmp(opcode, "jsr") == 0){
    if (arg1 == NULL || arg2 != NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b0100;
    // get address from label;

    // TODO: fix label and address calculations (PCoffset11)
    for (int i = 0; i < symbolcount; i++) {
      if (strcmp(symbolTable[i].label, arg1) == 0) {
        int lab_address = (symbolTable[i].address * 2) + currentAddr;
        int pc = currentAddr + 2;
        int PCoffset11 = lab_address- pc;
        PCoffset11 &= 0x7ff;
        ret_hex = (op_bin << 12) | (1 << 11) | PCoffset11;
        return ret_hex;
      }
      else {
        printf("Error: label not found\n");
        exit(4);
      }
    }
  }
  else if(strcmp(opcode, "jsrr") == 0){
    if (arg1 == NULL || arg2 != NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b0100;
    int br = reg_to_binary(arg1);
    ret_hex = (op_bin << 12) | (br << 6);
    return ret_hex;
  }
  else if(strcmp(opcode, "ldb") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b0010;
    int dr = reg_to_binary(arg1);
    int sr1 = reg_to_binary(arg2);
    int boffset6 = toNum(arg3);
    //TODO: check if boffset6 is in range
    boffset6 &= 0x3f;
    ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | boffset6;
    return ret_hex;  
  }
  else if(strcmp(opcode, "ldw") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b0110;
    int dr = reg_to_binary(arg1);
    int sr1 = reg_to_binary(arg2);
    int offset6 = toNum(arg3);
    if(!(offset6 <= 31) || !(offset6 >= -32)){
      printf("Error: offset6 out of range\n");
      exit(4);
    }
    ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | offset6;
    return ret_hex;  
  }
  else if(strcmp(opcode, "lea") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1110;
    int dr = reg_to_binary(arg1);
    // TODO: fix label and address calculations (PCoffset9)
    for (int i = 0; i < symbolcount; i++) {
      if (strcmp(symbolTable[i].label, arg1) == 0) {
        int lab_address = (symbolTable[i].address * 2) + currentAddr;
        int pc = currentAddr + 2;
        int PCoffset9 = lab_address- pc;
        if ((PCoffset9 > 255) || (PCoffset9 < -256)){
          printf("Error: PCoffset9 out of range\n");
          exit(4);
        };
        PCoffset9 &= 0x1ff;
        ret_hex = (op_bin << 12) | (dr << 9) | PCoffset9;
        return ret_hex;
      }
      else {
        printf("Error: label not found\n");
        exit(4);
      }
    }
  }
  else if(strcmp(opcode, "nop") == 0){
    if (arg1 != NULL || arg2 != NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    ret_hex = 0x0000;
    return ret_hex;
    }
  else if(strcmp(opcode, "not") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1001;
    int dr = reg_to_binary(arg1);
    int sr = reg_to_binary(arg2);
    ret_hex = (op_bin << 12) | (dr << 9) | (sr << 6) | (1 << 5) | (0b11111);
  }
  else if(strcmp(opcode, "ret") == 0){
    if (arg1 != NULL || arg2 != NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1100;
    ret_hex = (op_bin << 12) | (7 << 6);
    return ret_hex;
  }
  else if(strcmp(opcode, "lshf") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1101;
    int dr = reg_to_binary(arg1);
    int sr1 = reg_to_binary(arg2);
    int amount4 = toNum(arg3);
    if(!(amount4 <= 15) || !(amount4 >= 0)) {
      printf("Error: amount4 out of range\n");
      exit(4);
    }
    ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | amount4;
  }
  else if(strcmp(opcode, "rshfl") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1101;
    int dr = reg_to_binary(arg1);
    int sr = reg_to_binary(arg2);
    int amount4 = toNum(arg3);
    if(!(amount4 <= 15) || !(amount4 >= 0)) {
      printf("Error: amount4 out of range\n");
      exit(4);
    }
    ret_hex = (op_bin << 12) | (dr << 9) | (sr << 6) | (1 << 4) | amount4;
    return ret_hex;
  }
  else if(strcmp(opcode, "rshfa") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1101;
    int dr = reg_to_binary(arg1);
    int sr = reg_to_binary(arg2);
    int amount4 = toNum(arg3);
    if(!(amount4 <= 15) || !(amount4 >= 0)){
      printf("Error: amount4 out of range\n");
      exit(4);}
    ret_hex = (op_bin << 12) | (dr << 9) | (sr << 6) | (1 << 5) | (1 << 4) | amount4;
    return ret_hex;
  }
  else if(strcmp(opcode, "rti") == 0){
    if (arg1 != NULL || arg2 != NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b1000;
    ret_hex = (op_bin << 12);
    return ret_hex;
  }
  else if(strcmp(opcode, "stb") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
      exit(4);
    }
    int op_bin = 0b0011;
    int sr = reg_to_binary(arg1);
    int br = reg_to_binary(arg2);
    int boffset6 = toNum(arg3);
    ret_hex = (op_bin << 12) | (sr << 9) | (br << 6) | boffset6;
    return ret_hex;
  }
  else if(strcmp(opcode, "stw") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
    }
    int op_bin = 0b0111;
    int sr = reg_to_binary(arg1);
    int br = reg_to_binary(arg2);
    int offset6 = toNum(arg3);
    if(!(offset6 <= 31) || !(offset6 >= -32)){
      printf("Error: offset6 out of range\n");
      exit(4);
    }
    ret_hex = (op_bin << 12) | (sr << 9) | (br << 6) | offset6;
    return ret_hex;
  }
  else if(strcmp(opcode, "trap") == 0){
    if (arg1 == NULL || arg2 != NULL || arg3 != NULL || arg4 != NULL) {
      printf("Error: operand error\n");
    }
    int op_bin = 0b1111;
    int trapvect8 = toNum(arg1);
    if(!(trapvect8 <= 255) || !(trapvect8 >= 0)){
      printf("Error: trapvect8 out of range\n");
      exit(4);
    }
    ret_hex = (op_bin << 12) | trapvect8;
    return ret_hex;
  }
  else if(strcmp(opcode, "xor") == 0){
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL || arg4 != NULL) {
      printf("Error: operand error\n");
    }
    int op_bin = 0b1001;
    int dr = reg_to_binary(arg1);
    int sr1 = reg_to_binary(arg2);
    if (arg3[0] == 'r') {
      int sr2 = reg_to_binary(arg3);
      ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | sr2;
    }
    else {
      int imm5 = toNum(arg3);
      if(!(imm5 <= 15) || !(imm5 >= -16)) {
        printf("Error: imm5 out of range\n");
        exit(4);
      }
      ret_hex = (op_bin << 12) | (dr << 9) | (sr1 << 6) | (1 << 5) | imm5;
    }
    return ret_hex;
  }
  else {
    return -1;
  }
}

int pass2(int argc, char* argv[]) {
  /* second pass */
  /* translate each instruction to machine code one by one */
  FILE *lInfile;
	lInfile = fopen(argv[1], "r" );	/* open the input file */

  // variables for readAndParse()
  char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
  int lRet;

  int startAddr = 0; // placeholder for starting address
  int currentAddr = 0; // current address

  do
  {
  lRet = readAndParse( lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
  if( lRet != DONE && lRet != EMPTY_LINE )
  {
    // check if the opcode is valid
    if (isOpcode(lOpcode) == -1) {
      printf("Error: invalid opcode\n");
      exit(4);
    }
    // find .orig and get the starting address
    if (strcmp(lOpcode, ".orig" == 0)) {
      if (startAddr != 0) {
        printf("Error: two .orig found\n");
        exit(4);
      }
      if (startAddr % 2 != 0) {
        printf("Error: starting address must be even\n");
        exit(4);
      }
      startAddr = toNum(lArg1);
      currentAddr = startAddr;
    }
    // find .end and get the ending address
    if (strcmp(lOpcode, ".end" == 0)) {
      break;
    }
    // write translated instruction(int) to output file as hex
    fprintf( outfile, "0x%.4X\n", translate(lOpcode, lArg1, lArg2, lArg3, lArg4, currentAddr) );	
    currentAddr += 0x0002; // increment current address
  }

  } while( lRet != DONE );


}


int main(int argc, char* argv[]){
  /* open the source file */
  infile = fopen(argv[1], "r"); // open the input file 
  outfile = fopen(argv[2], "w"); // open the output file, if it doesn't exist, create it
  if (!infile) {
    printf("Error: Cannot open file %s\n", argv[1]);
    exit(4);
  }
  if (!outfile) {
    printf("Error: Cannot open file %s\n", argv[2]);
    exit(4);
  }
  parse_cl(argc, argv); // prints out program name, inputfile, and output file
  
  /* Do stuff with files */
  pass1(argv[1]); // keep track of label/symbol to the symbol table
  pass2(argv[1], argv[2]); // translate each instruction to machine code one by one


  fclose(infile);
  fclose(outfile);
  
  return 0;
}