# 801baa60
# branch from ScenePrep_EventMode_CSS to ScenePrep_VsMode_CSS
#.include "Globals.s"
#mflr r0
#backupall

lis r4, 0x801b
ori r4, r4, 0x14a0
mtctr r4
bctr

#restoreall
