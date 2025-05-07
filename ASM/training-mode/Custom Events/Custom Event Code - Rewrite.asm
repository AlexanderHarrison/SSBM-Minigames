    # To be inserted at 801bb128
    .include "../Globals.s"
    .include "../../m-ex/Header.s"

    # Check if event is legacy (no file)
    lwz r3, MemcardData(r13)
    lbz r3, CurrentEventPage(r3)
    mr r4, r25                                          # event
    mr r5, r26                                          # match struct
    rtocbl r12, TM_GetEventFile
    cmpwi r3, 0
    beq LegacyEvent

    # Branch to C function to initialize the event
    lwz r3, MemcardData(r13)
    lbz r3, CurrentEventPage(r3)
    mr r4, r25                                          # event
    mr r5, r26                                          # match struct
    rtocbl r12, TM_EventInit
    branch r12, 0x801bb738

###########################################
exit:
    li r0, 3
