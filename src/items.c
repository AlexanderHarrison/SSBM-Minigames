#include "../MexTK/mex.h"
#include "events.h"

static int item_indices[6];
static int jumps_used[6];
static GOBJ *items_left2[6];
static GOBJ *items_left[6];
static GOBJ *items[6];
static GOBJ *items_right[6];
static GOBJ *items_right2[6];
static SpawnItem spawn_item;

static const u8 allowed_items[] = {
    ITEM_GREENSHELL,
    ITEM_BEAMSWORD,
    ITEM_HOMERUNBAT,
    ITEM_RAYGUN,
    ITEM_MRSATURN,
    ITEM_FLIPPER,
    ITEM_HAMMER,
    ITEM_SUPERMUSHROOM,
    ITEM_SCREWATTACK,
    ITEM_BUNNYHOOD,
};

#define ITEM_MAX (sizeof(allowed_items)/sizeof(*allowed_items))

int wrap_item(int i) {
    while (i < 0)
        i += ITEM_MAX;
    while (i >= ITEM_MAX)
        i -= ITEM_MAX;
    return i;
}

void spawn(FighterData *data, int item, int pos) {
    spawn_item.it_kind = item;
    spawn_item.facing_direction = data->facing_direction;

    float x = data->phys.pos.X;
    spawn_item.pos.X = x + pos * 10.f;
    spawn_item.pos2.X = x + pos * 10.f;
    float y = data->phys.pos.Y + 20.f;
    spawn_item.pos.Y = y;
    spawn_item.pos2.Y = y;
    
    GOBJ* it = Item_CreateItem(&spawn_item);
    int i = data->ply;
    if (pos == -2)
        items_left2[i] = it;
    else if (pos == -1)
        items_left[i] = it;
    else if (pos == 0)
        items[i] = it;
    else if (pos == 1)
        items_right[i] = it;
    else if (pos == 2)
        items_right2[i] = it;
}

void Event_Think(GOBJ *event) {
    for (int i = 0; i < 6; ++i) {
        GOBJ *ch = Fighter_GetGObj(i);
        if (ch == 0)
            continue;
        FighterData *data = ch->userdata;

        if ((data->input.held & HSD_BUTTON_DPAD_DOWN) == 0) {
            items[i] = 0;
            
            if ((data->input.held_prev & HSD_BUTTON_DPAD_DOWN) != 0) {
                if (data->state_id == ASID_FALLSPECIAL) {
                    Fighter_EnterFall(ch);
                    data->jump.jumps_used = jumps_used[i];
                }
            }
            jumps_used[i] = 0;
            
            GOBJ *it = items_left[i];
            if (it) Item_Destroy(it);
            items_left[i] = 0;
            
            it = items_right[i];
            if (it) Item_Destroy(it);
            items_right[i] = 0;
            
            it = items_left2[i];
            if (it) Item_Destroy(it);
            items_left2[i] = 0;
            
            it = items_right2[i];
            if (it) Item_Destroy(it);
            items_right2[i] = 0;
            
            continue;
        } 

        if (data->phys.air_state == 1) {
            if (ASID_JUMPF <= data->state_id && data->state_id <= ASID_FALLAERIALB) {
                jumps_used[i] = data->jump.jumps_used;
                Fighter_EnterSpecialFall(ch, 0, 0, 0, 0.f, 1.f);
            }
            
            if (jumps_used[i] != 0 && data->state_id == ASID_FALLSPECIAL) { 
                data->phys.self_vel.X /= 1.1f;
                if (data->phys.self_vel.Y < -0.2f)
                    data->phys.self_vel.Y = -0.2f;
            } else {
                return;
            }
        } else {
            if (data->state_id == ASID_WAIT) { 
                ActionStateChange(0.f, 0.f, 0.f, ch, ASID_FURAFURA, 0, 0);
            } else {
                return;
            }
        }

        int item_idx = item_indices[i];
        if (data->input.down & HSD_BUTTON_A)
            item_idx = wrap_item(item_idx + 1);

        if (data->input.down & HSD_BUTTON_B)
            item_idx = wrap_item(item_idx - 1);
        item_indices[i] = item_idx;
        
        GOBJ *item = items[i];
        if (item) Item_Destroy(item);
        
        item = items_left[i];
        if (item) Item_Destroy(item);
        item = items_right[i];
        if (item) Item_Destroy(item);
        item = items_left2[i];
        if (item) Item_Destroy(item);
        item = items_right2[i];
        if (item) Item_Destroy(item);
        
        spawn(data, allowed_items[item_idx], 0);
        spawn(data, allowed_items[wrap_item(item_idx-1)], -1);
        spawn(data, allowed_items[wrap_item(item_idx+1)], +1);
        spawn(data, allowed_items[wrap_item(item_idx-2)], -2);
        spawn(data, allowed_items[wrap_item(item_idx+2)], +2);
    }
}

void Event_Init(GOBJ *event) {
    memset(item_indices, 0, sizeof(item_indices));
    memset(items, 0, sizeof(items));
    memset(&spawn_item, 0, sizeof(spawn_item));
}
