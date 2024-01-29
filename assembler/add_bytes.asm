
	; Name 1: Hyokwon Chung
	; UTEID 1: hc27426
    
    .ORIG x3000

    LEA R4, ADR   ; R4 has the adress x3100
    LDB R1, R4, x0  ; R1 has the first byte
    LDB R2, R4, x1  ; R2 has the second byte

    ADD R0, R1, R2  ; Perform addition

    BRn NEGATIVE   ; Branch to NEGATIVE if the result is negative
    BRp POSITIVE   ; Branch to POSITIVE if the result is positive
    BRnzp ZERO     ; Branch to ZERO if the result is zero
    

NEGATIVE    STB R0, R4, x2  ; Store sum (R0) in X3102 (X3100 + 2)
    AND R3, R3, #0      ; Prepare 1 at R3 to put in the register
    ADD R3, R3, #1
    STB R3, R4, x3 ; Store 1 in X3103 to indicate overflow
    BR DONE

    
POSITVIE    STB R0, R4, x2  ; Store sum in X3102
    AND R3, R3, #0      ; Prepare 1 at R3 to put in the register
    ADD R3, R3, #1
    STB R3, R4, x3 ; Store 1 in X3103 to indicate overflow
    BR DONE


    
ZERO    STB R0, R4, x2  ; Store sum in X3102
    BR DONE


    ADR .FILL x3100

    DONE
    HALT

    .END
