#include "../MexTK/mex.h"
#include "events.h"

void Event_Think(GOBJ *event) {
    for (int i = 0; i < 6; ++i) {
        GOBJ *player = Fighter_GetGObj(i);
        if (player == 0) continue;
        
        FighterData *player_data = player->userdata;

        if (player_data->flags.hitlag == 0) continue;
        if (player_data->flags.is_fastfall == 1) continue;
        if (player_data->phys.self_vel.Y <= 0.f) continue;
        if (player_data->input.lstick.Y >= -0.3f) continue;

        int state = player_data->state_id;
        if (state < ASID_ATTACKAIRN || ASID_ATTACKAIRLW < state) continue;

        player_data->phys.self_vel.Y = -0.01f;
        player_data->flags.is_fastfall = 1;
    }
}
