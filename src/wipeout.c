#include "../MexTK/mex.h"
#include "events.h"

static int respawn_delay[6];
static int respawn_timer[6];

void Event_Init(GOBJ *gobj) {
    for (int i = 0; i < 6; ++i) {
        respawn_delay[i] = 120;
        respawn_timer[i] = 0;
    }
}

void Event_Update() { }

void Event_Think(GOBJ *event) {
    int team_dead[6] = {0,0,0,0,0,0};
    int team_count[6] = {0,0,0,0,0,0};

    for (int ply = 0; ply < 6; ++ply) {
        GOBJ *ft = Fighter_GetGObj(ply);
        if (ft) {
            FighterData *ft_data = ft->userdata;
            team_count[ft_data->team]++;
            
            if (ft_data->state_id == ASID_REBIRTH) {
                team_dead[ft_data->team]++;
                if (respawn_timer[ply] == 0) {
                    ft_data->state_var.state_var1 = respawn_delay[ply];
                    respawn_timer[ply] = respawn_delay[ply];
                    respawn_delay[ply] *= 2;
                }
                else if (respawn_timer > 0) {
                    respawn_timer[ply]--;
                }
            }
        }
    }
    
    int loser_team = -1;
    for (int i = 0; i < 6; ++i) {
        if (team_count[i] != 0 && team_dead[i] == team_count[i]) {
            // team is all dead at same time
            loser_team = i;
            break;
        }
    }
    
    if (loser_team != -1) {
        // end game
        u32 *stats_something = (u32*)0x80479da4;
        for (int ply = 0; ply < 6; ++ply) {
            GOBJ *ft = Fighter_GetGObj(ply);
            if (ft) {
                FighterData *ft_data = ft->userdata;
                if (ft_data->team == loser_team) {
                    Fighter_SetStocks(ply, 0);
                } else {
                    Fighter_SetStocks(ply, 1);
                    stats_something[30] = 1;
                }
                Fighter_GetPlayerblock(ply)->x88 = stc_match->time_frames;
            }
            stats_something += 42;
        }

        Match_EndImmediate();
    }
}

