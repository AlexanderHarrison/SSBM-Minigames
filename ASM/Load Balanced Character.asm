# insert at 80085774
# Right before call to load file
# ptr to name in r4

    .include "training-mode/Globals.s"
    .include "m-ex/Header.s"
    
    backup
    
    # ensure enabled
    load r3, 0x803d7054
    lwz r3, 0x0(r3)
    lbz r3, 0x15(r3)
    cmpwi r3, 0
    beq Exit
    
    .set CHAR_COUNT, 11
    .set PlPtr, r31
    .set PlI, r30
    .set PlLd, r29
    .set PlCk, r28
        
    bl BalancedStrs
    .ascii "ClFeGwKbKpLkMrMtNsPcZd"
    .align 2
BalancedStrs:
    mflr PlPtr
    lhz PlLd, 0x2(r4)
    li PlI, CHAR_COUNT
CheckChar:
    lhz PlCk, 0x0(PlPtr)
    
    cmpw PlLd, PlCk
    beq GetBalancedChar

    addi PlPtr, PlPtr, 2
    subi PlI, PlI, 1
    cmpwi PlI, 0
    bne CheckChar
    b Exit

GetBalancedChar:
    bl NewString
    .string "Pl.._bl.dat"
    .align 2
NewString:
    mflr r4
    sth PlLd, 0x2(r4)
    
Exit:
    restore
    li r3, 0

