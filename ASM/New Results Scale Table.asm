# insert at 8017a894

# r0 = 803d7058
# r4 = external character ID * 4
# original: add	r3, r0, r4

# This injection loads the results model scale from a new address.
# The table is placed at 0x803d7058 normally, but mex overwrites it for function pointers,
# so we need to move it!

bl TableEnd
.float 0.85, 0.8
.float 1.0, 1.0, 1.0, 0.7
.float 0.9, 1.0, 1.0, 0.88
.float 0.8, 1.0, 0.9, 1.0
.float 0.9, 1.0, 0.9, 0.9
.float 0.9, 0.9, 1.0, 1.0
.float 1.0, 1.0, 1.0, 0.79
.float 0.0, 0.0, 0.0, 0.0
TableEnd:
    mflr r3
    add r3, r3, r4
    
