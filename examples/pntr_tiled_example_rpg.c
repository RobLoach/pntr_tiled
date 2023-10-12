#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

// just some lil utils to see what is in map
#include "debug_map.h"

typedef struct AppData {
    struct nk_context* ctx;
    cute_tiled_map_t* map;
    int speed;
    int x, y;
} AppData;

// I find it easier to use tile-objects to represent different objects/orientations (in "logic" layer)
typedef enum LogicObjects {
    LOGIC_OBJECTS_DOOR = 132,
    LOGIC_OBJECTS_ENEMY_BAT_E = 159,
    LOGIC_OBJECTS_ENEMY_BAT_N = 165,
    LOGIC_OBJECTS_ENEMY_BAT_S = 147,
    LOGIC_OBJECTS_ENEMY_BAT_W = 153,
    LOGIC_OBJECTS_ENEMY_BLOB_E = 135,
    LOGIC_OBJECTS_ENEMY_BLOB_N = 141,
    LOGIC_OBJECTS_ENEMY_BLOB_S = 123,
    LOGIC_OBJECTS_ENEMY_BLOB_W = 129,
    LOGIC_OBJECTS_ENEMY_GHOST_E = 136,
    LOGIC_OBJECTS_ENEMY_GHOST_N = 142,
    LOGIC_OBJECTS_ENEMY_GHOST_S = 124,
    LOGIC_OBJECTS_ENEMY_GHOST_W = 130,
    LOGIC_OBJECTS_ENEMY_SKELETON_E = 134,
    LOGIC_OBJECTS_ENEMY_SKELETON_N = 140,
    LOGIC_OBJECTS_ENEMY_SKELETON_S = 122,
    LOGIC_OBJECTS_ENEMY_SKELETON_W = 128,
    LOGIC_OBJECTS_ENEMY_SPIDER_E = 158,
    LOGIC_OBJECTS_ENEMY_SPIDER_N = 164,
    LOGIC_OBJECTS_ENEMY_SPIDER_S = 146,
    LOGIC_OBJECTS_ENEMY_SPIDER_W = 152,
    LOGIC_OBJECTS_FIREPLACE = 167,
    LOGIC_OBJECTS_GATE = 126,
    LOGIC_OBJECTS_LAVA = 154,
    LOGIC_OBJECTS_LOOT = 161,
    LOGIC_OBJECTS_PLAYER_E = 133,
    LOGIC_OBJECTS_PLAYER_N = 139,
    LOGIC_OBJECTS_PLAYER_S = 121,
    LOGIC_OBJECTS_PLAYER_W = 127,
    LOGIC_OBJECTS_POT = 125,
    LOGIC_OBJECTS_SIGN = 168,
    LOGIC_OBJECTS_STAIRS_DOWN_E = 150,
    LOGIC_OBJECTS_STAIRS_DOWN_W = 138,
    LOGIC_OBJECTS_STAIRS_UP_E = 144,
    LOGIC_OBJECTS_STAIRS_UP_W = 156,
    LOGIC_OBJECTS_SWITCH_BLUE = 151,
    LOGIC_OBJECTS_SWITCH_GREEN = 163,
    LOGIC_OBJECTS_SWITCH_ORANGE = 157,
    LOGIC_OBJECTS_SWITCH_RED = 145,
    LOGIC_OBJECTS_TORCH_BLUE = 137,
    LOGIC_OBJECTS_TORCH_GREEN = 149,
    LOGIC_OBJECTS_TORCH_ORANGE = 155,
    LOGIC_OBJECTS_TORCH_RED = 143,
    LOGIC_OBJECTS_TORCH_YELLOW = 131,
    LOGIC_OBJECTS_TRAP_FIRE = 166,
    LOGIC_OBJECTS_TRAP_SPIKES = 160,
    LOGIC_OBJECTS_WATER = 148,
    LOGIC_OBJECTS_WELL = 162
} LogicObject;


bool Init(pntr_app* app) {
    AppData* appData = pntr_load_memory(sizeof(AppData));
    pntr_app_set_userdata(app, appData);

    appData->map = pntr_load_tiled("resources/rpg/welcome.tmj");
    appData->x = 0;
    appData->y = 0;
    appData->speed = 200;

    // print_map(appData->map);

    // find the objects and set them up
    cute_tiled_layer_t* layer = appData->map->layers;
    while(layer) {
        printf("LAYER: %s\n", layer->name.ptr);

        // this is the layer where I keep collision-shapes (to block where the player can go)
        // this could also be done with proper tiled collisions, this is just how I set it up.
        if (strcmp(layer->name.ptr, "collision") == 0) {
            // TODO: store collision shapes
        }

        // this is the layer where I keep object-pointers that determine game-logic
        if (strcmp(layer->name.ptr, "logic") == 0) {
            cute_tiled_object_t* o = layer->objects;
            while (o) {
                // this is the tile position
                int posX = (int) (o->x/16.0);
                int posY = (int) (o->y/16.0);

                // for tile-objects, this will tell you the tile (for direction/style)
                // you could actually just use this to determine all the types, and not even set classes/types in editor
                int gid = o->gid;

                printf("  OBJECT (%dx%d) - %d: %s (%s)\n", posX, posY, gid, o->name.ptr, o->type.ptr);

                if (strcmp(o->type.ptr, "player") == 0){
                    // TODO: this does not change the view of the map
                    // set position to player
                    appData->x = (int) o->x;
                    appData->y = (int) o->y;
                    // TODO: track player direction (based on gid)
                }

                if (strcmp(o->type.ptr, "sign") == 0){
                    // TODO: track sign tile/text/position
                    char* text;
                    for (int i=0; i < o->property_count; i++) {
                        cute_tiled_property_t* p = o->properties + i;
                        if (strcmp(p->name.ptr, "text") == 0 && p->type == CUTE_TILED_PROPERTY_STRING){
                            printf("found sign that says: %s\n", p->data.string.ptr);
                        }
                    }

                }

                if (strcmp(o->type.ptr, "enemy") == 0){
                    // TODO: track enemy direction/tileset/position
                }

                if (strcmp(o->type.ptr, "portal") == 0){
                    // TODO: track portal tile/target/position
                }

                if (strcmp(o->type.ptr, "trap") == 0){
                    // TODO: track trap type/position
                }

                if (strcmp(o->type.ptr, "loot") == 0){
                    // TODO: track loot prize/position
                }

                if (strcmp(o->type.ptr, "switch") == 0){
                    // TODO: track switch color/trigger
                }

                if (strcmp(o->type.ptr, "animaton") == 0){
                    // TODO: track animation type/position
                }

                o = o->next;
            }
        }
        layer = layer->next;
    }

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);

    // Keyboard/Gamepad
    if (pntr_app_key_down(app, PNTR_APP_KEY_LEFT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_LEFT)) {
        appData->x += appData->speed * pntr_app_delta_time(app);
    }
    else if (pntr_app_key_down(app, PNTR_APP_KEY_RIGHT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_RIGHT)) {
        appData->x -= appData->speed * pntr_app_delta_time(app);
    }
    if (pntr_app_key_down(app, PNTR_APP_KEY_UP) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_UP)) {
        appData->y += appData->speed * pntr_app_delta_time(app);
    }
    else if (pntr_app_key_down(app, PNTR_APP_KEY_DOWN) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_DOWN)) {
        appData->y -= appData->speed * pntr_app_delta_time(app);
    }

    // Mouse
    if (pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_LEFT)) {
        appData->x -= pntr_app_mouse_delta_x(app);// * pntr_app_delta_time(app) * app->fps;
        appData->y -= pntr_app_mouse_delta_y(app);// * pntr_app_delta_time(app) * app->fps;
    }

    // Keep the map within screen bounds
    if (appData->x > 0) {
        appData->x = 0;
    }
    if (appData->y > 0) {
        appData->y = 0;
    }
    if (appData->x - screen->width < -appData->map->width * appData->map->tilewidth) {
        appData->x = -appData->map->width * appData->map->tilewidth + screen->width;
    }
    if (appData->y - screen->height < -appData->map->height * appData->map->tileheight) {
        appData->y = -appData->map->height * appData->map->tileheight + screen->height;
    }

    // Clear the screen
    pntr_clear_background(screen, PNTR_BLACK);

    // Draw the map
    pntr_draw_tiled(screen, appData->map, appData->x, appData->y, PNTR_WHITE);

    // TODO: draw sprites of objects
    // TODO: handle enemies & collisions

    return true;
}

void Close(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    pntr_unload_tiled(appData->map);
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = 400,
        .height = 225,
        .title = "pntr_tiled: RPG",
        .init = Init,
        .update = Update,
        .close = Close,
        .fps = 60
    };
}
