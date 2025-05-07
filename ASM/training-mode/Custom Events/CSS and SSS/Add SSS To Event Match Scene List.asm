    # To be inserted at 801a44bc
    .include "../../Globals.s"
    .include "../../../m-ex/Header.s"
##################################################

    # Overwrite Event Scene List
    bl EventSceneList
    mflr r3
    load r4, 0x803db010
    stw r3, 0x0(r4)

    # Overwrite SSS_SceneDecide
    bl SSS_SceneDecide
    mflr r4
    stw r4, 0x38(r3)
    
    # Overwrite Results Enter/Exit
    bl FN_ResultsEnter
    mflr r4
    stw r4, 0x4C(r3)
    
    bl FN_ResultsExit
    mflr r4
    stw r4, 0x50(r3)

    # Overwrite MainMenu Scene List
    bl MainMenuSceneList
    mflr r3
    load r4, 0x803dae44
    stw r3, 0x0(r4)

    # Overwrite Credits_SceneDecide
    bl Credits_SceneDecide
    mflr r4
    stw r4, 0x20(r3)
    b exit

###############################################

# Event Scene List Pointer @ 0x803db010
EventSceneList:
    blrl

    # CSS
    .long 0x00030000
    .long 0x801baa60
    .long 0x801baad0
    .long 0x08000000
    .long 0x80497758
    .long 0x80497758

    # In-Match
    .long 0x01030000
    .long 0x801bad70
    .long 0x801bb758
    .long 0x02000000
    .long 0x804978a0
    .long 0x804979d8

    # SSS
    .long 0x02030000
    .long 0x801b1eb8
    .long 0x801b1eec
    .long 0x09000000
    .long 0x80497758
    .long 0x80497758
    
    # Results
    .long 0x03030000
    .long 0x801b16a8 # prep/enter function
    .long 0x801b16c8 # decide/exit function
    .long 0x05000000 # minor scene type
    .long 0x8047c020 # prep/enter static data
    .long 0x00000000 # decide/exit static data

    .long 0xFF000000
    
#############################
    
FN_ResultsEnter:
  blrl

  backup
  # branchl r12, 0x801b16a8 # original results enter
  
  bl FN_CopyMatchData

  restore
  blr

######

FN_ResultsExit:
  blrl

  backup
  branchl r12, 0x801b16c8 # original results exit
  
  # Go back to CSS
  #li r3, 0
  #branchl r12, Scene_SetNextMinor

  restore
  blr

#####
# This is a direct copy from the original code,
# except we replace the match data (r3) with the event match data.
# We could replace the exit data pointer in the minor array to the one 
# in the original code, but we would have to make sure
# all of the other references to that data are replaced as well
FN_CopyMatchData:
  backup

  branchl r12, 0x801A427C
  mr	r31, r3
  branchl r12, 0x80177724
  li	r0, 1103
  mtctr	r0

  # lis	r3, 0x8048
  # subi	r3, r3, 25192
  load r3, 0x804979d8 # event match data

  addi	r5, r31, 0
  addi	r4, r3, 4

  COPY_LOOP:
    lwzu	r3, 0x0008 (r4)
    lwz	r0, 0x0004 (r4)
    stwu	r3, 0x0008 (r5)
    stw	r0, 0x0004 (r5)
    bdnz+	COPY_LOOP

  restore
  blr	


################################

SSS_SceneDecide:
    blrl
    backup

    # Get Minor Scenes Data Pointer
    lwz r3, 0x0014(r3)

    # Get Method of Leaving SSS
    lbz r0, 0x0004(r3)
    cmpwi r0, 0x0
    beq SSS_SceneDecide_BackToCSS

SSS_SceneDecide_AdvanceToMatch:
    # Match Minor ID
    li r3, 0x2
    b SSS_SceneDecide_Exit

SSS_SceneDecide_BackToCSS:
    li r3, 0x1

SSS_SceneDecide_Exit:
    # Store Next Minor
    load r4, SceneController
    stb r3, 0x5(r4)

    restore
    blr

################################

################################
MainMenuSceneList:
    blrl

    # MainMenu
    .long 0x00020000
    .long 0x801b0ff8
    .long 0x801b138c
    .long 0x01000000
    .long 0x804d68b8
    .long 0x804d68bc

    # Credits
    .long 0x01030000
    .long 0x00000000
    .long 0xFFFFFFFF
    .long 0x2B000000
    .long 0x00000000
    .long 0x00000000

    .long 0xFF000000

################################
Credits_SceneDecide:
    blrl

    # Store Next Minor
    load r4, SceneController
    li r3, 0x0
    stb r3, 0x5(r4)

    blr

#################################

exit:
    mr r3, r30
