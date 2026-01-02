#include "../MexTK/mex.h"
#include "events.h"

struct swapped_attrs {
    float walk_initial_velocity;                           // 0x110
    float walk_acceleration;                               // 0x114
    float walk_maximum_velocity;                           // 0x118
    float slow_walk_max;                                   // 0x11C
    float mid_walk_point;                                  // 0x120
    float fast_walk_min;                                   // 0x124
    float ground_friction;                                 // 0x128
    float dash_initial_velocity;                           // 0x12C
    float dashrun_acceleration_a;                          // 0x130
    float dashrun_acceleration_b;                          // 0x134
    float dashrun_terminal_velocity;                       // 0x138
    float run_animation_scaling;                           // 0x13C
    float max_runbrake_frames;                             // 0x140
    float grounded_max_horizontal_velocity;                // 0x144
    float jump_startup_time;                               // 0x148
    float jump_h_initial_velocity;                         // 0x14C
    float jump_v_initial_velocity;                         // 0x150
    float ground_to_air_jump_momentum_multiplier;          // 0x154
    float jump_h_max_velocity;                             // 0x158
    float hop_v_initial_velocity;                          // 0x15C
    float air_jump_v_multiplier;                           // 0x160
    float air_jump_h_multiplier;                           // 0x164
    float gravity;                                         // 0x16C
    float terminal_velocity;                               // 0x170
    float aerial_drift_stick_mult;                         // 0x174
    float aerial_drift_base;                               // 0x178
    float aerial_drift_max;                                // 0x17C
    float aerial_friction;                                 // 0x180
    float fastfall_velocity;                               // 0x184
    float horizontal_air_mobility_constant;                // 0x188
} attr_cache[4];
int attr_cache_count;

void GetAttrs(GOBJ *ft, struct swapped_attrs *dst) {
    FighterData *data = ft->userdata;
    
    dst->walk_initial_velocity                  = data->attr.walk_initial_velocity                 ;
    dst->walk_acceleration                      = data->attr.walk_acceleration                     ;
    dst->walk_maximum_velocity                  = data->attr.walk_maximum_velocity                 ;
    dst->slow_walk_max                          = data->attr.slow_walk_max                         ;
    dst->mid_walk_point                         = data->attr.mid_walk_point                        ;
    dst->fast_walk_min                          = data->attr.fast_walk_min                         ;
    dst->ground_friction                        = data->attr.ground_friction                       ;
    dst->dash_initial_velocity                  = data->attr.dash_initial_velocity                 ;
    dst->dashrun_acceleration_a                 = data->attr.dashrun_acceleration_a                ;
    dst->dashrun_acceleration_b                 = data->attr.dashrun_acceleration_b                ;
    dst->dashrun_terminal_velocity              = data->attr.dashrun_terminal_velocity             ;
    dst->run_animation_scaling                  = data->attr.run_animation_scaling                 ;
    dst->max_runbrake_frames                    = data->attr.max_runbrake_frames                   ;
    dst->grounded_max_horizontal_velocity       = data->attr.grounded_max_horizontal_velocity      ;
    dst->jump_startup_time                      = data->attr.jump_startup_time                     ;
    dst->jump_h_initial_velocity                = data->attr.jump_h_initial_velocity               ;
    dst->jump_v_initial_velocity                = data->attr.jump_v_initial_velocity               ;
    dst->ground_to_air_jump_momentum_multiplier = data->attr.ground_to_air_jump_momentum_multiplier;
    dst->jump_h_max_velocity                    = data->attr.jump_h_max_velocity                   ;
    dst->hop_v_initial_velocity                 = data->attr.hop_v_initial_velocity                ;
    dst->air_jump_v_multiplier                  = data->attr.air_jump_v_multiplier                 ;
    dst->air_jump_h_multiplier                  = data->attr.air_jump_h_multiplier                 ;
    dst->gravity                                = data->attr.gravity                               ;
    dst->terminal_velocity                      = data->attr.terminal_velocity                     ;
    dst->aerial_drift_stick_mult                = data->attr.aerial_drift_stick_mult               ;
    dst->aerial_drift_base                      = data->attr.aerial_drift_base                     ;
    dst->aerial_drift_max                       = data->attr.aerial_drift_max                      ;
    dst->aerial_friction                        = data->attr.aerial_friction                       ;
    dst->fastfall_velocity                      = data->attr.fastfall_velocity                     ;
    dst->horizontal_air_mobility_constant       = data->attr.horizontal_air_mobility_constant      ;
}

void SetAttrs(GOBJ *ft, struct swapped_attrs *src) {
    FighterData *data = ft->userdata;
    
    data->attr.walk_initial_velocity                  = src->walk_initial_velocity                 ;
    data->attr.walk_acceleration                      = src->walk_acceleration                     ;
    data->attr.walk_maximum_velocity                  = src->walk_maximum_velocity                 ;
    data->attr.slow_walk_max                          = src->slow_walk_max                         ;
    data->attr.mid_walk_point                         = src->mid_walk_point                        ;
    data->attr.fast_walk_min                          = src->fast_walk_min                         ;
    data->attr.ground_friction                        = src->ground_friction                       ;
    data->attr.dash_initial_velocity                  = src->dash_initial_velocity                 ;
    data->attr.dashrun_acceleration_a                 = src->dashrun_acceleration_a                ;
    data->attr.dashrun_acceleration_b                 = src->dashrun_acceleration_b                ;
    data->attr.dashrun_terminal_velocity              = src->dashrun_terminal_velocity             ;
    data->attr.run_animation_scaling                  = src->run_animation_scaling                 ;
    data->attr.max_runbrake_frames                    = src->max_runbrake_frames                   ;
    data->attr.grounded_max_horizontal_velocity       = src->grounded_max_horizontal_velocity      ;
    data->attr.jump_startup_time                      = src->jump_startup_time                     ;
    data->attr.jump_h_initial_velocity                = src->jump_h_initial_velocity               ;
    data->attr.jump_v_initial_velocity                = src->jump_v_initial_velocity               ;
    data->attr.ground_to_air_jump_momentum_multiplier = src->ground_to_air_jump_momentum_multiplier;
    data->attr.jump_h_max_velocity                    = src->jump_h_max_velocity                   ;
    data->attr.hop_v_initial_velocity                 = src->hop_v_initial_velocity                ;
    data->attr.air_jump_v_multiplier                  = src->air_jump_v_multiplier                 ;
    data->attr.air_jump_h_multiplier                  = src->air_jump_h_multiplier                 ;
    data->attr.gravity                                = src->gravity                               ;
    data->attr.terminal_velocity                      = src->terminal_velocity                     ;
    data->attr.aerial_drift_stick_mult                = src->aerial_drift_stick_mult               ;
    data->attr.aerial_drift_base                      = src->aerial_drift_base                     ;
    data->attr.aerial_drift_max                       = src->aerial_drift_max                      ;
    data->attr.aerial_friction                        = src->aerial_friction                       ;
    data->attr.fastfall_velocity                      = src->fastfall_velocity                     ;
    data->attr.horizontal_air_mobility_constant       = src->horizontal_air_mobility_constant      ;
}

void Event_Think(GOBJ *event) {
    static bool thunk = false;
    if (!thunk) {
        attr_cache_count = 0;
        thunk = true;

        for (int ply = 0; ply < 4; ++ply) {
            GOBJ *ft = Fighter_GetGObj(ply);
            if (ft)
                GetAttrs(ft, &attr_cache[attr_cache_count++]);
        }
    }

    int i = 0;
    for (int ply = 0; ply < 4; ++ply) {
        GOBJ *ft = Fighter_GetGObj(ply);
        if (ft)
            SetAttrs(ft, &attr_cache[++i % attr_cache_count]);
    }
}

