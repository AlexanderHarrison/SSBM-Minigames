################################################################################
# Address: 0x801bba3c
################################################################################


.include "training-mode/Globals.s"

  backup
  
  # instead of going back to the event screen, go to the new scene we've added
  # 0: css
  # 1: in game 
  # 2: sss
  # 3: results   
  li r3, 0
  branchl r12, Scene_SetNextMinor

  restore
