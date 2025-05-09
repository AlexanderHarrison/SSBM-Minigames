# 801baad0
# branch from SceneDecide_EventMode_CSS to SceneDecide_VsMode_CSS

.include "Globals.s"
mflr r0
backupall

lis r4, 0x801b
ori r4, r4, 0x14dc
mtctr r4
bctrl

restoreall
