#include "../MexTK/mex.h"
#include "events.h"

static bool in_spawn_mode[6];
static int item_indices[6];
static int jumps_used[6];
static GOBJ *items_left2[6];
static GOBJ *items_left[6];
static GOBJ *items[6];
static GOBJ *items_right[6];
static GOBJ *items_right2[6];
static SpawnItem spawn_item;

static u8 allowed_items[32];
static int item_count;

static const u8 DEFAULT_ITEMS[] = {
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

static const u8 ITEM_LOOKUP[] = {
    ITEM_BOBOMB,
    ITEM_HOMERUNBAT,
    ITEM_MRSATURN,
    ITEM_FIREFLOWER,
    ITEM_FLIPPER,
    ITEM_FOOD,
    ITEM_FREEZIE,
    ITEM_GREENSHELL,
    ITEM_HAMMER,
    ITEM_FAN,
    ITEM_HEARTCONTAINER,
    ITEM_SUPERMUSHROOM,
    ITEM_PARTYBALL,
    ITEM_RAYGUN,
    ITEM_LIPSSTICK,
    ITEM_POKEBALL,
    ITEM_METALBOX,
    ITEM_MOTIONSENSORBOMB,
    ITEM_MAXIMTOMATO,
    ITEM_CLOAKINGDEVICE,
    ITEM_PARASOL,
    ITEM_REDSHELL,
    ITEM_BUNNYHOOD,
    ITEM_STARROD,
    ITEM_SUPERSCOPE,
    ITEM_SCREWATTACK,
    ITEM_STARMAN,
    ITEM_BEAMSWORD,
    255,
    ITEM_BARRELCANNON,
    ITEM_WARPSTAR,
    ITEM_POISONMUSHROOM,
};

/*poison mushroom 80 00 00 00
warp 40 00 00 00
barrel cannon 20 00 00 00
beam sword 8 00 00 00
starman 4 00 00 00
screw attack 2 00 00 00
super scope 1 00 00 00
star rod 80 00 00
bunny hood 40 00 00
red shell 20 00 00
parasol 10 00 00
cloaking 8 00 00
tomato 4 00 00
motion sensor bomb 2 00 00
metal box 1 00 00
pokeball 80 00
lips stick 40 00
ray gun 20 00
party ball 10 00
super mushroom 8 00
heart container 4 00
fan 2 00
hammer 100
green shell 80
freezie 40
food 20
flipper 10
fire flower 8
saturn 4
bat 2
bobomb 1*/

static int wrap_item(int i) {
    while (i < 0)
        i += item_count;
    while (i >= item_count)
        i -= item_count;
    return i;
}

static void spawn(FighterData *data, int item, int pos) {
    spawn_item.it_kind = item;
    spawn_item.facing_direction = data->facing_direction;

    float x = data->phys.pos.X;
    spawn_item.pos.X = x + pos * 10.f;
    spawn_item.pos2.X = x + pos * 10.f;
    float y = data->phys.pos.Y + 20.f - pow(fabs((float)pos * 1.f), 2);
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
        
    if (pos == 0) {
        Vec3 offset = {0, 20.f, 0};
        Vec3 range = {5, 5, 5};
        Effect_SpawnAsyncLookup(data->fighter, 56, 0, 0, false, &offset, &range);
    }
}

void Event_Think(GOBJ *event) {
    for (int i = 0; i < 6; ++i) {
        GOBJ *ch = Fighter_GetGObj(i);
        if (ch == 0)
            continue;
        FighterData *data = ch->userdata;
        
        if (!in_spawn_mode[i]) {
            // NOT in item spawn mode
            
            if (data->input.held & HSD_BUTTON_DPAD_DOWN) {
                if (ASID_JUMPF <= data->state_id && data->state_id <= ASID_FALLAERIALB) {
                    jumps_used[i] = data->jump.jumps_used;
                    data->jump.jumps_used = data->attr.max_jumps;
                    ActionStateChange(0.f, 0.f, 0.f, ch, ASID_FALLSPECIAL, 0, 0);
                    in_spawn_mode[i] = true;
                } else if (data->state_id == ASID_WAIT) {
                    ActionStateChange(0.f, 0.f, 0.f, ch, ASID_SLEEP, 0, 0);
                    in_spawn_mode[i] = true;
                }
            }
        }
        
        if (in_spawn_mode[i]) {
            // in item spawn mode
            
            bool forced_exit;
            if (data->phys.air_state == 0)
                forced_exit = data->state_id != ASID_SLEEP;
            else
                forced_exit = data->state_id != ASID_FALLSPECIAL;
            int normal_exit = (data->input.held & HSD_BUTTON_DPAD_DOWN) == 0;
            
            GOBJ *it;
            if (forced_exit) {
                it = items[i];
                if (it) Item_Destroy(it);
            }
            
            if (normal_exit & !forced_exit) {
                if (data->state_id == ASID_FALLSPECIAL) {
                    Fighter_EnterFall(ch);
                    data->jump.jumps_used = jumps_used[i];
                } else if (data->state_id == ASID_SLEEP) {
                    Fighter_EnterWait(ch);
                }
            }
            
            if (normal_exit | forced_exit) {
                in_spawn_mode[i] = false;
                items[i] = 0;
                
                it = items_left[i];
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
            }
            
            if (!normal_exit & !forced_exit) {
                data->phys.self_vel.X /= 1.1f;
                if (data->phys.air_state == 0) {
                    ActionStateChange(0.f, 0.f, 0.f, ch, ASID_SLEEP, 0, 0);
                } else {
                    if (data->phys.self_vel.Y < -0.2f)
                        data->phys.self_vel.Y = -0.2f;
                    ActionStateChange(0.f, 0.f, 0.f, ch, ASID_FALLSPECIAL, 0, 0);
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
    }
}
u32 *LoadRulesSettingsPointer4(void);

void Event_Init(GOBJ *event) {
    event_vars = *event_vars_ptr;
    
    u32 *rules = LoadRulesSettingsPointer4();
    u32 spawn_items = rules[3];
    item_count = 0;
    for (u32 i = 0; i < 32; ++i) {
        if ((spawn_items >> i) & 1) {
            u8 it = ITEM_LOOKUP[i];
            if (it != 255)
                allowed_items[item_count++] = it;
        }
    }
    
    if (item_count == 0) {
        item_count = sizeof(DEFAULT_ITEMS)/sizeof(DEFAULT_ITEMS[0]);
        memcpy(allowed_items, DEFAULT_ITEMS, sizeof(DEFAULT_ITEMS));
    }
    
    memset(in_spawn_mode, 0, sizeof(in_spawn_mode));
    memset(item_indices, 0, sizeof(item_indices));
    memset(items_left, 0, sizeof(items_left));
    memset(items_left2, 0, sizeof(items_left2));
    memset(items, 0, sizeof(items));
    memset(items_right, 0, sizeof(items_right));
    memset(items_right2, 0, sizeof(items_right2));
    memset(jumps_used, 0, sizeof(jumps_used));
}
