#include "../MexTK/mex.h"
#include "events.h"

typedef struct {
    int active;
    int count;
    GOBJ *ft[6];
} T;

static T ts[3];

static void Fighter_UpdatePosition(GOBJ *fighter)
{
    FighterData *fighter_data = fighter->userdata;

    // Update Position (Copy Physics XYZ into all ECB XYZ)
    fighter_data->coll_data.topN_Curr.X = fighter_data->phys.pos.X;
    fighter_data->coll_data.topN_Curr.Y = fighter_data->phys.pos.Y;
    fighter_data->coll_data.topN_Prev.X = fighter_data->phys.pos.X;
    fighter_data->coll_data.topN_Prev.Y = fighter_data->phys.pos.Y;
    fighter_data->coll_data.topN_CurrCorrect.X = fighter_data->phys.pos.X;
    fighter_data->coll_data.topN_CurrCorrect.Y = fighter_data->phys.pos.Y;
    fighter_data->coll_data.topN_Proj.X = fighter_data->phys.pos.X;
    fighter_data->coll_data.topN_Proj.Y = fighter_data->phys.pos.Y;

    // Update Collision Frame ID
    fighter_data->coll_data.coll_test = *stc_colltest;

    // Adjust JObj position (code copied from 8006c324)
    JOBJ *fighter_jobj = fighter->hsd_object;
    fighter_jobj->trans.X = fighter_data->phys.pos.X;
    fighter_jobj->trans.Y = fighter_data->phys.pos.Y;
    fighter_jobj->trans.Z = fighter_data->phys.pos.Z;
    JOBJ_SetMtxDirtySub(fighter_jobj);

    // Update Static Player Block Coords
    Fighter_SetPosition(fighter_data->ply, fighter_data->flags.ms, &fighter_data->phys.pos);
}

static FighterData *GetMainFighter(GOBJ *ft) {
    FighterData *ft_data = ft->userdata;
    return Fighter_GetGObj(ft_data->ply);
}

static void Sleep(GOBJ *ft) {
    ft = GetMainFighter(ft);
    Fighter_EnterSleep(ft, 0);
    
    FighterData *ft_data = ft->userdata;
    
    MatchHUDElement *hud = &stc_matchhud->element_data[ft_data->ply];
    hud->is_hidden = true;

    GOBJ *sub = Fighter_GetSubcharGObj(ft_data->ply, 1);
    if (sub)
        Fighter_EnterSleep(sub, 1);
}

static void Wake(GOBJ *ft, GOBJ *inherit) {
    ft = GetMainFighter(ft);
    inherit = GetMainFighter(inherit);

    FighterData *ft_data = ft->userdata;
    FighterData *in_data = inherit->userdata;
    
    MatchHUDElement *hud = &stc_matchhud->element_data[ft_data->ply];
    hud->is_hidden = false;

    Fighter_SetStocks(ft_data->ply, Fighter_GetStocks(in_data->ply));

    ft_data->phys.pos = in_data->phys.pos;
    ft_data->phys.self_vel = in_data->phys.self_vel;
    ft_data->coll_data.ground_index = in_data->coll_data.ground_index;
    Fighter_UpdatePosition(ft);

    ft_data->flags.invisible = false;
    ft_data->flags.x221e_2 = false;
    ft_data->flags.x221e_3 = false;
    ft_data->flags.dead = false;
    ft_data->flags.sleep = false;
    ft_data->flags.immune = false;
    ft_data->camera_subject->kind = 0;

    if (in_data->phys.air_state)
        Fighter_EnterFall(ft);
    else
        Fighter_EnterWait(ft);
}

void Event_Init(GOBJ *gobj) {
    memset(ts, 0, sizeof(ts));

    for (int ply = 0; ply < 6; ++ply) {
        GOBJ *ft = Fighter_GetGObj(ply);
        if (ft) {
            FighterData *ft_data = ft->userdata;
            T *t = &ts[ft_data->team];
            t->ft[t->count++] = ft;
        }
    }
}

static int frame = 0;

void Event_Update() {
    if (frame++ == 80) {
        for (int team = 0; team < 3; ++team) {
            T *t = &ts[team];
            for (int i = 1; i < t->count; ++i)
                Sleep(t->ft[i]);
        }
    }

    for (int team = 0; team < 3; ++team) {
        T *t = &ts[team];
        for (int i = 0; i < t->count; ++i) {
            if (i == t->active) continue;
            
            GOBJ *sleeper = t->ft[i];
            FighterData *data = sleeper->userdata;
            
            HSD_Pad *pad = PadGet(data->pad_index, PADGET_ENGINE);
            
            if (pad->down & HSD_BUTTON_DPAD_DOWN) {
                GOBJ *awake = t->ft[t->active];
                FighterData *awake_data = awake->userdata;

                bool actionable;
                switch (awake_data->state_id) {
                    case ASID_WAIT...ASID_FALLAERIALB:
                    case ASID_DAMAGEFALL...ASID_LANDINGFALLSPECIAL:
                    case ASID_GUARD...ASID_GUARDREFLECT:
                        actionable = true;
                        break;
                    default:
                        actionable = false;
                        break;
                }
                
                if (actionable) {
                    Wake(sleeper, awake);
                    Sleep(awake);
                    t->active = i;
                }
            }
        }
    }
    
    for (int team = 0; team < 3; ++team) {
        T *t = &ts[team];

        int min_stocks = 99;
        for (int i = 0; i < t->count; ++i) {
            GOBJ *ft = t->ft[i];
            FighterData *data = ft->userdata;
            int stocks = Fighter_GetStocks(data->ply);
            if (stocks < min_stocks) min_stocks = stocks;
        }
        
        for (int i = 0; i < t->count; ++i) {
            GOBJ *ft = t->ft[i];
            FighterData *data = ft->userdata;
            Fighter_SetStocks(data->ply, min_stocks);
        }
    }
}

void Event_Think(GOBJ *event) {}

