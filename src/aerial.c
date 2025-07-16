#include "../MexTK/mex.h"
#include "events.h"

static void swap(void *av, void *bv, int size) {
    u8 *a = av;
    u8 *b = bv;
    for (int i = 0; i < size; ++i) {
        u8 c = a[i];
        a[i] = b[i];
        b[i] = c;
    }
}

swap3(FighterData *d, int a, int b, int c) {
    int i = HSD_Randi(3);
    if (i == 0)
        swap(d->ftaction + a, d->ftaction + b, sizeof(FtAction));
    if (i == 1)
        swap(d->ftaction + a, d->ftaction + c, sizeof(FtAction));
}

// jab 46 47 - 51

// dash 52

// ftilt 53 - 57
// utilt 58
// dtilt 59

// fsmash 65
// usmash 66
// dsmash 67

// nair 68
// fair 69
// bair 70
// uair 71
// dair 72

void ShuffleAerials(GOBJ *ft) {
    FighterData *d = ft->userdata;
    swap3(d, 68, 52, 46);
    swap3(d, 69, 55, 65);
    swap3(d, 71, 58, 66);
    swap3(d, 72, 59, 67);
}

void Event_Init(GOBJ *gobj) {
    for (int ply = 0; ply < 4; ++ply) {
        GOBJ *ft = Fighter_GetGObj(ply);
        if (ft)
            ShuffleAerials(ft);
    }
}
void Event_Think(GOBJ *event) {}

