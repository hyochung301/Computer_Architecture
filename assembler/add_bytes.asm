.ORIG x3000

LEA R4, ADR   ; R4 has the adress x3100
LDB R1, R4, x0  ; R1 has the first byte
LDB R2, R4, x1  ; R2 has the second byte

AND R5, R5, #0 ; Save the signs of R1 and R2 (R5 and R6 is neg indicator)
ADD R5, R1, #0
BRn R1NEGATIVE
BRnzp R1XNEG ;300c

R1NEGATIVE ADD R5, R5, #1 ;300e

R1XNEG AND R6, R6, #0 ;;check R2 negative or not 3010
ADD R6, R2, #0
BRn R2NEGATIVE
BRnzp R2XNEG ;3014

R2NEGATIVE ADD R6, R6, #1

R2XNEG ADD R0, R1, R2  ; Perform addition 3016
STB R0, R4, x2  ; Store sum (R0) in X3102 (X3100 + 2)

NOT R7, R5 ; Check for overflow if R1 and R2 have the same sign
ADD R7, R7, #1
ADD R7, R7, R6
BRz SAMESIGN
BRnzp DONE

SAMESIGN AND R7, R7, #0
ADD R7, R0, #0
BRn R0NEGATIVE
BRzp R0XNEG

R0NEGATIVE ADD R7, R7, #1

R0XNEG NOT R3, R5
ADD R3, R3, #1
ADD R3, R3, R7
BRz DONE
BRnzp OVERFLOW

OVERFLOW AND R3, R3, #0      ; Prepare 1 at R3 to put in the register
ADD R3, R3, #1
STB R3, R4, x3 ; Store 1 in X3103 to indicate overflow
BRnzp DONE


DONE ADD R0, R0, #0 ; Dummy code
HALT
ADR .FILL x3100
.END