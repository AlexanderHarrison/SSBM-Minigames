#include "../MexTK/mex.h"
#include "events.h"

#define countof(A) (sizeof(A)/sizeof(*A))

static int  BURST_STARTUP_TIMER = 1;
static int  BURST_EFFECT_TIMER = 10;
static int  BURST_EFFECTS[] = {20, 25};
static Vec3 BURST_OFFSET        = (Vec3){0,0,0};
static Vec3 BURST_SIZE          = (Vec3){20,20,20};

typedef struct MeterData {
    int ply;
    int burst_startup_timer;
    int burst_effect_timer;
} MeterData;

void (*Hitbox_StoreHitboxAngle)(ftHit *hit, int angle, GOBJ *gobj) = 0x8007ac9c;
void (*Hitbox_UpdateHitboxPositions)(FighterData *data, ftHit *hit) = 0x8007ad18;
void (*DataOffset_StartHitboxTimer)(FighterData *data) = 0x80080484;

void CreateHitbox(GOBJ *fighter, float size, float damage) {
    FighterData *fighter_data = fighter->userdata;
    ftHit fthit = {
        .active = 1,
        .x4 = 1,
        .dmg = (int)damage,
        .dmg_f = damage,
        .size = size,
        .kb = 100.0,
        .kb_growth = 100.f,
        .bone = fighter->hsd_object,
        .pos = fighter_data->phys.pos,
        .pos_prev = fighter_data->phys.pos,
        .pos_coll = fighter_data->phys.pos,
        .no_reflect = 1,
        .x428 = 1,
        .x422 = 1,
        .x431 = 1,
        .x432 = 1,
        .x40 = 255,
        .x41 = 255 
    };
    Fighter_CreateHitbox(fighter_data, &fthit);
    Fighter_UpdateHitboxDamage(&fthit, (int)damage, fighter);
    Hitbox_StoreHitboxAngle(&fthit, 90, fighter);
    Hitbox_UpdateHitboxPositions(fighter_data, &fthit);
    DataOffset_StartHitboxTimer(fighter_data);
                    
    /*for (int i = 0; i < 6; ++i) {
        GOBJ *victim = Fighter_GetGObj(i);
        if (victim == fighter) continue;
        if (victim == 0) continue;
        FighterData *victim_data = victim->userdata;
        
        Vec3 fighter_pos = fighter_data->phys.pos; 
        Vec3 victim_pos = victim_data->phys.pos;
        float dist = Math_Vec3Distance(&fighter_pos, &victim_pos);
        if (dist > size) continue;
        
        
    }*/
}

void Meter_Think(GOBJ *meter) {
    MeterData *data = meter->userdata;
    GOBJ *fighter = Fighter_GetGObj(data->ply);
    FighterData *fighter_data = fighter->userdata;
    HSD_Pad *pad = PadGet(data->ply, PADGET_MASTER);
    
    if (data->burst_startup_timer >= 0)
        data->burst_startup_timer--;
    if (data->burst_effect_timer >= 0)
        data->burst_effect_timer--;
    
    if (data->burst_startup_timer == 0) {
        data->burst_effect_timer = BURST_EFFECT_TIMER;
        CreateHitbox(fighter, 200.f, 50.f);
    }
    
    if (data->burst_effect_timer > 0) {
        for (int i = 0; i < countof(BURST_EFFECTS); ++i) {
            Effect_SpawnAsyncLookup(
                fighter, BURST_EFFECTS[i], 0, 0, false,
                &BURST_OFFSET, &BURST_SIZE
            );
        }
    }
    
    int buttons = HSD_TRIGGER_L | HSD_TRIGGER_R | HSD_BUTTON_B;
    if ((pad->held & buttons) == buttons) {
        Fighter_EnterFall(fighter);
        data->burst_startup_timer = BURST_STARTUP_TIMER;
    }
}

void Event_Init(GOBJ *gobj) {
    for (int i = 0; i < 6; ++i) {
        GOBJ *fighter = Fighter_GetGObj(i);
        if (fighter) {
            //Vec3 hud_pos = *Match_GetPlayerHUDPos(i);
            GOBJ *meter_bar = GObj_Create(0, 0, 0);
            MeterData *meter_data = HSD_MemAlloc(sizeof(MeterData));
            *meter_data = (MeterData) {
                .ply = i,
                .burst_startup_timer = -1, 
                .burst_effect_timer = -1, 
            };
            GObj_AddUserData(meter_bar, 4, HSD_Free, meter_data);
            GObj_AddProc(meter_bar, Meter_Think, 20);
        }
    }
}

void Event_Think(GOBJ *gobj) {}
