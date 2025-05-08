#include "../MexTK/mex.h"
#include "events.h"

GOBJ *monkey;
GOBJ *monkey_queued;
int player_count = 0;
GOBJ *players[6];

GOBJ *dk;

void Event_PostThink(GOBJ *event);

void (*fileLoad_PlXX)(int ftkind) = (void*)0x8008572c;
void (*loadFile_CharCostumeDat)(int ftkind, int costume) = (void*)0x80085820;

struct idk {
    int ftkind;   
    u8 port_maybe;
    u8 unk; // set as -1
    u8 flags_unk;   
};

void Event_Init(GOBJ *gobj) {
    monkey = 0;
    monkey_queued = 0;
    player_count = 0;
    memset(players, 0, sizeof(players));
    dk = 0;

    stc_memcard->TM_OSDEnabled = 0;

    int initial_monkey_idx = -1;

    for (int i = 0; i < 6; ++i) {
        GOBJ *player = Fighter_GetGObj(i);
        if (player == 0) continue;
        FighterData *player_data = player->userdata;
        HSD_Pad *pad = PadGet(i, PADGET_MASTER);
        if (pad->held & HSD_BUTTON_B)
            initial_monkey_idx = player_count;
        players[player_count++] = player;

        Playerblock *pb = Fighter_GetPlayerblock(player_data->ply);
        pb->selfDestructs = 0;
    }
    
    {
        dk = GObj_Create(3, 0, 0);
        GObj_AddGXLink(dk, GXLink_Common, 5, 0);
        //fileLoad_PlXX(FTKIND_DK);
        loadFile_CharCostumeDat(FTKIND_DK, 0);

        JOBJDesc *jobjdesc = **(JOBJDesc***)(0x803C0EC0 + FTKIND_DK * 8);
        Fighter_InitPObj();
        JOBJ *jobj = JOBJ_LoadJoint(jobjdesc);
        GObj_AddObject(dk, 3, jobj);
    }

    if (initial_monkey_idx == -1)
        initial_monkey_idx = HSD_Randi(player_count);
    monkey = players[initial_monkey_idx];
    GObj_AddProc(gobj, Event_PostThink, 20);
}

void Event_Update() { }

static int end_timer = -1;
void Event_Think(GOBJ *event) {
    int alive_count = 0;
    GOBJ *dead = 0;
    for (int i = 0; i < player_count; ++i) {
        GOBJ *player = players[i];
        FighterData *player_data = player->userdata;
        if (Fighter_GetStocks(player_data->ply) > 0)
            alive_count++;
        else
            dead = player;
    }

    int any_dead = false;

    if (alive_count < 3) {
        GOBJ *loser = dead;

        if (end_timer == -1) {
            end_timer = 0;

            // 8ball rule check
            if (monkey != dead && dead != 0) {
                FighterData *dead_data = dead->userdata;
                int killer_ply = dead_data->dmg.source_ply;
                if (killer_ply != 6) {
                    GOBJ *killer = Fighter_GetGObj(killer_ply);
                    if (killer != monkey) {
                        // was killed by teammate -> teammate loses
                        end_timer = 80;
                        loser = killer;
                        SFX_Play(40008); // FAILURE
                    }
                }
            }
        }

        if (end_timer == 0) {
            // Found by reading MatchStats_CalculateKOStars
            u32 *stats_something = (u32*)0x80479da4;
            for (int i = 0; i < 6; ++i) {
                GOBJ *player = Fighter_GetGObj(i);
                if (player != 0 && player != loser)
                    stats_something[30] = 1;
                stats_something += 42;
            }
        
            Match_EndImmediate();
        } else {
            end_timer--;
        }
    } else {
        for (int i = 0; i < player_count; ++i) {
            GOBJ *player = players[i];
            if (player == monkey)
                continue;

            FighterData *player_data = player->userdata;
            Playerblock *pb = Fighter_GetPlayerblock(player_data->ply);

            int state = player_data->state_id;
            int prev_state = player_data->TM.state_prev[0];
            int is_dead = ASID_DEADDOWN <= state && state <= ASID_DEADUPFALLHITCAMERAICE;
            int prev_dead = ASID_DEADDOWN <= prev_state && prev_state <= ASID_DEADUPFALLHITCAMERAICE;
            int newly_dead = is_dead && !prev_dead;
            //int out_of_game = Fighter_GetStocks(player_data->ply) == 0;
            any_dead |= is_dead || state == ASID_REBIRTHWAIT;

            if (monkey && newly_dead && monkey_queued == 0) {
                FighterData *monkey_data = monkey->userdata;

                int killer = player_data->dmg.source_ply;

                if (killer == monkey_data->ply || killer == 6) {
                    monkey_queued = player;
                } else {
                    monkey_queued = Fighter_GetGObj(killer);
                }

                monkey = 0;
            }
        }
    }

    static int effect_timer = 0;
    if (monkey_queued != 0 && !any_dead) {
        if (monkey != monkey_queued)
            effect_timer = 20;
        monkey = monkey_queued;
        monkey_queued = 0;
    }

    if (effect_timer && monkey) {
        Vec3 offset = {0, 0, 0};
        Vec3 range = {5, 5, 5};
        Effect_SpawnAsyncLookup(monkey, 97, 0, 0, false, &offset, &range);
        effect_timer--;
    }

    if (monkey) {
        JOBJ *dk_jobj = dk->hsd_object;
        JOBJ_ClearFlagsAll(dk_jobj, JOBJ_HIDDEN);

        FighterData *monkey_data = monkey->userdata;
        int head_bone = monkey_data->ftData->modelLookup[0];
        JOBJ *head;
        JOBJ_GetChild(monkey->hsd_object, &head, head_bone, -1);

        JOBJ_Detach(dk_jobj);
        JOBJ_AttachPosition(dk_jobj, head);
        dk_jobj->scale = (Vec3) {0.2f, 0.2f, 0.2f};
        dk_jobj->child->trans = (Vec3) {0.f, 100.f, 0.f};

        static float rot = 0.f;
        rot += 0.1f;
        if (rot > M_PI)
            rot -= M_PI * 2.f;
        dk_jobj->child->rot.Y = rot;

        JOBJ_SetMtxDirtySub(dk_jobj);

        Vec3 offset = {0, 20.f, 0};
        Vec3 range = {1.f, 1.f, 1.f};
        Effect_SpawnAsyncLookup(monkey, 56, head_bone, 0, false, &offset, &range);
    } else {
        JOBJ *dk_jobj = dk->hsd_object;
        JOBJ_SetFlagsAll(dk_jobj, JOBJ_HIDDEN);
    }
}

void Event_PostThink(GOBJ *event) {
    //if (end) {
    //        for (int i = 0; i < player_count; ++i) {
    //            GOBJ *player = players[i];

    //            //ActionStateChange(1.0, 1.0, 1.0, player, ASID_FALL, 0, 0);
    //            //Fighter_SetScale(player, 10.0);

    //            //int placement = player == loser ? 0 : placement_idx++;
    //            //stc_match->standings.ply_standings[i].placement = placement;
    //        }
    //}
    //if (end)
    //    Match_EndImmediate();

    GXColor monkey_colour = Fighter_GetPlyHUDColor(0);
    GXColor team_colour = Fighter_GetPlyHUDColor(1);

    static int alpha = 200;
    if (alpha == 0) alpha = 200;
    else alpha -= 4;

    for (int i = 0; i < player_count; ++i) {
        GOBJ *player = players[i];
        FighterData *player_data = player->userdata;

        MatchHUDElement *insig = &stc_matchhud->element_data[player_data->ply];

        GXColor c;
        if (player == monkey) {
            c = monkey_colour;
        } else {
            c = team_colour;

            //memset(&player_data->color[1], 0, sizeof(ColorOverlay));
            //memset(&player_data->color[0], 0, sizeof(ColorOverlay));
            //player_data->color[1].hex = c;
            //player_data->color[1].hex.a = alpha;
            //player_data->color[1].color_enable = 1;
        }

        JOBJ *j = insig->insignia->hsd_object;
        j->child->dobj->mobj->mat->diffuse = c;

    }
}
