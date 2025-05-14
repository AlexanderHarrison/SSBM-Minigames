################################################################################
# Address: 0x801bba3c
################################################################################


.include "training-mode/Globals.s"

  backup
  
  #mr r3, r27
  #branchl r12, 0x801b15c8 # SceneDecide_EventMode_InGame
    
  # instead of going back to the event screen, go to the new scene we've added
  # 0: css
  # 1: in game 
  # 2: sss
  # 3: results   
  li r3, 3
  branchl r12, Scene_SetNextMinor

  restore
