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

// Convert a String To a Number
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

#define MAX_LINE_LENGTH 255
enum
{
    DONE, OK, EMPTY_LINE
};

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
)
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



int pass1(int argc, char* argv[]) {
  // variables for readAndParse()
  char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
        *lArg2, *lArg3, *lArg4;
  int lRet;
  int linecount = 0;
  /* first pass */
  /* keep track of label/symbol to the symbol table */

  do
  {
  lRet = readAndParse( infile, lLine, &lLabel,
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

int pass2(int argc, char* argv[]) {
  /* second pass */
  /* translate each instruction to machine code one by one */

  // variables for readAndParse()
  char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
        *lArg2, *lArg3, *lArg4;
  int lRet;

  int lInstr = 0;
  // find .orig and get the starting address
  do
  {
  lRet = readAndParse( infile, lLine, &lLabel,
      &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
  if( lRet != DONE && lRet != EMPTY_LINE )
  {
    /// ...
  }
  } while( lRet != DONE );

	fprintf( outfile, "0x%.4X\n", lInstr );	//  where lInstr is declared as an int 

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
  pass1(argc, argv); // keep track of label/symbol to the symbol table
  pass2(argc, argv); // translate each instruction to machine code one by one


  fclose(infile);
  fclose(outfile);
  
  return 0;
}