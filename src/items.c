#include "../MexTK/mex.h"
#include "events.h"

static int item_indices[6];
static GOBJ *items[6];

#define ITEM_MAX ITEM_POKEBALL

static SpawnItem spawn_item;

void Event_Think(GOBJ *event) {
    for (int i = 0; i < 6; ++i) {
        GOBJ *ch = Fighter_GetGObj(i);
        if (ch == 0)
            continue;
        FighterData *data = ch->userdata;

        if ((data->input.held & HSD_BUTTON_DPAD_DOWN) == 0) {
            items[i] = 0;
            continue;
        }

        if (data->phys.air_state == 1)
            continue;

        ActionStateChange(0.f, 0.f, 0.f, ch, ASID_FURAFURA, 0, 0);

        int item_idx = item_indices[i];
        if (data->input.down & HSD_BUTTON_A)
            item_idx = item_idx < ITEM_MAX ? item_idx + 1 : 0;

        if (data->input.down & HSD_BUTTON_B)
            item_idx = item_idx > 0 ? item_idx - 1 : ITEM_MAX;
        item_indices[i] = item_idx;

        GOBJ *item = items[i];
        if (item) Item_Destroy(item);

        spawn_item.it_kind = item_idx;
        spawn_item.facing_direction = data->facing_direction;

        float x = data->phys.pos.X;
        spawn_item.pos.X = x;
        spawn_item.pos2.X = x;
        float y = data->phys.pos.Y + 20.f;
        spawn_item.pos.Y = y;
        spawn_item.pos2.Y = y;
        items[i] = Item_CreateItem(&spawn_item);
    }
}
